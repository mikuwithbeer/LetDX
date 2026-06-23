#include "let/network/server.h"

#include <sys/time.h>
#include <unistd.h>
#include <errno.h>

let_network_server_t let_network_server_empty(void) {
    let_network_server_t network_server = {};
    network_server.handle = -1;

    return network_server;
}

let_error_t let_network_server_init(let_network_server_t *network_server,
                                    const let_u16_t port,
                                    const let_u16_t backlog) {
    network_server->port = port;
    network_server->handle = socket(AF_INET, SOCK_STREAM, 0);
    if (network_server->handle < 0) {
        return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_SERVER_CREATE_FAILED);
    }

    constexpr auto option_value = 1;
    setsockopt(network_server->handle, SOL_SOCKET, SO_REUSEADDR, &option_value, sizeof(option_value));

    network_server->address = (typeof_unqual(network_server->address)){};
    network_server->address.sin_family = AF_INET;
    network_server->address.sin_addr.s_addr = INADDR_ANY;
    network_server->address.sin_port = htons(network_server->port);

    const auto bind_result = bind(
        network_server->handle,
        (struct sockaddr *) &network_server->address,
        sizeof(network_server->address));

    if (bind_result < 0) {
        close(network_server->handle);
        network_server->handle = -1;
        return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_SERVER_BIND_FAILED);
    }

    const auto listen_result = listen(network_server->handle, backlog);

    if (listen_result < 0) {
        close(network_server->handle);
        network_server->handle = -1;
        return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_SERVER_LISTEN_FAILED);
    }

    return let_error_none();
}

let_error_t let_network_server_accept(const let_network_server_t *network_server,
                                      const let_u32_t read_timeout,
                                      const let_u32_t write_timeout,
                                      let_network_server_t *network_client) {
    socklen_t client_address_length = sizeof(network_client->address);

    const auto client_handle = accept(
        network_server->handle,
        (struct sockaddr *) &network_client->address,
        &client_address_length);

    if (client_handle < 0) {
        if (errno == EINTR) {
            return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_SERVER_CLOSED);
        }

        return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_SERVER_ACCEPT_FAILED);
    }

    const struct timeval network_read_timeout = {
        .tv_sec = read_timeout,
        .tv_usec = 0
    };

    const struct timeval network_write_timeout = {
        .tv_sec = write_timeout,
        .tv_usec = 0
    };

    setsockopt(client_handle, SOL_SOCKET, SO_RCVTIMEO, &network_read_timeout, sizeof(network_read_timeout));
    setsockopt(client_handle, SOL_SOCKET, SO_SNDTIMEO, &network_write_timeout, sizeof(network_write_timeout));

    network_client->handle = client_handle;
    return let_error_none();
}

let_error_t let_network_client_read(let_network_server_t *network_client,
                                    let_network_request_t *request) {
    let_u8_t read_byte = 0;

    for (network_client->read_buffer_index = 0
         ; network_client->read_buffer_index < LET_NETWORK_BUFFER_LENGTH
         ; network_client->read_buffer_index++) {
        const auto read_result = recv(network_client->handle, &read_byte, sizeof(read_byte), 0);

        if (read_result == 0) {
            return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_CLIENT_CLOSED);
        }

        if (read_result < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_SERVER_READ_TIMEOUT);
            }

            if (errno == EINTR) {
                return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_SERVER_CLOSED);
            }

            return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_SERVER_READ_FAILED);
        }

        if (read_byte == '\n') {
            network_client->read_buffer[network_client->read_buffer_index++] = ' ';
            break;
        }

        network_client->read_buffer[network_client->read_buffer_index] = read_byte;
    }

    if (network_client->read_buffer_index == LET_NETWORK_BUFFER_LENGTH && read_byte != '\n') {
        return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_REQUEST_BUFFER_OVERFLOW);
    }

    return let_network_request_decode(network_client->read_buffer, network_client->read_buffer_index, request);
}

let_error_t let_network_client_write(let_network_server_t *network_client,
                                     const let_network_response_t response) {
    let_size_t response_length = 0;

    const auto encode_result = let_network_response_encode(
        response,
        network_client->write_buffer,
        LET_NETWORK_BUFFER_LENGTH,
        &response_length);

    if (let_error_exists(encode_result)) {
        return encode_result;
    }

    let_i64_t sent_bytes = 0;
    for (network_client->write_buffer_index = 0
         ; network_client->write_buffer_index < response_length
         ; network_client->write_buffer_index += (size_t) sent_bytes) {
        const auto remaining_bytes = response_length - network_client->write_buffer_index;

        sent_bytes = send(
            network_client->handle,
            network_client->write_buffer + network_client->write_buffer_index,
            remaining_bytes,
            0);

        if (sent_bytes == 0) {
            return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_CLIENT_CLOSED);
        }

        if (sent_bytes < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_SERVER_WRITE_TIMEOUT);
            }

            if (errno == EINTR) {
                return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_SERVER_CLOSED);
            }

            return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_SERVER_WRITE_FAILED);
        }
    }

    return let_error_none();
}

void let_network_close(let_network_server_t *network_server) {
    if (network_server->handle >= 0) {
        close(network_server->handle);
        network_server->handle = -1;
    }
}

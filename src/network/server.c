#include "let/network/server.h"

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

let_network_server_t let_network_server_empty(void) {
    return (let_network_server_t){};
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

    memset(&network_server->address, 0, sizeof(network_server->address));

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
                                      let_network_server_t *network_client) {
    socklen_t client_address_length = sizeof(network_client->address);
    const auto client_handle = accept(network_server->handle,
                                      (struct sockaddr *) &network_client->address,
                                      &client_address_length);

    if (client_handle < 0) {
        return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_SERVER_ACCEPT_FAILED);
    }

    network_client->handle = client_handle;
    return let_error_none();
}

let_error_t let_network_client_read(const let_network_server_t *network_client,
                                    let_network_request_t *request) {
    auto network_parser = let_network_request_parser_empty();
    let_u8_t current_byte = 0;

    while (true) {
        const auto read_result = recv(network_client->handle, &current_byte, 1, 0);
        if (read_result == 0) {
            return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_SERVER_CLOSED);
        }

        if (read_result < 0) {
            return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_SERVER_READ_FAILED);
        }

        const auto parser_result = let_network_request_parser_next(&network_parser, current_byte, request);
        if (let_error_exists(parser_result)) {
            return parser_result;
        }

        if (network_parser.state == LET_NETWORK_REQUEST_PARSER_STATE_DONE) {
            return let_error_none();
        }
    }
}

let_error_t let_network_client_write(const let_network_server_t *network_client,
                                     const let_network_response_t *response) {
    u_int8_t response_buffer[LET_NETWORK_RESPONSE_SIZE_MAX] = {};
    const auto response_length = let_network_response_to_bytes(response, response_buffer);


    for (size_t written_bytes = 0; written_bytes < response_length;) {
        const auto send_result = send(
            network_client->handle,
            response_buffer + written_bytes,
            response_length - written_bytes,
            MSG_NOSIGNAL);

        if (send_result < 0) {
            if (errno == EINTR) {
                continue;
            }

            return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_SERVER_WRITE_FAILED);
        }

        written_bytes += (size_t) send_result;
    }

    return let_error_none();
}

void let_network_close(let_network_server_t *network_server) {
    if (network_server->handle >= 0) {
        close(network_server->handle);
        network_server->handle = -1;
    }
}

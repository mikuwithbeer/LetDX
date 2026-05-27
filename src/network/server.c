#include "let/network/server.h"

#include <string.h>
#include <unistd.h>
#include <stdlib.h>

let_network_server_error_t let_network_server_init(let_network_server_t *network_server) {
    network_server->handle = socket(AF_INET, SOCK_STREAM, 0);
    if (network_server->handle < 0) {
        return LET_NETWORK_ERROR_SOCKET_CREATE_FAILED;
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
        let_network_free(network_server);
        return LET_NETWORK_ERROR_SOCKET_BIND_FAILED;
    }

    const auto listen_result = listen(network_server->handle, LET_NETWORK_SERVER_BACKLOG_DEFAULT);

    if (listen_result < 0) {
        let_network_free(network_server);
        return LET_NETWORK_ERROR_SOCKET_LISTEN_FAILED;
    }

    return LET_NETWORK_ERROR_NONE;
}

let_network_server_error_t let_network_server_accept(const let_network_server_t *network_server,
                                                     let_network_server_t *network_client) {
    socklen_t client_address_length = sizeof(network_client->address);
    const auto client_handle = accept(network_server->handle,
                                      (struct sockaddr *) &network_client->address,
                                      &client_address_length);

    if (client_handle < 0) {
        return LET_NETWORK_ERROR_SOCKET_ACCEPT_FAILED;
    }

    network_client->handle = client_handle;
    return LET_NETWORK_ERROR_NONE;
}

let_network_server_error_t let_network_client_read(const let_network_server_t *network_client,
                                                   let_network_request_t *request) {
    auto network_parser = let_network_request_parser_new();
    let_u8_t current_byte = 0;

    while (true) {
        const auto read_result = recv(network_client->handle, &current_byte, 1, 0);
        if (read_result == 0) {
            return LET_NETWORK_ERROR_SOCKET_CLOSED;
        }

        if (read_result < 0) {
            return LET_NETWORK_ERROR_SOCKET_READ_FAILED;
        }

        const auto parser_result = let_network_request_parser_next(&network_parser, current_byte, request);
        if (parser_result != LET_NETWORK_REQUEST_PARSER_ERROR_NONE) {
            return LET_NETWORK_ERROR_SOCKET_PARSE_FAILED;
        }

        if (network_parser.state == LET_NETWORK_REQUEST_PARSER_STATE_DONE) {
            return LET_NETWORK_ERROR_NONE;
        }
    }
}

let_network_server_error_t let_network_client_write(const let_network_server_t *network_client,
                                                    const let_network_response_t *response) {
    u_int8_t response_buffer[64] = {0};
    const auto response_length = let_network_response_to_bytes(response, response_buffer);

    const auto send_result = send(network_client->handle, response_buffer, response_length, 0);
    if (send_result < 0) {
        return LET_NETWORK_ERROR_SOCKET_WRITE_FAILED;
    }

    return LET_NETWORK_ERROR_NONE;
}

void let_network_free(let_network_server_t *network_server) {
    if (network_server->handle >= 0) {
        close(network_server->handle);
        network_server->handle = -1;
    }
}

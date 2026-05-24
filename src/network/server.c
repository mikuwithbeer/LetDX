#include "network/parser.h"
#include "network/server.h"

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

    if (bind(network_server->handle, (struct sockaddr *) &network_server->address,
             sizeof(network_server->address)) < 0) {
        return LET_NETWORK_ERROR_SOCKET_BIND_FAILED;
    }

    if (listen(network_server->handle, LET_NETWORK_SERVER_BACKLOG_DEFAULT) < 0)
        return LET_NETWORK_ERROR_SOCKET_LISTEN_FAILED;

    return LET_NETWORK_ERROR_NONE;
}

let_network_server_error_t let_network_server_accept(const let_network_server_t *network_server,
                                                     let_network_server_t *network_client) {
    constexpr socklen_t client_address_length = sizeof(network_client->address);
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
                                                   let_network_protocol_request_t *request) {
    auto network_parser = let_network_parser_new();
    let_u8_t current_byte = 0;

    while (true) {
        const auto read_result = recv(network_client->handle, &current_byte, 1, 0);
        if (read_result == 0) {
            return LET_NETWORK_ERROR_SOCKET_CLOSED;
        }

        if (read_result < 0) {
            return LET_NETWORK_ERROR_SOCKET_READ_FAILED;
        }

        const auto parser_result = let_network_parser_next(&network_parser, current_byte);
        if (parser_result != LET_NETWORK_PARSER_ERROR_NONE) {
            return LET_NETWORK_ERROR_SOCKET_PARSE_FAILED;
        }

        if (network_parser.state == LET_NETWORK_PARSER_STATE_DONE) {
            memcpy(request, &network_parser.request, sizeof(let_network_protocol_request_t));
            return LET_NETWORK_ERROR_NONE;
        }
    }
}

let_network_server_error_t let_network_client_write(const let_network_server_t *network_client,
                                                    const let_network_protocol_response_t response) {
    let_size_t network_buffer_length = 0;
    u_int8_t network_buffer[64] = {0};

    switch (response.id) {
        case LET_NETWORK_PROTOCOL_RESPONSE_ID_ERROR:
            network_buffer_length = 4;
            network_buffer[0] = 'E';
            network_buffer[1] = 'R';
            network_buffer[2] = 'R';
            network_buffer[3] = '\n';
            break;
        case LET_NETWORK_PROTOCOL_RESPONSE_ID_OK:
            network_buffer_length = 3;
            network_buffer[0] = 'O';
            network_buffer[1] = 'K';
            network_buffer[2] = '\n';
            break;
        default:
            break;
    }

    const auto send_result = send(network_client->handle, network_buffer, network_buffer_length, 0);
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

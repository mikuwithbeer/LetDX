#include "network/server.h"

#include <string.h>
#include <unistd.h>

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
                                                   const let_pointer_t network_buffer,
                                                   const let_size_t network_buffer_length) {
    const auto receive_result = recv(network_client->handle, network_buffer, network_buffer_length, 0);
    if (receive_result < 0) {
        return LET_NETWORK_ERROR_SOCKET_READ_FAILED;
    }

    return LET_NETWORK_ERROR_NONE;
}

let_network_server_error_t let_network_client_write(const let_network_server_t *network_client,
                                                    const let_pointer_t network_buffer,
                                                    const let_size_t network_buffer_length) {
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

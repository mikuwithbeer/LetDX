#ifndef LET_NETWORK_SERVER_H
#define LET_NETWORK_SERVER_H

#include "network/request.h"
#include "network/response.h"

#include <netinet/in.h>

constexpr int LET_NETWORK_SERVER_BACKLOG_DEFAULT = 1 << 2;

typedef enum [[nodiscard]] : let_u8_t {
    LET_NETWORK_ERROR_NONE,
    LET_NETWORK_ERROR_SOCKET_CREATE_FAILED,
    LET_NETWORK_ERROR_SOCKET_BIND_FAILED,
    LET_NETWORK_ERROR_SOCKET_LISTEN_FAILED,
    LET_NETWORK_ERROR_SOCKET_ACCEPT_FAILED,
    LET_NETWORK_ERROR_SOCKET_READ_FAILED,
    LET_NETWORK_ERROR_SOCKET_WRITE_FAILED,
    LET_NETWORK_ERROR_SOCKET_CLOSED,
    LET_NETWORK_ERROR_SOCKET_PARSE_FAILED,
} let_network_server_error_t;

typedef struct sockaddr_in let_network_server_address_t;

typedef struct {
    let_network_server_address_t address;
    let_i32_t handle;
    let_u16_t port;
} let_network_server_t;

let_network_server_error_t let_network_server_init(let_network_server_t *network_server);

let_network_server_error_t let_network_server_accept(const let_network_server_t *network_server,
                                                     let_network_server_t *network_client);

let_network_server_error_t let_network_client_read(const let_network_server_t *network_client,
                                                   let_network_request_t *request);

let_network_server_error_t let_network_client_write(const let_network_server_t *network_client,
                                                    const let_network_response_t *response);

void let_network_free(let_network_server_t *network_server);

#endif //LET_NETWORK_SERVER_H

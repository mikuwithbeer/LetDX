#ifndef LET_NETWORK_H
#define LET_NETWORK_H

#include "common.h"

#include <netinet/in.h>

constexpr int LET_NETWORK_BACKLOG_DEFAULT = 1 << 2;

typedef enum : let_u8_t {
    LET_NETWORK_ERROR_NONE,
    LET_NETWORK_ERROR_SOCKET_CREATE_FAILED,
    LET_NETWORK_ERROR_SOCKET_BIND_FAILED,
    LET_NETWORK_ERROR_SOCKET_LISTEN_FAILED,
    LET_NETWORK_ERROR_SOCKET_ACCEPT_FAILED,
    LET_NETWORK_ERROR_SOCKET_READ_FAILED,
    LET_NETWORK_ERROR_SOCKET_WRITE_FAILED,
} let_network_error_t;

typedef struct sockaddr_in let_network_socket_address_t;

typedef struct {
    let_network_socket_address_t address;
    let_i32_t handle;
    let_u16_t port;
} let_network_t;

typedef let_network_t let_network_server_t;
typedef let_network_t let_network_client_t;

[[nodiscard]] let_network_error_t let_network_server_init(let_network_server_t *network_server);

[[nodiscard]] let_network_error_t let_network_server_accept(const let_network_server_t *network_server,
                                                            let_network_client_t *network_client);

[[nodiscard]] let_network_error_t let_network_client_read(const let_network_client_t *network_client,
                                                          let_pointer_t network_buffer,
                                                          let_size_t network_buffer_length);

[[nodiscard]] let_network_error_t let_network_client_write(const let_network_client_t *network_client,
                                                           let_pointer_t network_buffer,
                                                           let_size_t network_buffer_length);

void let_network_free(let_network_t *network);

#endif //LET_NETWORK_H

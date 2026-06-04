#ifndef LET_NETWORK_SERVER_H
#define LET_NETWORK_SERVER_H

#include "let/network/request.h"
#include "let/network/response.h"

#include <netinet/in.h>

constexpr int LET_NETWORK_SERVER_BACKLOG_DEFAULT = 1 << 2;

typedef struct sockaddr_in let_network_server_address_t;

typedef struct {
    let_network_server_address_t address;
    let_i32_t handle;
    let_u16_t port;
} let_network_server_t;

[[nodiscard]] let_network_server_t let_network_server_empty(void);

let_error_t let_network_server_init(let_network_server_t *network_server,
                                    let_u16_t port);

let_error_t let_network_server_accept(const let_network_server_t *network_server,
                                      let_network_server_t *network_client);

let_error_t let_network_client_read(const let_network_server_t *network_client,
                                    let_network_request_t *request);

let_error_t let_network_client_write(const let_network_server_t *network_client,
                                     const let_network_response_t *response);

void let_network_close(let_network_server_t *network_server);

#endif //LET_NETWORK_SERVER_H

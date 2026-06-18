#ifndef LET_NETWORK_SERVER_H
#define LET_NETWORK_SERVER_H

#include "let/network/request/decoder.h"
#include "let/network/response/encoder.h"

#include <netinet/in.h>

constexpr let_u8_t LET_NETWORK_BUFFER_LENGTH = 128;

typedef struct {
    struct sockaddr_in address;
    let_i32_t handle;
    let_u16_t port;

    let_u8_t read_buffer[LET_NETWORK_BUFFER_LENGTH];
    let_u8_t write_buffer[LET_NETWORK_BUFFER_LENGTH];

    let_size_t read_buffer_index;
    let_size_t write_buffer_index;
} let_network_server_t;

[[nodiscard]] let_network_server_t let_network_server_empty(void);

let_error_t let_network_server_init(let_network_server_t *network_server,
                                    let_u16_t port,
                                    let_u16_t backlog);

let_error_t let_network_server_accept(const let_network_server_t *network_server,
                                      let_u32_t read_timeout,
                                      let_u32_t write_timeout,
                                      let_network_server_t *network_client);

let_error_t let_network_client_read(let_network_server_t *network_client,
                                    let_network_request_t *request);

let_error_t let_network_client_write(let_network_server_t *network_client,
                                     let_network_response_t response);

void let_network_close(let_network_server_t *network_server);

#endif //LET_NETWORK_SERVER_H

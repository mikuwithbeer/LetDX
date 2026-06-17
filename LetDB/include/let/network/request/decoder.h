#ifndef LET_NETWORK_REQUEST_DECODER_H
#define LET_NETWORK_REQUEST_DECODER_H

#include "let/network/request/request.h"

typedef enum : let_u8_t {
    LET_NETWORK_REQUEST_DECODER_STATE_COMMAND,
    LET_NETWORK_REQUEST_DECODER_STATE_ARGUMENT,
    LET_NETWORK_REQUEST_DECODER_STATE_END,
} let_network_request_decoder_state_t;

typedef struct {
    let_network_request_t request;
    let_u8_t request_argc;

    let_u8_t *buffer;
    let_size_t buffer_length;

    let_network_request_decoder_state_t state;
} let_network_request_decoder_t;

[[nodiscard]] let_network_request_decoder_t let_network_request_decoder_empty(void);

void let_network_request_decoder_init(let_network_request_decoder_t *request_decoder,
                                      let_u8_t *buffer,
                                      let_size_t buffer_length);

let_error_t let_network_request_decoder_run(let_network_request_decoder_t *request_decoder);

#endif //LET_NETWORK_REQUEST_DECODER_H

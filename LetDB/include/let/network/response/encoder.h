#ifndef LET_NETWORK_RESPONSE_ENCODER_H
#define LET_NETWORK_RESPONSE_ENCODER_H

#include "let/network/response/response.h"
#include "let/common.h"

constexpr let_u8_t LET_NETWORK_RESPONSE_ENCODER_HEX_DIGITS[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F'
};

typedef struct {
    let_network_response_t response;

    let_u8_t *buffer;
    let_size_t buffer_length;
    let_size_t buffer_capacity;
} let_network_response_encoder_t;

[[nodiscard]] let_network_response_encoder_t let_network_response_encoder_empty(void);

void let_network_response_encoder_init(let_network_response_encoder_t *response_encoder,
                                       let_network_response_t network_response,
                                       let_u8_t *buffer,
                                       let_size_t buffer_capacity);

let_error_t let_network_response_encoder_run(let_network_response_encoder_t *response_encoder);

#endif //LET_NETWORK_RESPONSE_ENCODER_H

#include "let/network/response/encoder.h"

let_network_response_t let_network_response_empty(void) {
    return (let_network_response_t){};
}

let_error_t let_network_response_encode(const let_network_response_t network_response,
                                        let_u8_t *buffer,
                                        const let_size_t buffer_capacity,
                                        let_size_t *output) {
    auto response_encoder = let_network_response_encoder_empty();
    let_network_response_encoder_init(&response_encoder, network_response, buffer, buffer_capacity);

    const auto encode_result = let_network_response_encoder_run(&response_encoder);
    if (!let_error_exists(encode_result)) {
        *output = response_encoder.buffer_length;
    }

    return encode_result;
}

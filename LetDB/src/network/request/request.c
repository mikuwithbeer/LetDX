#include "let/network/request/decoder.h"

let_network_request_t let_network_request_empty(void) {
    return (let_network_request_t){};
}

let_error_t let_network_request_decode(let_u8_t *buffer,
                                       const let_size_t buffer_length,
                                       let_network_request_t *output) {
    auto request_decoder = let_network_request_decoder_empty();
    let_network_request_decoder_init(&request_decoder, buffer, buffer_length);

    const auto decode_result = let_network_request_decoder_run(&request_decoder);
    if (!let_error_exists(decode_result)) {
        *output = request_decoder.request;
    }

    return decode_result;
}

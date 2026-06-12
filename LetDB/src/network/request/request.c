#include "let/network/request/decoder.h"
#include "let/network/request/request.h"

let_network_request_t let_network_request_empty(void) {
    return (let_network_request_t){};
}

let_error_t let_network_request_decode(let_network_request_t *request,
                                       let_u8_t *buffer,
                                       const let_size_t buffer_length) {
    auto request_decoder = let_network_request_decoder_empty();
    let_network_request_decoder_init(&request_decoder, buffer, buffer_length);

    const auto decode_result = let_network_request_decoder_run(&request_decoder);
    if (!let_error_exists(decode_result)) {
        *request = request_decoder.request;
    }

    return decode_result;
}

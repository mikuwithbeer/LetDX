#include "let/network/response.h"

#include <stdio.h>

let_network_response_t let_network_response_new(void) {
    return (let_network_response_t){0};
}

let_size_t let_network_response_to_bytes(const let_network_response_t *response,
                                         let_u8_t *output) {
    let_size_t written_bytes = 0;

    switch (response->id) {
        case LET_NETWORK_RESPONSE_ID_MAGIC: {
            output[written_bytes++] = LET_NETWORK_RESPONSE_MAGIC[0];
            output[written_bytes++] = LET_NETWORK_RESPONSE_MAGIC[1];
            output[written_bytes++] = LET_NETWORK_RESPONSE_MAGIC[2];
            output[written_bytes++] = LET_NETWORK_RESPONSE_MAGIC[3];
            break;
        }
        case LET_NETWORK_RESPONSE_ID_ADD_ACCOUNT:
            written_bytes = sprintf(output, "%llu", response->data.add_account);
            break;
        case LET_NETWORK_RESPONSE_ID_OK:
            output[written_bytes++] = 'O';
            output[written_bytes++] = 'K';
            break;
        case LET_NETWORK_RESPONSE_ID_ERROR: {
            output[written_bytes++] = 'E';
            output[written_bytes++] = 'R';
            output[written_bytes++] = 'R';
        }
    }

    output[written_bytes++] = '\n';
    return written_bytes;
}

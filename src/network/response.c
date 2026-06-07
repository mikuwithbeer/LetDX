#include "let/network/response.h"

#include <stdio.h>

let_network_response_t let_network_response_empty(void) {
    return (let_network_response_t){};
}

let_size_t let_network_response_to_bytes(const let_network_response_t *response,
                                         let_u8_t *output) {
    let_size_t written_bytes = 0;
    switch (response->id) {
        case LET_NETWORK_RESPONSE_ID_MAGIC: {
            output[written_bytes++] = LET_NETWORK_RESPONSE_MAGIC[0];
            output[written_bytes++] = LET_NETWORK_RESPONSE_MAGIC[1];
            output[written_bytes++] = LET_NETWORK_RESPONSE_MAGIC[2];
            break;
        }
        case LET_NETWORK_RESPONSE_ID_ADD_ACCOUNT: {
            written_bytes = (let_size_t) sprintf((char *)output, "AID %llX", response->data.add_account);
            break;
        }
        case LET_NETWORK_RESPONSE_ID_GET_BALANCE: {
            const auto balance_high = (let_u64_t) (response->data.get_balance >> 64);
            const auto balance_low = (let_u64_t) response->data.get_balance;

            if (balance_high == 0) {
                written_bytes = (let_size_t) sprintf((char *)output, "BAL %llX", balance_low);
            } else {
                written_bytes = (let_size_t) sprintf((char *)output, "BAL %llX%016llX", balance_high, balance_low);
            }

            break;
        }
        case LET_NETWORK_RESPONSE_ID_COUNT_ENTRIES: {
            written_bytes = (let_size_t) sprintf((char *)output, "SEC %llX", response->data.count_entries);
            break;
        }
        case LET_NETWORK_RESPONSE_ID_OK: {
            output[written_bytes++] = 'O';
            output[written_bytes++] = 'K';
            output[written_bytes++] = 'E';
            break;
        }
        case LET_NETWORK_RESPONSE_ID_ERROR: {
            const auto error_code = let_error_code(response->data.error);
            written_bytes = (let_size_t) sprintf((char *)output, "ERR %hX", error_code);

            break;
        }
    }

    output[written_bytes++] = '\n';
    return written_bytes;
}

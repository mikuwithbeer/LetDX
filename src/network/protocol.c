#include "network/protocol.h"

#include <stddef.h>

uint8_t let_network_protocol_request_id_to_argument_count(const let_network_protocol_request_id_t request_id) {
    switch (request_id) {
        case LET_NETWORK_PROTOCOL_REQUEST_ID_VERSION:
            return 0;
        case LET_NETWORK_PROTOCOL_REQUEST_ID_CREATE_ACCOUNT:
            return 3;
        case LET_NETWORK_PROTOCOL_REQUEST_ID_CLOSE:
            return 0;
        default:
            unreachable();
    }
}

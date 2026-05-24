#ifndef LET_NETWORK_PROTOCOL_H
#define LET_NETWORK_PROTOCOL_H

#include "common.h"

typedef enum : let_u8_t {
    LET_NETWORK_PROTOCOL_REQUEST_ID_VERSION,
    LET_NETWORK_PROTOCOL_REQUEST_ID_CREATE_ACCOUNT,
    LET_NETWORK_PROTOCOL_REQUEST_ID_CLOSE,
} let_network_protocol_request_id_t;

typedef enum : let_u8_t {
    LET_NETWORK_PROTOCOL_RESPONSE_ID_OK,
    LET_NETWORK_PROTOCOL_RESPONSE_ID_ERROR,
} let_network_protocol_response_id_t;

typedef union {
    struct {
        let_u128_t credits;
        let_u128_t debits;
        let_u8_t flags;
    } create_account;
} let_network_protocol_request_data_t;

typedef struct {
    let_network_protocol_request_id_t id;
    let_network_protocol_request_data_t data;
} let_network_protocol_request_t;

typedef union {
    let_u8_t error;
} let_network_protocol_response_data_t;

typedef struct {
    let_network_protocol_response_id_t id;
    let_network_protocol_response_data_t data;
} let_network_protocol_response_t;

[[nodiscard]] uint8_t let_network_protocol_request_id_to_argument_count(let_network_protocol_request_id_t request_id);

#endif //LET_NETWORK_PROTOCOL_H

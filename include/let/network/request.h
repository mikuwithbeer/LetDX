#ifndef LET_NETWORK_REQUEST_H
#define LET_NETWORK_REQUEST_H

#include "let/common.h"

typedef enum : let_u8_t {
    LET_NETWORK_REQUEST_ID_VERSION,
    LET_NETWORK_REQUEST_ID_ADD_ACCOUNT,
    LET_NETWORK_REQUEST_ID_MAKE_TRANSFER,
    LET_NETWORK_REQUEST_ID_CLOSE,
} let_network_request_id_t;

typedef enum : let_u8_t {
    LET_NETWORK_REQUEST_PARSER_STATE_COMMAND,
    LET_NETWORK_REQUEST_PARSER_STATE_ARGUMENT,
    LET_NETWORK_REQUEST_PARSER_STATE_DONE,
} let_network_request_parser_state_t;

typedef enum [[nodiscard]] : let_u8_t {
    LET_NETWORK_REQUEST_PARSER_ERROR_NONE,
    LET_NETWORK_REQUEST_PARSER_ERROR_UNKNOWN_COMMAND,
    LET_NETWORK_REQUEST_PARSER_ERROR_INVALID_INTEGER,
    LET_NETWORK_REQUEST_PARSER_ERROR_INTEGER_OVERFLOW,
    LET_NETWORK_REQUEST_PARSER_ERROR_EXPECTED_NEW_LINE,
} let_network_request_parser_error_t;

typedef union {
    struct {
        let_u128_t credits;
        let_u128_t debits;
        let_u8_t flags;
    } create_account;
    struct {
        let_u64_t from_id;
        let_u64_t to_id;
        let_u128_t amount;
    } make_transfer;
} let_network_request_data_t;

typedef struct {
    let_network_request_id_t id;
    let_network_request_data_t data;
} let_network_request_t;

typedef struct {
    let_u8_t request_argument_counter;
    let_network_request_parser_state_t state;
} let_network_request_parser_t;

[[nodiscard]] let_network_request_t let_network_request_new(void);

[[nodiscard]] uint8_t let_network_request_to_argument_count(const let_network_request_t *request);

[[nodiscard]] let_network_request_parser_t let_network_request_parser_new(void);

let_network_request_parser_error_t let_network_request_parser_next(let_network_request_parser_t *request_parser,
                                                                   let_u8_t byte,
                                                                   let_network_request_t *output);

let_network_request_parser_error_t let_network_request_parser_collect_u128(let_network_request_parser_t *request_parser,
                                                                           let_u8_t byte,
                                                                           let_u128_t *output);

let_network_request_parser_error_t let_network_request_parser_collect_u64(let_network_request_parser_t *request_parser,
                                                                           let_u8_t byte,
                                                                           let_u64_t *output);

let_network_request_parser_error_t let_network_request_parser_collect_u8(let_network_request_parser_t *request_parser,
                                                                         let_u8_t byte,
                                                                         let_u8_t *output);
#endif //LET_NETWORK_REQUEST_H

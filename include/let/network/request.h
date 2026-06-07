#ifndef LET_NETWORK_REQUEST_H
#define LET_NETWORK_REQUEST_H

#include "let/common.h"
#include "let/error.h"

typedef enum : let_u8_t {
    LET_NETWORK_REQUEST_TYPE_MAGIC,
    LET_NETWORK_REQUEST_TYPE_ADD_ACCOUNT,
    LET_NETWORK_REQUEST_TYPE_MAKE_TRANSFER,
    LET_NETWORK_REQUEST_TYPE_GET_BALANCE,
    LET_NETWORK_REQUEST_TYPE_COUNT_ENTRIES,
    LET_NETWORK_REQUEST_TYPE_CLOSE,
} let_network_request_type_t;

typedef enum : let_u8_t {
    LET_NETWORK_REQUEST_PARSER_STATE_COMMAND,
    LET_NETWORK_REQUEST_PARSER_STATE_ARGUMENT,
    LET_NETWORK_REQUEST_PARSER_STATE_DONE,
} let_network_request_parser_state_t;

typedef union {
    struct {
        let_u64_t wal_id;
        let_u128_t balance;
        let_u8_t flags;
    } create_account;

    struct {
        let_u64_t wal_id;
        let_u64_t from_id;
        let_u64_t to_id;
        let_u128_t amount;
    } make_transfer;

    let_u64_t get_balance;
} let_network_request_data_t;

typedef struct {
    let_network_request_type_t type;
    let_network_request_data_t data;
} let_network_request_t;

typedef struct {
    let_u8_t request_argument_counter;
    let_network_request_parser_state_t state;
} let_network_request_parser_t;

[[nodiscard]] let_network_request_t let_network_request_empty(void);

[[nodiscard]] uint8_t let_network_request_to_argument_count(const let_network_request_t *request);

[[nodiscard]] let_network_request_parser_t let_network_request_parser_empty(void);

let_error_t let_network_request_parser_next(let_network_request_parser_t *request_parser,
                                            let_u8_t byte,
                                            let_network_request_t *output);

let_error_t let_network_request_parser_collect_u128(let_network_request_parser_t *request_parser,
                                                    let_u8_t byte,
                                                    let_u128_t *output);

let_error_t let_network_request_parser_collect_u64(let_network_request_parser_t *request_parser,
                                                   let_u8_t byte,
                                                   let_u64_t *output);

let_error_t let_network_request_parser_collect_u8(let_network_request_parser_t *request_parser,
                                                  let_u8_t byte,
                                                  let_u8_t *output);
#endif //LET_NETWORK_REQUEST_H

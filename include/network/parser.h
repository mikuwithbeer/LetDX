#ifndef LET_NETWORK_PARSER_H
#define LET_NETWORK_PARSER_H

#include "protocol.h"

typedef enum : let_u8_t {
    LET_NETWORK_PARSER_ERROR_NONE,
    LET_NETWORK_PARSER_ERROR_INVALID_COMMAND,
    LET_NETWORK_PARSER_ERROR_INVALID_INTEGER,
    LET_NETWORK_PARSER_ERROR_INTEGER_OVERFLOW,
    LET_NETWORK_PARSER_ERROR_EXPECTED_NEW_LINE,
} let_network_parser_error_t;

typedef enum : let_u8_t {
    LET_NETWORK_PARSER_STATE_COMMAND,
    LET_NETWORK_PARSER_STATE_ARGUMENT,
    LET_NETWORK_PARSER_STATE_DONE,
} let_network_parser_state_t;

typedef struct {
    let_network_parser_state_t state;

    let_network_protocol_request_t request;
    let_u8_t request_argument_counter;
} let_network_parser_t;

[[nodiscard]] let_network_parser_t let_network_parser_new(void);

[[nodiscard]] let_network_parser_error_t let_network_parser_next(let_network_parser_t *parser,
                                                                 let_u8_t byte);

[[nodiscard]] let_network_parser_error_t let_network_parser_collect_asset(let_network_parser_t *parser,
                                                                          let_u8_t byte,
                                                                          let_u128_t *asset);

[[nodiscard]] let_network_parser_error_t let_network_parser_collect_flags(let_network_parser_t *parser,
                                                                          let_u8_t byte,
                                                                          let_u8_t *flags);

#endif //LET_NETWORK_PARSER_H

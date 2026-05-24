#include "network/parser.h"

#include <stddef.h>

let_network_parser_t let_network_parser_new(void) {
    return (let_network_parser_t){0};
}

let_network_parser_error_t let_network_parser_next(let_network_parser_t *parser,
                                                   const let_u8_t byte) {
    switch (parser->state) {
        case LET_NETWORK_PARSER_STATE_COMMAND: {
            switch (byte) {
                case '?':
                    parser->request.id = LET_NETWORK_PROTOCOL_REQUEST_ID_VERSION;
                    break;
                case '+':
                    parser->request.id = LET_NETWORK_PROTOCOL_REQUEST_ID_CREATE_ACCOUNT;
                    break;
                case '.':
                    parser->request.id = LET_NETWORK_PROTOCOL_REQUEST_ID_CLOSE;
                    break;
                default:
                    return LET_NETWORK_PARSER_ERROR_INVALID_COMMAND;
            }

            parser->request_argument_counter = let_network_protocol_request_id_to_argument_count(parser->request.id);
            parser->state = LET_NETWORK_PARSER_STATE_ARGUMENT;
            break;
        }
        case LET_NETWORK_PARSER_STATE_ARGUMENT: {
            if (parser->request_argument_counter == 0) {
                if (byte == '\n') {
                    parser->state = LET_NETWORK_PARSER_STATE_DONE;
                    break;
                }

                return LET_NETWORK_PARSER_ERROR_EXPECTED_NEW_LINE;
            }

            switch (parser->request.id) {
                case LET_NETWORK_PROTOCOL_REQUEST_ID_CREATE_ACCOUNT: {
                    switch (parser->request_argument_counter) {
                        case 3: {
                            const auto collect_result = let_network_parser_collect_asset(
                                parser,
                                byte,
                                &parser->request.data.create_account.credits);

                            if (collect_result != LET_NETWORK_PARSER_ERROR_NONE) {
                                return collect_result;
                            }

                            break;
                        }
                        case 2: {
                            const auto collect_result = let_network_parser_collect_asset(
                                parser,
                                byte,
                                &parser->request.data.create_account.debits);

                            if (collect_result != LET_NETWORK_PARSER_ERROR_NONE) {
                                return collect_result;
                            }

                            break;
                        }
                        case 1: {
                            const auto collect_result = let_network_parser_collect_flags(
                                parser,
                                byte,
                                &parser->request.data.create_account.flags);

                            if (collect_result != LET_NETWORK_PARSER_ERROR_NONE) {
                                return collect_result;
                            }

                            break;
                        }
                        default:
                            unreachable();
                    }

                    break;
                }
                default:
                    unreachable();
            }
        }
        case LET_NETWORK_PARSER_STATE_DONE:
            break;
    }

    return LET_NETWORK_PARSER_ERROR_NONE;
}

let_network_parser_error_t let_network_parser_collect_asset(let_network_parser_t *parser,
                                                            const let_u8_t byte,
                                                            let_u128_t *asset) {
    if (byte >= '0' && byte <= '9') {
        const let_u8_t digit = byte - '0';

        if (*asset > LET_U128_MAX / 10) {
            return LET_NETWORK_PARSER_ERROR_INTEGER_OVERFLOW;
        }

        *asset *= 10;

        if (digit > LET_U128_MAX - *asset) {
            return LET_NETWORK_PARSER_ERROR_INTEGER_OVERFLOW;
        }

        *asset += digit;
    } else if (byte == ' ') {
        parser->request_argument_counter--;
    } else {
        return LET_NETWORK_PARSER_ERROR_INVALID_INTEGER;
    }

    return LET_NETWORK_PARSER_ERROR_NONE;
}

let_network_parser_error_t let_network_parser_collect_flags(let_network_parser_t *parser,
                                                            const let_u8_t byte,
                                                            let_u8_t *flags) {
    if (byte >= '0' && byte <= '9') {
        const let_u8_t digit = byte - '0';

        if (*flags > LET_U8_MAX / 10) {
            return LET_NETWORK_PARSER_ERROR_INTEGER_OVERFLOW;
        }

        *flags *= 10;

        if (digit > LET_U8_MAX - *flags) {
            return LET_NETWORK_PARSER_ERROR_INTEGER_OVERFLOW;
        }

        *flags += digit;
    } else if (byte == ' ') {
        parser->request_argument_counter--;
    } else {
        return LET_NETWORK_PARSER_ERROR_INVALID_INTEGER;
    }

    return LET_NETWORK_PARSER_ERROR_NONE;
}

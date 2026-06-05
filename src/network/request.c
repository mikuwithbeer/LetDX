#include "let/network/request.h"

#include <stddef.h>

let_network_request_t let_network_request_empty(void) {
    return (let_network_request_t){0};
}

uint8_t let_network_request_to_argument_count(const let_network_request_t *request) {
    switch (request->type) {
        case LET_NETWORK_REQUEST_TYPE_MAGIC:
        case LET_NETWORK_REQUEST_TYPE_CLOSE:
            return 0;
        case LET_NETWORK_REQUEST_TYPE_GET_BALANCE:
            return 1;
        case LET_NETWORK_REQUEST_TYPE_ADD_ACCOUNT:
            return 3;
        case LET_NETWORK_REQUEST_TYPE_MAKE_TRANSFER:
            return 4;
    }

    unreachable();
}

let_network_request_parser_t let_network_request_parser_empty(void) {
    return (let_network_request_parser_t){0};
}

let_error_t let_network_request_parser_next(let_network_request_parser_t *request_parser,
                                            const let_u8_t byte,
                                            let_network_request_t *output) {
    switch (request_parser->state) {
        case LET_NETWORK_REQUEST_PARSER_STATE_COMMAND: {
            switch (byte) {
                case '!':
                    output->type = LET_NETWORK_REQUEST_TYPE_MAGIC;
                    break;
                case '+':
                    output->type = LET_NETWORK_REQUEST_TYPE_ADD_ACCOUNT;
                    break;
                case '%':
                    output->type = LET_NETWORK_REQUEST_TYPE_MAKE_TRANSFER;
                    break;
                case '?':
                    output->type = LET_NETWORK_REQUEST_TYPE_GET_BALANCE;
                    break;
                case '.':
                    output->type = LET_NETWORK_REQUEST_TYPE_CLOSE;
                    break;
                default:
                    return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_REQUEST_UNKNOWN_COMMAND);
            }

            request_parser->request_argument_counter = let_network_request_to_argument_count(output);
            request_parser->state = LET_NETWORK_REQUEST_PARSER_STATE_ARGUMENT;
            break;
        }
        case LET_NETWORK_REQUEST_PARSER_STATE_ARGUMENT: {
            if (request_parser->request_argument_counter == 0) {
                if (byte == '\n') {
                    request_parser->state = LET_NETWORK_REQUEST_PARSER_STATE_DONE;
                    break;
                }

                return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_REQUEST_EXPECTED_NEW_LINE);
            }

            if (request_parser->request_argument_counter == 1 && byte == '\n') {
                request_parser->request_argument_counter--;
                request_parser->state = LET_NETWORK_REQUEST_PARSER_STATE_DONE;
                break;
            }

            switch (output->type) {
                case LET_NETWORK_REQUEST_TYPE_ADD_ACCOUNT: {
                    switch (request_parser->request_argument_counter) {
                        case 3: {
                            const auto collect_result = let_network_request_parser_collect_u64(
                                request_parser,
                                byte,
                                &output->data.create_account.wal_id);

                            if (collect_result.id != LET_ERROR_ID_NONE) {
                                return collect_result;
                            }

                            break;
                        }
                        case 2: {
                            const auto collect_result = let_network_request_parser_collect_u128(
                                request_parser,
                                byte,
                                &output->data.create_account.balance);

                            if (collect_result.id != LET_ERROR_ID_NONE) {
                                return collect_result;
                            }

                            break;
                        }
                        case 1: {
                            const auto collect_result = let_network_request_parser_collect_u8(
                                request_parser,
                                byte,
                                &output->data.create_account.flags);

                            if (collect_result.id != LET_ERROR_ID_NONE) {
                                return collect_result;
                            }

                            break;
                        }
                        default:
                            unreachable();
                    }

                    break;
                }
                case LET_NETWORK_REQUEST_TYPE_MAKE_TRANSFER: {
                    switch (request_parser->request_argument_counter) {
                        case 4: {
                            const auto collect_result = let_network_request_parser_collect_u64(
                                request_parser,
                                byte,
                                &output->data.make_transfer.wal_id);

                            if (collect_result.id != LET_ERROR_ID_NONE) {
                                return collect_result;
                            }

                            break;
                        }
                        case 3: {
                            const auto collect_result = let_network_request_parser_collect_u64(
                                request_parser,
                                byte,
                                &output->data.make_transfer.from_id);

                            if (collect_result.id != LET_ERROR_ID_NONE) {
                                return collect_result;
                            }

                            break;
                        }
                        case 2: {
                            const auto collect_result = let_network_request_parser_collect_u64(
                                request_parser,
                                byte,
                                &output->data.make_transfer.to_id);

                            if (collect_result.id != LET_ERROR_ID_NONE) {
                                return collect_result;
                            }

                            break;
                        }
                        case 1: {
                            const auto collect_result = let_network_request_parser_collect_u128(
                                request_parser,
                                byte,
                                &output->data.make_transfer.amount);

                            if (collect_result.id != LET_ERROR_ID_NONE) {
                                return collect_result;
                            }

                            break;
                        }
                        default:
                            unreachable();
                    }

                    break;
                }
                case LET_NETWORK_REQUEST_TYPE_GET_BALANCE: {
                    switch (request_parser->request_argument_counter) {
                        case 1: {
                            const auto collect_result = let_network_request_parser_collect_u64(
                                request_parser,
                                byte,
                                &output->data.get_balance);

                            if (collect_result.id != LET_ERROR_ID_NONE) {
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

            break;
        }
        case LET_NETWORK_REQUEST_PARSER_STATE_DONE:
            break;
    }

    return let_error_none();
}

let_error_t let_network_request_parser_collect_u128(let_network_request_parser_t *request_parser,
                                                    const let_u8_t byte,
                                                    let_u128_t *output) {
    if (byte >= '0' && byte <= '9') {
        const let_u8_t digit = byte - '0';

        if (*output > LET_U128_MAX / 10) {
            return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_REQUEST_INTEGER_OVERFLOW);
        }

        *output *= 10;

        if (digit > LET_U128_MAX - *output) {
            return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_REQUEST_INTEGER_OVERFLOW);
        }

        *output += digit;
    } else if (byte == ' ') {
        request_parser->request_argument_counter--;
    } else {
        return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_REQUEST_INVALID_INTEGER);
    }

    return let_error_none();
}


let_error_t let_network_request_parser_collect_u64(let_network_request_parser_t *request_parser,
                                                   const let_u8_t byte,
                                                   let_u64_t *output) {
    let_u128_t current_output = *output;

    const auto u128_result = let_network_request_parser_collect_u128(
        request_parser,
        byte,
        &current_output);

    if (u128_result.id != LET_ERROR_ID_NONE) {
        return u128_result;
    }

    if (current_output > LET_U64_MAX) {
        return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_REQUEST_INTEGER_OVERFLOW);
    }

    *output = (let_u64_t) current_output;
    return let_error_none();
}

let_error_t let_network_request_parser_collect_u8(let_network_request_parser_t *request_parser,
                                                  const let_u8_t byte,
                                                  let_u8_t *output) {
    let_u128_t current_output = *output;

    const auto u128_result = let_network_request_parser_collect_u128(
        request_parser,
        byte,
        &current_output);

    if (u128_result.id != LET_ERROR_ID_NONE) {
        return u128_result;
    }

    if (current_output > LET_U8_MAX) {
        return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_REQUEST_INTEGER_OVERFLOW);
    }

    *output = (let_u8_t) current_output;
    return let_error_none();
}

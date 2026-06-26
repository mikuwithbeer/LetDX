/**
 * @file decoder.c
 * @brief The network request decoder implementation.
 */

#include "let/network/request/decoder.h"

// -----------------------------------------------------------------------------
// Forward Declarations
// -----------------------------------------------------------------------------

static let_error_t let_network_request_decoder_run_command(let_network_request_decoder_t *request_decoder,
                                                           let_u8_t current_byte);

static let_error_t let_network_request_decoder_run_argument(let_network_request_decoder_t *request_decoder,
                                                            let_u8_t current_byte);

static let_error_t let_network_request_decoder_parse_u128(let_network_request_decoder_t *request_decoder,
                                                          let_u8_t current_byte,
                                                          let_u128_t *output);

static let_error_t let_network_request_decoder_parse_u64(let_network_request_decoder_t *request_decoder,
                                                         let_u8_t current_byte,
                                                         let_u64_t *output);

static let_error_t let_network_request_decoder_parse_u8(let_network_request_decoder_t *request_decoder,
                                                        let_u8_t current_byte,
                                                        let_u8_t *output);

// -----------------------------------------------------------------------------
// Function Implementations
// -----------------------------------------------------------------------------

let_network_request_decoder_t let_network_request_decoder_empty(void) {
    return (let_network_request_decoder_t){};
}

void let_network_request_decoder_init(let_network_request_decoder_t *request_decoder,
                                      let_u8_t *buffer,
                                      const let_size_t buffer_length) {
    request_decoder->buffer = buffer;
    request_decoder->buffer_length = buffer_length;
}

let_error_t let_network_request_decoder_run(let_network_request_decoder_t *request_decoder) {
    auto decoder_result = let_error_none();

    if (request_decoder->buffer == nullptr || request_decoder->buffer_length == 0) {
        return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_REQUEST_EMPTY);
    }

    let_size_t buffer_index = 0;

    // Run until the buffer is fully consumed or the decoder reaches the end state.
    while (buffer_index < request_decoder->buffer_length
           && request_decoder->state != LET_NETWORK_REQUEST_DECODER_STATE_END) {
        const auto current_byte = request_decoder->buffer[buffer_index];

        // Process the current byte based on the current state of the decoder.
        switch (request_decoder->state) {
            case LET_NETWORK_REQUEST_DECODER_STATE_COMMAND:
                decoder_result = let_network_request_decoder_run_command(request_decoder, current_byte);
                if (let_error_exists(decoder_result)) {
                    return decoder_result;
                }

                break;
            case LET_NETWORK_REQUEST_DECODER_STATE_ARGUMENT:
                decoder_result = let_network_request_decoder_run_argument(request_decoder, current_byte);
                if (let_error_exists(decoder_result)) {
                    return decoder_result;
                }

                break;
            case LET_NETWORK_REQUEST_DECODER_STATE_END:
                break;
        }

        buffer_index++;
    }

    // Ensure that all expected arguments have been processed.
    if (request_decoder->request_argc != 0) {
        return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_REQUEST_ARGUMENTS_MISSING);
    }

    // Ensure that the decoder has reached the end state and that the buffer has been fully consumed.
    if (request_decoder->state != LET_NETWORK_REQUEST_DECODER_STATE_END
        || buffer_index != request_decoder->buffer_length) {
        return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_REQUEST_MALFORMED);
    }

    return decoder_result;
}

// -----------------------------------------------------------------------------
// Internal Functions
// -----------------------------------------------------------------------------

static let_error_t let_network_request_decoder_run_command(let_network_request_decoder_t *request_decoder,
                                                           const let_u8_t current_byte) {
    switch (current_byte) {
        case ';':
            request_decoder->request.type = LET_NETWORK_REQUEST_TYPE_MAGIC;
            break;
        case '+':
            request_decoder->request.type = LET_NETWORK_REQUEST_TYPE_ADD_ACCOUNT; // wal_id, credits, debits, flags
            request_decoder->request_argc = 4;
            break;
        case '%':
            request_decoder->request.type = LET_NETWORK_REQUEST_TYPE_MAKE_TRANSFER; // wal_id, from_id, to_id, amount
            request_decoder->request_argc = 4;
            break;
        case '?':
            request_decoder->request.type = LET_NETWORK_REQUEST_TYPE_GET_ACCOUNT; // account_id
            request_decoder->request_argc = 1;
            break;
        case '#':
            request_decoder->request.type = LET_NETWORK_REQUEST_TYPE_COUNT_DATABASE;
            break;
        case '=':
            request_decoder->request.type = LET_NETWORK_REQUEST_TYPE_UPDATE_ACCOUNT; // wal_id, account_id, flags
            request_decoder->request_argc = 3;
            break;
        case '.':
            request_decoder->request.type = LET_NETWORK_REQUEST_TYPE_CLOSE;
            break;
        default:
            return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_REQUEST_INVALID_COMMAND);
    }

    request_decoder->state = LET_NETWORK_REQUEST_DECODER_STATE_ARGUMENT;
    return let_error_none();
}

static let_error_t let_network_request_decoder_run_argument(let_network_request_decoder_t *request_decoder,
                                                            const let_u8_t current_byte) {
    auto argument_result = let_error_none();

    // If no arguments are expected, the next byte must be a space to indicate the end of the request.
    if (request_decoder->request_argc == 0) {
        if (current_byte == ' ') {
            request_decoder->state = LET_NETWORK_REQUEST_DECODER_STATE_END;
            return argument_result;
        }

        return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_REQUEST_EXPECTED_NEW_LINE);
    }

    // If the last argument has been processed, the next byte must be a space to indicate the end of the request.
    if (request_decoder->request_argc == 1 && current_byte == ' ') {
        request_decoder->request_argc--;
        request_decoder->state = LET_NETWORK_REQUEST_DECODER_STATE_END;

        return argument_result;
    }

    // Parse the current argument based on the request type and the number of remaining arguments.
    switch (request_decoder->request.type) {
        case LET_NETWORK_REQUEST_TYPE_ADD_ACCOUNT:
            if (request_decoder->request_argc == 4) {
                argument_result = let_network_request_decoder_parse_u64(
                    request_decoder,
                    current_byte,
                    &request_decoder->request.data.create_account.wal_id
                );
            } else if (request_decoder->request_argc == 3) {
                argument_result = let_network_request_decoder_parse_u128(
                    request_decoder,
                    current_byte,
                    &request_decoder->request.data.create_account.credits
                );
            } else if (request_decoder->request_argc == 2) {
                argument_result = let_network_request_decoder_parse_u128(
                    request_decoder,
                    current_byte,
                    &request_decoder->request.data.create_account.debits
                );
            } else if (request_decoder->request_argc == 1) {
                argument_result = let_network_request_decoder_parse_u8(
                    request_decoder,
                    current_byte,
                    &request_decoder->request.data.create_account.flags
                );
            }

            break;
        case LET_NETWORK_REQUEST_TYPE_MAKE_TRANSFER:
            if (request_decoder->request_argc == 4) {
                argument_result = let_network_request_decoder_parse_u64(
                    request_decoder,
                    current_byte,
                    &request_decoder->request.data.make_transfer.wal_id
                );
            } else if (request_decoder->request_argc == 3) {
                argument_result = let_network_request_decoder_parse_u64(
                    request_decoder,
                    current_byte,
                    &request_decoder->request.data.make_transfer.from_id
                );
            } else if (request_decoder->request_argc == 2) {
                argument_result = let_network_request_decoder_parse_u64(
                    request_decoder,
                    current_byte,
                    &request_decoder->request.data.make_transfer.to_id
                );
            } else if (request_decoder->request_argc == 1) {
                argument_result = let_network_request_decoder_parse_u128(
                    request_decoder,
                    current_byte,
                    &request_decoder->request.data.make_transfer.amount
                );
            }

            break;
        case LET_NETWORK_REQUEST_TYPE_GET_ACCOUNT:
            if (request_decoder->request_argc == 1) {
                argument_result = let_network_request_decoder_parse_u64(
                    request_decoder,
                    current_byte,
                    &request_decoder->request.data.get_account
                );
            }

            break;
        case LET_NETWORK_REQUEST_TYPE_UPDATE_ACCOUNT:
            if (request_decoder->request_argc == 3) {
                argument_result = let_network_request_decoder_parse_u64(
                    request_decoder,
                    current_byte,
                    &request_decoder->request.data.update_account.wal_id
                );
            } else if (request_decoder->request_argc == 2) {
                argument_result = let_network_request_decoder_parse_u64(
                    request_decoder,
                    current_byte,
                    &request_decoder->request.data.update_account.account_id
                );
            } else if (request_decoder->request_argc == 1) {
                argument_result = let_network_request_decoder_parse_u8(
                    request_decoder,
                    current_byte,
                    &request_decoder->request.data.update_account.flags
                );
            }

            break;
        default:
            break;
    }

    return argument_result;
}


static let_error_t let_network_request_decoder_parse_u128(let_network_request_decoder_t *request_decoder,
                                                          const let_u8_t current_byte,
                                                          let_u128_t *output) {
    // Validate that the current byte is a digit, space, or underscore.
    if (current_byte >= '0' && current_byte <= '9') {
        const let_u8_t digit = current_byte - '0'; // Convert ASCII character to its numeric value

        // Check for overflow before multiplying the current output by 10.
        if (*output > LET_U128_MAX / 10) {
            return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_REQUEST_INTEGER_OVERFLOW);
        }

        *output *= 10;

        // Check for overflow before adding the digit to the output.
        if (digit > LET_U128_MAX - *output) {
            return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_REQUEST_INTEGER_OVERFLOW);
        }

        *output += digit;
    } else if (current_byte == ' ') {
        request_decoder->request_argc--;
    } else if (current_byte != '_') {
        // Allow underscores inside integers to serve as human-readable digit separators.
        // Any other non-numeric character is a validation failure.
        return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_REQUEST_INVALID_INTEGER);
    }

    return let_error_none();
}

static let_error_t let_network_request_decoder_parse_u64(let_network_request_decoder_t *request_decoder,
                                                         const let_u8_t current_byte,
                                                         let_u64_t *output) {
    let_u128_t current_output = *output;

    const auto parse_result = let_network_request_decoder_parse_u128(request_decoder, current_byte, &current_output);
    if (let_error_exists(parse_result)) {
        return parse_result;
    }

    if (current_output > LET_U64_MAX) {
        return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_REQUEST_INTEGER_OVERFLOW);
    }

    *output = (let_u64_t) current_output;
    return let_error_none();
}

static let_error_t let_network_request_decoder_parse_u8(let_network_request_decoder_t *request_decoder,
                                                        const let_u8_t current_byte,
                                                        let_u8_t *output) {
    let_u128_t current_output = *output;

    const auto parse_result = let_network_request_decoder_parse_u128(request_decoder, current_byte, &current_output);
    if (let_error_exists(parse_result)) {
        return parse_result;
    }

    if (current_output > LET_U8_MAX) {
        return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_REQUEST_INTEGER_OVERFLOW);
    }

    *output = (let_u8_t) current_output;
    return let_error_none();
}

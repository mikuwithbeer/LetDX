/**
 * @file encoder.c
 * @brief The network response encoder implementation.
 */

#include "let/network/response/encoder.h"

// ----------------------------------------------------------------------------
// Forward Declarations
// -----------------------------------------------------------------------------

static let_error_t let_network_response_encoder_run_integer(let_network_response_encoder_t *response_encoder,
                                                            const void *value,
                                                            let_size_t size);

// -----------------------------------------------------------------------------
// Function Implementations
// -----------------------------------------------------------------------------

let_network_response_encoder_t let_network_response_encoder_empty(void) {
    return (let_network_response_encoder_t){};
}

void let_network_response_encoder_init(let_network_response_encoder_t *response_encoder,
                                       const let_network_response_t network_response,
                                       let_u8_t *buffer,
                                       const let_size_t buffer_capacity) {
    response_encoder->response = network_response;

    response_encoder->buffer = buffer;
    response_encoder->buffer_capacity = buffer_capacity;
}

let_error_t let_network_response_encoder_run(let_network_response_encoder_t *response_encoder) {
    auto encoder_result = let_error_none();

    switch (response_encoder->response.type) {
        case LET_NETWORK_RESPONSE_TYPE_MAGIC:
            response_encoder->buffer[response_encoder->buffer_length++] = 'L';
            response_encoder->buffer[response_encoder->buffer_length++] = 'E';
            response_encoder->buffer[response_encoder->buffer_length++] = 'T';
            break;
        case LET_NETWORK_RESPONSE_TYPE_ADD_ACCOUNT:
            response_encoder->buffer[response_encoder->buffer_length++] = 'A';
            response_encoder->buffer[response_encoder->buffer_length++] = 'I';
            response_encoder->buffer[response_encoder->buffer_length++] = 'D';
            response_encoder->buffer[response_encoder->buffer_length++] = ' ';

            // Encode the account ID.
            encoder_result = let_network_response_encoder_run_integer(
                response_encoder,
                &response_encoder->response.data.add_account,
                sizeof(response_encoder->response.data.add_account));

            if (let_error_exists(encoder_result)) {
                return encoder_result;
            }

            break;
        case LET_NETWORK_RESPONSE_TYPE_GET_ACCOUNT:
            response_encoder->buffer[response_encoder->buffer_length++] = 'A';
            response_encoder->buffer[response_encoder->buffer_length++] = 'C';
            response_encoder->buffer[response_encoder->buffer_length++] = 'C';
            response_encoder->buffer[response_encoder->buffer_length++] = ' ';

            // Encode the account credits.
            encoder_result = let_network_response_encoder_run_integer(
                response_encoder,
                &response_encoder->response.data.get_account.credits,
                sizeof(response_encoder->response.data.get_account.credits));

            if (let_error_exists(encoder_result)) {
                return encoder_result;
            }

            response_encoder->buffer[response_encoder->buffer_length++] = ' ';

            // Encode the account debits.
            encoder_result = let_network_response_encoder_run_integer(
                response_encoder,
                &response_encoder->response.data.get_account.debits,
                sizeof(response_encoder->response.data.get_account.debits));

            if (let_error_exists(encoder_result)) {
                return encoder_result;
            }

            response_encoder->buffer[response_encoder->buffer_length++] = ' ';

            // Encode the account flags.
            encoder_result = let_network_response_encoder_run_integer(
                response_encoder,
                &response_encoder->response.data.get_account.flags,
                sizeof(response_encoder->response.data.get_account.flags));

            if (let_error_exists(encoder_result)) {
                return encoder_result;
            }

            break;
        case LET_NETWORK_RESPONSE_TYPE_COUNT_DATABASE:
            response_encoder->buffer[response_encoder->buffer_length++] = 'L';
            response_encoder->buffer[response_encoder->buffer_length++] = 'E';
            response_encoder->buffer[response_encoder->buffer_length++] = 'N';
            response_encoder->buffer[response_encoder->buffer_length++] = ' ';

            // Encode the count of accounts.
            encoder_result = let_network_response_encoder_run_integer(
                response_encoder,
                &response_encoder->response.data.count_database.accounts,
                sizeof(response_encoder->response.data.count_database.accounts));

            if (let_error_exists(encoder_result)) {
                return encoder_result;
            }

            response_encoder->buffer[response_encoder->buffer_length++] = ' ';

            // Encode the count of entries.
            encoder_result = let_network_response_encoder_run_integer(
                response_encoder,
                &response_encoder->response.data.count_database.entries,
                sizeof(response_encoder->response.data.count_database.entries));

            if (let_error_exists(encoder_result)) {
                return encoder_result;
            }

            break;
        case LET_NETWORK_RESPONSE_TYPE_OK:
            response_encoder->buffer[response_encoder->buffer_length++] = 'O';
            response_encoder->buffer[response_encoder->buffer_length++] = 'K';
            response_encoder->buffer[response_encoder->buffer_length++] = 'E';
            break;
        case LET_NETWORK_RESPONSE_TYPE_ERROR:
            response_encoder->buffer[response_encoder->buffer_length++] = 'E';
            response_encoder->buffer[response_encoder->buffer_length++] = 'R';
            response_encoder->buffer[response_encoder->buffer_length++] = 'R';
            response_encoder->buffer[response_encoder->buffer_length++] = ' ';

            const auto error_code = let_error_code(response_encoder->response.data.error);

            // Encode the error code.
            encoder_result = let_network_response_encoder_run_integer(
                response_encoder,
                &error_code,
                sizeof(error_code));

            if (let_error_exists(encoder_result)) {
                return encoder_result;
            }

            break;
    }

    response_encoder->buffer[response_encoder->buffer_length++] = '\n'; // Newline terminator
    return encoder_result;
}

// ----------------------------------------------------------------------------
// Internal Functions
// -----------------------------------------------------------------------------

static let_error_t let_network_response_encoder_run_integer(let_network_response_encoder_t *response_encoder,
                                                            const void *value,
                                                            const let_size_t size) {
    const auto bytes = (const let_u8_t *) value;
    auto skip = size;

    // Skip leading zero bytes for more compact representation.
    while (skip > 1 && bytes[skip - 1] == 0) {
        skip--;
    }

    // Check if the buffer has enough capacity to hold the encoded hexadecimal representation.
    if (response_encoder->buffer_length + skip * 2 > response_encoder->buffer_capacity) {
        return let_error_new(LET_ERROR_ID_NETWORK, LET_ERROR_NETWORK_RESPONSE_BUFFER_OVERFLOW);
    }

    // Convert the bytes to its hexadecimal representation and store it in the buffer.
    for (let_size_t index = skip; index-- > 0;) {
        const auto byte = bytes[index];

        response_encoder->buffer[response_encoder->buffer_length++] = LET_NETWORK_RESPONSE_ENCODER_HEX_DIGITS[
            byte >> 4 & 0x0F
        ];

        response_encoder->buffer[response_encoder->buffer_length++] = LET_NETWORK_RESPONSE_ENCODER_HEX_DIGITS[
            byte & 0x0F
        ];
    }

    return let_error_none();
}

#ifndef LET_NETWORK_RESPONSE_ENCODER_H
#define LET_NETWORK_RESPONSE_ENCODER_H

/**
 * @file encoder.h
 * @brief The network response encoder interface.
 *
 * This header defines the structures and functions for encoding network responses.
 * It provides a way to serialize response data into a buffer for transmission over the network.
 */

#include "let/network/response/response.h"

// -----------------------------------------------------------------------------
// Compile-Time Constants
// -----------------------------------------------------------------------------

// Hexadecimal digits used for encoding binary data into a human-readable format.
constexpr let_u8_t LET_NETWORK_RESPONSE_ENCODER_HEX_DIGITS[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F'
};

// -----------------------------------------------------------------------------
// Data Structures
// -----------------------------------------------------------------------------

/**
 * @brief The network response encoder structure.
 *
 * This structure maintains the state of the encoder, including the response being serialized,
 * the buffer into which the response is written, and the current position within the buffer.
 */
typedef struct {
    let_network_response_t response;

    let_u8_t *buffer;
    let_size_t buffer_length;
    let_size_t buffer_capacity;
} let_network_response_encoder_t;

// -----------------------------------------------------------------------------
// Function Declarations
// -----------------------------------------------------------------------------

/**
 * @brief Returns an empty, zero-initialized network response encoder struct.
 */
[[nodiscard]] let_network_response_encoder_t let_network_response_encoder_empty(void);

/**
 * @brief Initializes the network response encoder with a response and a buffer.
 */
void let_network_response_encoder_init(let_network_response_encoder_t *response_encoder,
                                       let_network_response_t network_response,
                                       let_u8_t *buffer,
                                       let_size_t buffer_capacity);

/**
 * @brief Runs the network response encoder to serialize the response into the buffer.
 */
let_error_t let_network_response_encoder_run(let_network_response_encoder_t *response_encoder);

#endif //LET_NETWORK_RESPONSE_ENCODER_H

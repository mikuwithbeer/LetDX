#ifndef LET_NETWORK_REQUEST_DECODER_H
#define LET_NETWORK_REQUEST_DECODER_H

/**
 * @file decoder.h
 * @brief The network request decoder interface.
 *
 * This header defines the structures and functions for decoding network requests.
 * It provides a state machine to parse incoming requests and extract their components.
 */

#include "let/network/request/request.h"

// -----------------------------------------------------------------------------
// Data Structures
// -----------------------------------------------------------------------------

/**
 * @brief An enumeration of the states in the network request decoder state machine.
 */
typedef enum : let_u8_t {
    LET_NETWORK_REQUEST_DECODER_STATE_COMMAND, // Collecting the command token
    LET_NETWORK_REQUEST_DECODER_STATE_ARGUMENT, // Collecting the argument tokens
    LET_NETWORK_REQUEST_DECODER_STATE_END, // Finished collecting tokens
} let_network_request_decoder_state_t;

/**
 * @brief The network request decoder structure.
 *
 * This structure maintains the state of the decoder, including the current request being built,
 * the buffer being parsed, and the current state of the state machine.
 */
typedef struct {
    let_network_request_t request;
    let_u8_t request_argc;

    let_u8_t *buffer;
    let_size_t buffer_length;

    let_network_request_decoder_state_t state;
} let_network_request_decoder_t;

// -----------------------------------------------------------------------------
// Function Declarations
// -----------------------------------------------------------------------------

/**
 * @brief Returns an empty, zero-initialized network request decoder struct.
 */
[[nodiscard]] let_network_request_decoder_t let_network_request_decoder_empty(void);

/**
 * @brief Initializes the network request decoder with a buffer and its length.
 */
void let_network_request_decoder_init(let_network_request_decoder_t *request_decoder,
                                      let_u8_t *buffer,
                                      let_size_t buffer_length);

/**
 * @brief Runs the network request decoder state machine to parse the buffer.
 */
let_error_t let_network_request_decoder_run(let_network_request_decoder_t *request_decoder);

#endif //LET_NETWORK_REQUEST_DECODER_H

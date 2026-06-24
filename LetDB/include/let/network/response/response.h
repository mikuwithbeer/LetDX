#ifndef LET_NETWORK_RESPONSE_H
#define LET_NETWORK_RESPONSE_H

/**
 * @file response.h
 * @brief The network response interface.
 *
 * This header defines the structures and functions for representing and encoding
 * network responses. It provides a unified way to handle different types of responses
 * in the system.
 */

#include "let/common.h"
#include "let/error.h"

// -----------------------------------------------------------------------------
// Data Structures
// -----------------------------------------------------------------------------

/**
 * @brief An enumeration of the different types of network responses.
 */
typedef enum : let_u8_t {
    LET_NETWORK_RESPONSE_TYPE_MAGIC,
    LET_NETWORK_RESPONSE_TYPE_ADD_ACCOUNT,
    LET_NETWORK_RESPONSE_TYPE_GET_ACCOUNT,
    LET_NETWORK_RESPONSE_TYPE_COUNT_ENTRIES,
    LET_NETWORK_RESPONSE_TYPE_OK,
    LET_NETWORK_RESPONSE_TYPE_ERROR,
} let_network_response_type_t;

/**
 * @brief A union representing the data associated with different types of network responses.
 */
typedef union {
    struct {
        let_u128_t credits;
        let_u128_t debits;
        let_u8_t flags;
    } get_account;

    let_u64_t add_account; // Account ID
    let_u64_t count_entries; // Number of entries in the WAL
    let_error_t error; // Error information
} let_network_response_data_t;

/**
 * @brief Represents a network response, including its type and associated data.
 */
typedef struct {
    let_network_response_type_t type;
    let_network_response_data_t data;
} let_network_response_t;

// -----------------------------------------------------------------------------
// Function Declarations
// -----------------------------------------------------------------------------

/**
 * @brief Returns an empty, zero-initialized network response struct.
 */
[[nodiscard]] let_network_response_t let_network_response_empty(void);

/**
 * @brief Encodes a network response into a buffer for transmission.
 */
let_error_t let_network_response_encode(let_network_response_t network_response,
                                        let_u8_t *buffer,
                                        let_size_t buffer_capacity,
                                        let_size_t *output);

#endif //LET_NETWORK_RESPONSE_H

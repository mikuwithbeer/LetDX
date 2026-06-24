#ifndef LET_NETWORK_REQUEST_H
#define LET_NETWORK_REQUEST_H

/**
 * @file request.h
 * @brief The network request interface.
 *
 * This header defines the structures and functions for representing and decoding
 * network requests. It provides a unified way to handle different types of requests
 * in the system.
 */

#include "let/common.h"
#include "let/error.h"

// -----------------------------------------------------------------------------
// Data Structures
// -----------------------------------------------------------------------------

/**
 * @brief An enumeration of the different types of network requests.
 */
typedef enum : let_u8_t {
    LET_NETWORK_REQUEST_TYPE_MAGIC,
    LET_NETWORK_REQUEST_TYPE_ADD_ACCOUNT,
    LET_NETWORK_REQUEST_TYPE_MAKE_TRANSFER,
    LET_NETWORK_REQUEST_TYPE_GET_ACCOUNT,
    LET_NETWORK_REQUEST_TYPE_COUNT_ENTRIES,
    LET_NETWORK_REQUEST_TYPE_UPDATE_ACCOUNT,
    LET_NETWORK_REQUEST_TYPE_CLOSE,
} let_network_request_type_t;

/**
 * @brief A union representing the data associated with different types of network requests.
 */
typedef union {
    struct {
        let_u64_t wal_id;
        let_u128_t credits;
        let_u128_t debits;
        let_u8_t flags;
    } create_account;

    struct {
        let_u64_t wal_id;
        let_u64_t from_id;
        let_u64_t to_id;
        let_u128_t amount;
    } make_transfer;

    struct {
        let_u64_t wal_id;
        let_u64_t account_id;
        let_u8_t flags;
    } update_account;

    let_u64_t get_account; // Account ID
} let_network_request_data_t;

/**
 * @brief Represents a network request, including its type and associated data.
 */
typedef struct {
    let_network_request_type_t type;
    let_network_request_data_t data;
} let_network_request_t;

// -----------------------------------------------------------------------------
// Function Declarations
// -----------------------------------------------------------------------------

/**
 * @brief Returns an empty, zero-initialized network request struct.
 */
[[nodiscard]] let_network_request_t let_network_request_empty(void);

/**
 * @brief Decodes a network request from a buffer.
 */
let_error_t let_network_request_decode(let_u8_t *buffer,
                                       let_size_t buffer_length,
                                       let_network_request_t *output);

#endif //LET_NETWORK_REQUEST_H

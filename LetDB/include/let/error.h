#ifndef LET_ERROR_H
#define LET_ERROR_H

/**
 * @file error.h
 * @brief The error interface.
 *
 * This header defines the structures and functions for error handling in the system.
 * It provides a unified way to represent and report errors across different modules.
 */

#include "let/common.h"

// -----------------------------------------------------------------------------
// Data Structures
// -----------------------------------------------------------------------------

/**
 * @brief An enumeration of the different error IDs, representing various modules in the system.
 */
typedef enum [[nodiscard]] : let_u8_t {
    LET_ERROR_ID_NONE = 0, // No error
    LET_ERROR_ID_ACCOUNT,
    LET_ERROR_ID_STATE,
    LET_ERROR_ID_GUARD,
    LET_ERROR_ID_NETWORK,
    LET_ERROR_ID_STORAGE,
    LET_ERROR_ID_CLI,
} let_error_id_t;

/**
 * @brief An enumeration of the different error codes for the account module.
 */
typedef enum [[nodiscard]] : let_u8_t {
    LET_ERROR_ACCOUNT_OUT_OF_MEMORY = 1,
    LET_ERROR_ACCOUNT_NOT_FOUND,
    LET_ERROR_ACCOUNT_CAPACITY_OVERFLOW,
} let_error_account_t;

/**
 * @brief An enumeration of the different error codes for the state module.
 */
typedef enum [[nodiscard]] : let_u8_t {
    LET_ERROR_STATE_INVALID_ACCOUNT_LIST = 1,
    LET_ERROR_STATE_INVALID_TIME,
} let_error_state_t;

/**
 * @brief An enumeration of the different error codes for the guard module.
 */
typedef enum [[nodiscard]] : let_u8_t {
    LET_ERROR_GUARD_SAME_ACCOUNT = 1,
    LET_ERROR_GUARD_SENDER_NOT_FOUND,
    LET_ERROR_GUARD_RECIPIENT_NOT_FOUND,
    LET_ERROR_GUARD_INSUFFICIENT_BALANCE,
    LET_ERROR_GUARD_TRANSACTION_OVERFLOW,
    LET_ERROR_GUARD_ZERO_BALANCE,
    LET_ERROR_GUARD_ACCOUNT_CANNOT_SEND,
    LET_ERROR_GUARD_ACCOUNT_CANNOT_RECEIVE,
    LET_ERROR_GUARD_INVALID_STATE,
} let_error_guard_t;

/**
 * @brief An enumeration of the different error codes for the network module.
 */
typedef enum [[nodiscard]] : let_u8_t {
    LET_ERROR_NETWORK_SERVER_CREATE_FAILED = 1,
    LET_ERROR_NETWORK_SERVER_BIND_FAILED,
    LET_ERROR_NETWORK_SERVER_LISTEN_FAILED,
    LET_ERROR_NETWORK_SERVER_ACCEPT_FAILED,
    LET_ERROR_NETWORK_SERVER_READ_FAILED,
    LET_ERROR_NETWORK_SERVER_WRITE_FAILED,
    LET_ERROR_NETWORK_SERVER_READ_TIMEOUT,
    LET_ERROR_NETWORK_SERVER_WRITE_TIMEOUT,

    LET_ERROR_NETWORK_SERVER_CLOSED,
    LET_ERROR_NETWORK_CLIENT_CLOSED,

    LET_ERROR_NETWORK_REQUEST_EMPTY,
    LET_ERROR_NETWORK_REQUEST_MALFORMED,
    LET_ERROR_NETWORK_REQUEST_INVALID_COMMAND,
    LET_ERROR_NETWORK_REQUEST_INVALID_INTEGER,
    LET_ERROR_NETWORK_REQUEST_INTEGER_OVERFLOW,
    LET_ERROR_NETWORK_REQUEST_EXPECTED_NEW_LINE,
    LET_ERROR_NETWORK_REQUEST_ARGUMENTS_MISSING,

    LET_ERROR_NETWORK_REQUEST_BUFFER_OVERFLOW,
    LET_ERROR_NETWORK_RESPONSE_BUFFER_OVERFLOW,
} let_error_network_t;

/**
 * @brief An enumeration of the different error codes for the storage module.
 */
typedef enum [[nodiscard]] : let_u8_t {
    LET_ERROR_STORAGE_WAL_CREATE_FAILED = 1,
    LET_ERROR_STORAGE_WAL_OPEN_FAILED,
    LET_ERROR_STORAGE_WAL_WRITE_FAILED,
    LET_ERROR_STORAGE_WAL_READ_FAILED,
    LET_ERROR_STORAGE_WAL_SYNC_FAILED,
    LET_ERROR_STORAGE_WAL_SEEK_FAILED,
    LET_ERROR_STORAGE_WAL_TRUNCATE_FAILED,
    LET_ERROR_STORAGE_WAL_INVALID_MAGIC,
    LET_ERROR_STORAGE_WAL_INVALID_VERSION,
    LET_ERROR_STORAGE_WAL_NONCE_MISMATCH,
    LET_ERROR_STORAGE_WAL_CHECKSUM_MISMATCH,
    LET_ERROR_STORAGE_WAL_INVALID_ENTRY_TYPE,
    LET_ERROR_STORAGE_WAL_INVALID_TIME,
    LET_ERROR_STORAGE_WAL_INVALID_STATE,
    LET_ERROR_STORAGE_WAL_INVALID_PATH,
} let_error_storage_t;

/**
 * @brief An enumeration of the different error codes for the CLI module.
 */
typedef enum [[nodiscard]] : let_u8_t {
    LET_ERROR_CLI_INVALID_OPTION = 1,
    LET_ERROR_CLI_INVALID_PORT,
    LET_ERROR_CLI_INVALID_BACKLOG,
    LET_ERROR_CLI_INVALID_READ_TIMEOUT,
    LET_ERROR_CLI_INVALID_WRITE_TIMEOUT,
    LET_ERROR_CLI_INVALID_LOG_LEVEL,
} let_error_cli_t;

/**
 * @brief An enumeration of the different error actions that can be taken in response to an error.
 */
typedef enum : let_u8_t {
    LET_ERROR_ACTION_NONE = 0, // No action
    LET_ERROR_ACTION_REJECT,
    LET_ERROR_ACTION_IGNORE,
    LET_ERROR_ACTION_CLOSE,
    LET_ERROR_ACTION_FATAL,
} let_error_action_t;

/**
 * @brief Represents an error with an ID and a specific error code.
 */
typedef struct [[nodiscard]] {
    let_error_id_t id;
    let_u8_t error;
} let_error_t;

/**
 * @brief Represents an error code.
 */
typedef let_u16_t let_error_code_t;

/**
 * @brief Represents an error report, including the action to be taken and a message describing the error.
 *
 * @note The message is a static string and should not be freed or modified.
 */
typedef struct [[nodiscard]] {
    let_error_action_t action;
    const char *message;
} let_error_report_t;

// -----------------------------------------------------------------------------
// Function Declarations
// -----------------------------------------------------------------------------

/**
 * @brief Creates a new error with no error code.
 */
let_error_t let_error_none(void);

/**
 * @brief Creates a new error with the specified ID and error code.
 */
let_error_t let_error_new(let_error_id_t id,
                          let_u8_t code);

/**
 * @brief Returns the error code for the specified error.
 */
let_error_code_t let_error_code(let_error_t error);

/**
 * @brief Generates an error report for the specified error.
 */
let_error_report_t let_error_report(let_error_t error);

/**
 * @brief Checks if the specified error exists.
 *
 * @return true if the error exists, false otherwise.
 */
[[nodiscard, maybe_unused]] static inline bool let_error_exists(const let_error_t error) {
    return error.id != LET_ERROR_ID_NONE;
}

#endif //LET_ERROR_H

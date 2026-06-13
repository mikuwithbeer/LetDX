#ifndef LET_ERROR_H
#define LET_ERROR_H

#include "common.h"

typedef enum [[nodiscard]] : let_u8_t {
    LET_ERROR_ID_NONE = 0,
    LET_ERROR_ID_ACCOUNT,
    LET_ERROR_ID_STATE,
    LET_ERROR_ID_GUARD,
    LET_ERROR_ID_NETWORK,
    LET_ERROR_ID_STORAGE,
    LET_ERROR_ID_CLI,
} let_error_id_t;

typedef enum [[nodiscard]] : let_u8_t {
    LET_ERROR_ACCOUNT_OUT_OF_MEMORY = 1,
    LET_ERROR_ACCOUNT_NOT_FOUND,
    LET_ERROR_ACCOUNT_CAPACITY_OVERFLOW,
} let_error_account_t;

typedef enum [[nodiscard]] : let_u8_t {
    LET_ERROR_STATE_OUT_OF_MEMORY = 1,
    LET_ERROR_STATE_INVALID_ACCOUNT_LIST,
    LET_ERROR_STATE_INVALID_TIME,
} let_error_state_t;

typedef enum [[nodiscard]] : let_u8_t {
    LET_ERROR_GUARD_SAME_ACCOUNT = 1,
    LET_ERROR_GUARD_ACCOUNT_NOT_FOUND,
    LET_ERROR_GUARD_INSUFFICIENT_BALANCE,
    LET_ERROR_GUARD_TRANSACTION_OVERFLOW,
    LET_ERROR_GUARD_ZERO_BALANCE,
    LET_ERROR_GUARD_ACCOUNT_CANNOT_SEND,
    LET_ERROR_GUARD_ACCOUNT_CANNOT_RECEIVE,
    LET_ERROR_GUARD_INVALID_STATE,
} let_error_guard_t;

typedef enum [[nodiscard]] : let_u8_t {
    LET_ERROR_NETWORK_SERVER_CREATE_FAILED = 1,
    LET_ERROR_NETWORK_SERVER_BIND_FAILED,
    LET_ERROR_NETWORK_SERVER_LISTEN_FAILED,
    LET_ERROR_NETWORK_SERVER_ACCEPT_FAILED,
    LET_ERROR_NETWORK_SERVER_READ_FAILED,
    LET_ERROR_NETWORK_SERVER_WRITE_FAILED,
    LET_ERROR_NETWORK_SERVER_CLOSED,
    LET_ERROR_NETWORK_SERVER_READ_TIMEOUT,
    LET_ERROR_NETWORK_SERVER_WRITE_TIMEOUT,

    LET_ERROR_NETWORK_REQUEST_EMPTY,
    LET_ERROR_NETWORK_REQUEST_MALFORMED,
    LET_ERROR_NETWORK_REQUEST_INVALID_COMMAND,
    LET_ERROR_NETWORK_REQUEST_INVALID_INTEGER,
    LET_ERROR_NETWORK_REQUEST_INTEGER_OVERFLOW,
    LET_ERROR_NETWORK_REQUEST_EXPECTED_NEW_LINE,
    LET_ERROR_NETWORK_REQUEST_ARGUMENTS_MISSING,
} let_error_network_t;

typedef enum [[nodiscard]] : let_u8_t {
    LET_ERROR_STORAGE_WAL_CREATE_FAILED = 1,
    LET_ERROR_STORAGE_WAL_WRITE_FAILED,
    LET_ERROR_STORAGE_WAL_READ_FAILED,
    LET_ERROR_STORAGE_WAL_SYNC_FAILED,
    LET_ERROR_STORAGE_WAL_SEEK_FAILED,
    LET_ERROR_STORAGE_WAL_INVALID_MAGIC,
    LET_ERROR_STORAGE_WAL_INVALID_VERSION,
    LET_ERROR_STORAGE_WAL_NONCE_MISMATCH,
    LET_ERROR_STORAGE_WAL_CHECKSUM_MISMATCH,
    LET_ERROR_STORAGE_WAL_INVALID_ENTRY_TYPE,
} let_error_storage_t;

typedef enum [[nodiscard]] : let_u8_t {
    LET_ERROR_CLI_INVALID_OPTION = 1,
    LET_ERROR_CLI_INVALID_PORT,
    LET_ERROR_CLI_INVALID_BACKLOG,
    LET_ERROR_CLI_INVALID_READ_TIMEOUT,
    LET_ERROR_CLI_INVALID_WRITE_TIMEOUT,
} let_error_cli_t;

typedef enum : let_u8_t {
    LET_ERROR_ACTION_IGNORE,
    LET_ERROR_ACTION_REJECT,
    LET_ERROR_ACTION_FATAL,
} let_error_action_t;

typedef struct [[nodiscard]] {
    let_error_id_t id;
    let_u8_t error;
} let_error_t;

typedef struct [[nodiscard]] {
    let_error_action_t action;
    const char *message;
} let_error_report_t;

typedef let_u16_t let_error_code_t;

let_error_t let_error_none(void);

let_error_t let_error_new(let_error_id_t id,
                          let_u8_t code);

let_error_report_t let_error_report(let_error_t error);

let_error_code_t let_error_code(let_error_t error);

[[nodiscard, maybe_unused]] static inline bool let_error_exists(const let_error_t error) {
    return error.id != LET_ERROR_ID_NONE;
}

#endif //LET_ERROR_H

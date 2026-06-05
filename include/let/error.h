#ifndef LET_ERROR_H
#define LET_ERROR_H

#include "common.h"

typedef enum [[nodiscard]] : let_u8_t {
    LET_ERROR_ACCOUNT_OUT_OF_MEMORY = 1,
    LET_ERROR_ACCOUNT_NOT_FOUND,
} let_error_account_t;

typedef enum [[nodiscard]] : let_u8_t {
    LET_ERROR_STATE_OUT_OF_MEMORY = 1,
    LET_ERROR_STATE_INVALID_ACCOUNT_LIST,
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

    LET_ERROR_NETWORK_REQUEST_UNKNOWN_COMMAND,
    LET_ERROR_NETWORK_REQUEST_INVALID_INTEGER,
    LET_ERROR_NETWORK_REQUEST_INTEGER_OVERFLOW,
    LET_ERROR_NETWORK_REQUEST_EXPECTED_NEW_LINE,
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
    LET_ERROR_STORAGE_WAL_CHECKSUM_MISMATCH
} let_error_storage_t;

typedef enum [[nodiscard]] : let_u8_t {
    LET_ERROR_ID_NONE = 0,
    LET_ERROR_ID_ACCOUNT,
    LET_ERROR_ID_STATE,
    LET_ERROR_ID_GUARD,
    LET_ERROR_ID_NETWORK,
    LET_ERROR_ID_STORAGE,
} let_error_id_t;

typedef struct [[nodiscard]] {
    let_error_id_t id;
    let_u8_t error;
} let_error_t;

typedef let_u16_t let_error_code_t;

let_error_t let_error_new(let_error_id_t id,
                          let_u8_t code);

let_error_t let_error_none(void);

let_error_code_t let_error_code(let_error_t error);

#endif //LET_ERROR_H

#include "let/error.h"

#include <stdio.h>
#include <string.h>

let_error_t let_error_new(const let_error_id_t id,
                          const let_u8_t code) {
    return (let_error_t){
        .id = id,
        .error = code
    };
}

let_error_t let_error_none(void) {
    return (let_error_t){
        .id = LET_ERROR_ID_NONE,
        .error = 0
    };
}

let_error_code_t let_error_code(const let_error_t error) {
    if (error.id == LET_ERROR_ID_NONE) {
        return 0;
    }

    let_error_code_t code = 1000;
    code *= error.id;
    code += error.error;

    return code;
}

let_size_t let_error_message(const let_error_t error,
                             char *buffer) {
    const char *start = buffer;

    switch (error.id) {
        case LET_ERROR_ID_NONE:
            return 0;
        case LET_ERROR_ID_ACCOUNT:
            LET_MEMORY_COPY(buffer, "account: ");

            switch ((let_error_account_t) error.error) {
                case LET_ERROR_ACCOUNT_OUT_OF_MEMORY:
                    LET_MEMORY_COPY(buffer, "out of memory");
                    break;
                case LET_ERROR_ACCOUNT_NOT_FOUND:
                    LET_MEMORY_COPY(buffer, "account not found");
                    break;
                case LET_ERROR_ACCOUNT_CAPACITY_OVERFLOW:
                    LET_MEMORY_COPY(buffer, "capacity overflow");
                    break;
            }

            break;
        case LET_ERROR_ID_STATE:
            LET_MEMORY_COPY(buffer, "state: ");

            switch ((let_error_state_t) error.error) {
                case LET_ERROR_STATE_OUT_OF_MEMORY:
                    LET_MEMORY_COPY(buffer, "out of memory");
                    break;
                case LET_ERROR_STATE_INVALID_ACCOUNT_LIST:
                    LET_MEMORY_COPY(buffer, "invalid account list");
                    break;
            }

            break;
        case LET_ERROR_ID_GUARD:
            LET_MEMORY_COPY(buffer, "guard: ");

            switch ((let_error_guard_t) error.error) {
                case LET_ERROR_GUARD_SAME_ACCOUNT:
                    LET_MEMORY_COPY(buffer, "same account");
                    break;
                case LET_ERROR_GUARD_ACCOUNT_NOT_FOUND:
                    LET_MEMORY_COPY(buffer, "account not found");
                    break;
                case LET_ERROR_GUARD_INSUFFICIENT_BALANCE:
                    LET_MEMORY_COPY(buffer, "insufficient balance");
                    break;
                case LET_ERROR_GUARD_TRANSACTION_OVERFLOW:
                    LET_MEMORY_COPY(buffer, "transaction overflow");
                    break;
                case LET_ERROR_GUARD_ZERO_BALANCE:
                    LET_MEMORY_COPY(buffer, "zero balance");
                    break;
                case LET_ERROR_GUARD_ACCOUNT_CANNOT_SEND:
                    LET_MEMORY_COPY(buffer, "account cannot send");
                    break;
                case LET_ERROR_GUARD_ACCOUNT_CANNOT_RECEIVE:
                    LET_MEMORY_COPY(buffer, "account cannot receive");
                    break;
                case LET_ERROR_GUARD_INVALID_STATE:
                    LET_MEMORY_COPY(buffer, "invalid state");
                    break;
            }

            break;
        case LET_ERROR_ID_NETWORK:
            LET_MEMORY_COPY(buffer, "network: ");

            switch ((let_error_network_t) error.error) {
                case LET_ERROR_NETWORK_SERVER_CREATE_FAILED:
                    LET_MEMORY_COPY(buffer, "server create failed");
                    break;
                case LET_ERROR_NETWORK_SERVER_BIND_FAILED:
                    LET_MEMORY_COPY(buffer, "server bind failed");
                    break;
                case LET_ERROR_NETWORK_SERVER_LISTEN_FAILED:
                    LET_MEMORY_COPY(buffer, "server listen failed");
                    break;
                case LET_ERROR_NETWORK_SERVER_ACCEPT_FAILED:
                    LET_MEMORY_COPY(buffer, "server accept failed");
                    break;
                case LET_ERROR_NETWORK_SERVER_READ_FAILED:
                    LET_MEMORY_COPY(buffer, "server read failed");
                    break;
                case LET_ERROR_NETWORK_SERVER_WRITE_FAILED:
                    LET_MEMORY_COPY(buffer, "server write failed");
                    break;
                case LET_ERROR_NETWORK_SERVER_CLOSED:
                    LET_MEMORY_COPY(buffer, "server closed");
                    break;

                case LET_ERROR_NETWORK_REQUEST_UNKNOWN_COMMAND:
                    LET_MEMORY_COPY(buffer, "request unknown command");
                    break;
                case LET_ERROR_NETWORK_REQUEST_INVALID_INTEGER:
                    LET_MEMORY_COPY(buffer, "request invalid integer");
                    break;
                case LET_ERROR_NETWORK_REQUEST_INTEGER_OVERFLOW:
                    LET_MEMORY_COPY(buffer, "request integer overflow");
                    break;
                case LET_ERROR_NETWORK_REQUEST_EXPECTED_NEW_LINE:
                    LET_MEMORY_COPY(buffer, "request expected new line");
                    break;
            }

            break;
        case LET_ERROR_ID_STORAGE:
            LET_MEMORY_COPY(buffer, "storage: ");

            switch ((let_error_storage_t) error.error) {
                case LET_ERROR_STORAGE_WAL_CREATE_FAILED:
                    LET_MEMORY_COPY(buffer, "wal create failed");
                    break;
                case LET_ERROR_STORAGE_WAL_WRITE_FAILED:
                    LET_MEMORY_COPY(buffer, "wal write failed");
                    break;
                case LET_ERROR_STORAGE_WAL_READ_FAILED:
                    LET_MEMORY_COPY(buffer, "wal read failed");
                    break;
                case LET_ERROR_STORAGE_WAL_SYNC_FAILED:
                    LET_MEMORY_COPY(buffer, "wal sync failed");
                    break;
                case LET_ERROR_STORAGE_WAL_SEEK_FAILED:
                    LET_MEMORY_COPY(buffer, "wal seek failed");
                    break;
                case LET_ERROR_STORAGE_WAL_INVALID_MAGIC:
                    LET_MEMORY_COPY(buffer, "wal invalid magic");
                    break;
                case LET_ERROR_STORAGE_WAL_INVALID_VERSION:
                    LET_MEMORY_COPY(buffer, "wal invalid version");
                    break;
                case LET_ERROR_STORAGE_WAL_NONCE_MISMATCH:
                    LET_MEMORY_COPY(buffer, "wal nonce mismatch");
                    break;
                case LET_ERROR_STORAGE_WAL_CHECKSUM_MISMATCH:
                    LET_MEMORY_COPY(buffer, "wal checksum mismatch");
                    break;
            }

            break;
        case LET_ERROR_ID_CLI:
            LET_MEMORY_COPY(buffer, "cli: ");

            switch ((let_error_cli_t) error.error) {
                case LET_ERROR_CLI_INVALID_OPTION:
                    LET_MEMORY_COPY(buffer, "invalid option");
                    break;
                case LET_ERROR_CLI_INVALID_PORT:
                    LET_MEMORY_COPY(buffer, "invalid port");
                    break;
                case LET_ERROR_CLI_INVALID_BACKLOG:
                    LET_MEMORY_COPY(buffer, "invalid backlog");
                    break;
                case LET_ERROR_CLI_INVALID_FILE:
                    LET_MEMORY_COPY(buffer, "invalid file");
                    break;
            }

            break;
    }

    *buffer = '\0';
    return (let_size_t) (buffer - start);
}

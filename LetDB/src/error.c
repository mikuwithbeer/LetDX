#include "let/error.h"

let_error_t let_error_none(void) {
    return (let_error_t){
        .id = LET_ERROR_ID_NONE,
        .error = 0
    };
}

let_error_t let_error_new(const let_error_id_t id,
                          const let_u8_t code) {
    return (let_error_t){
        .id = id,
        .error = code
    };
}

let_error_report_t let_error_report(const let_error_t error) {
    let_error_report_t error_report = {
        .action = LET_ERROR_ACTION_IGNORE,
        .message = "No error"
    };

    switch (error.id) {
        case LET_ERROR_ID_NONE:
            break;
        case LET_ERROR_ID_ACCOUNT: {
            switch ((let_error_account_t) error.error) {
                case LET_ERROR_ACCOUNT_OUT_OF_MEMORY:
                    error_report.action = LET_ERROR_ACTION_FATAL;
                    error_report.message = "Account subsystem ran out of memory";
                    break;
                case LET_ERROR_ACCOUNT_NOT_FOUND:
                    error_report.action = LET_ERROR_ACTION_REJECT;
                    error_report.message = "Account not found";
                    break;
                case LET_ERROR_ACCOUNT_CAPACITY_OVERFLOW:
                    error_report.action = LET_ERROR_ACTION_REJECT;
                    error_report.message = "Account subsystem capacity overflow";
                    break;
            }

            break;
        }
        case LET_ERROR_ID_STATE: {
            error_report.action = LET_ERROR_ACTION_FATAL;
            switch ((let_error_state_t) error.error) {
                case LET_ERROR_STATE_OUT_OF_MEMORY:
                    error_report.message = "State subsystem ran out of memory";
                    break;
                case LET_ERROR_STATE_INVALID_ACCOUNT_LIST:
                    error_report.message = "Corrupted account list given for the state subsystem";
                    break;
                case LET_ERROR_STATE_INVALID_TIME:
                    error_report.message = "Invalid time given for the state subsystem";
                    break;
            }

            break;
        }
        case LET_ERROR_ID_GUARD: {
            error_report.action = LET_ERROR_ACTION_REJECT;
            switch ((let_error_guard_t) error.error) {
                case LET_ERROR_GUARD_SAME_ACCOUNT:
                    error_report.message = "Same account given for the guard subsystem";
                    break;
                case LET_ERROR_GUARD_ACCOUNT_NOT_FOUND:
                    error_report.message = "Account not found";
                    break;
                case LET_ERROR_GUARD_INSUFFICIENT_BALANCE:
                    error_report.message = "Insufficient balance";
                    break;
                case LET_ERROR_GUARD_TRANSACTION_OVERFLOW:
                    error_report.message = "Transaction overflow";
                    break;
                case LET_ERROR_GUARD_ZERO_BALANCE:
                    error_report.message = "Cannot send due to zero balance";
                    break;
                case LET_ERROR_GUARD_ACCOUNT_CANNOT_SEND:
                    error_report.message = "Account cannot send";
                    break;
                case LET_ERROR_GUARD_ACCOUNT_CANNOT_RECEIVE:
                    error_report.message = "Account cannot receive";
                    break;
                case LET_ERROR_GUARD_INVALID_STATE:
                    error_report.action = LET_ERROR_ACTION_FATAL;
                    error_report.message = "Corrupted state given for the guard subsystem";
                    break;
            }

            break;
        }
        case LET_ERROR_ID_NETWORK: {
            switch ((let_error_network_t) error.error) {
                case LET_ERROR_NETWORK_SERVER_CREATE_FAILED:
                    error_report.action = LET_ERROR_ACTION_FATAL;
                    error_report.message = "Network server creation failed";
                    break;
                case LET_ERROR_NETWORK_SERVER_BIND_FAILED:
                    error_report.action = LET_ERROR_ACTION_FATAL;
                    error_report.message = "Network server binding failed";
                    break;
                case LET_ERROR_NETWORK_SERVER_LISTEN_FAILED:
                    error_report.action = LET_ERROR_ACTION_FATAL;
                    error_report.message = "Network server listening failed";
                    break;
                case LET_ERROR_NETWORK_SERVER_ACCEPT_FAILED:
                    error_report.action = LET_ERROR_ACTION_FATAL;
                    error_report.message = "Network server accept failed";
                    break;
                case LET_ERROR_NETWORK_SERVER_READ_FAILED:
                    error_report.action = LET_ERROR_ACTION_FATAL;
                    error_report.message = "Network server read failed";
                    break;
                case LET_ERROR_NETWORK_SERVER_WRITE_FAILED:
                    error_report.action = LET_ERROR_ACTION_FATAL;
                    error_report.message = "Network server write failed";
                    break;
                case LET_ERROR_NETWORK_SERVER_CLOSED:
                    error_report.action = LET_ERROR_ACTION_IGNORE;
                    error_report.message = "Network server closed";
                    break;
                case LET_ERROR_NETWORK_SERVER_READ_TIMEOUT:
                    error_report.action = LET_ERROR_ACTION_IGNORE;
                    error_report.message = "Network server read timeout";
                    break;
                case LET_ERROR_NETWORK_SERVER_WRITE_TIMEOUT:
                    error_report.action = LET_ERROR_ACTION_IGNORE;
                    error_report.message = "Network server write timeout";
                    break;

                case LET_ERROR_NETWORK_REQUEST_EMPTY:
                    error_report.action = LET_ERROR_ACTION_REJECT;
                    error_report.message = "Empty network request";
                    break;
                case LET_ERROR_NETWORK_REQUEST_MALFORMED:
                    error_report.action = LET_ERROR_ACTION_REJECT;
                    error_report.message = "Malformed network request";
                    break;
                case LET_ERROR_NETWORK_REQUEST_INVALID_COMMAND:
                    error_report.action = LET_ERROR_ACTION_REJECT;
                    error_report.message = "Unknown network request command";
                    break;
                case LET_ERROR_NETWORK_REQUEST_INVALID_INTEGER:
                    error_report.action = LET_ERROR_ACTION_REJECT;
                    error_report.message = "Invalid network request integer";
                    break;
                case LET_ERROR_NETWORK_REQUEST_INTEGER_OVERFLOW:
                    error_report.action = LET_ERROR_ACTION_REJECT;
                    error_report.message = "Network request possess integer overflow";
                    break;
                case LET_ERROR_NETWORK_REQUEST_EXPECTED_NEW_LINE:
                    error_report.action = LET_ERROR_ACTION_REJECT;
                    error_report.message = "Expected new line after network request";
                    break;
                case LET_ERROR_NETWORK_REQUEST_ARGUMENTS_MISSING:
                    error_report.action = LET_ERROR_ACTION_REJECT;
                    error_report.message = "Network request arguments missing";
                    break;
            }

            break;
        }
        case LET_ERROR_ID_STORAGE: {
            error_report.action = LET_ERROR_ACTION_FATAL;
            switch ((let_error_storage_t) error.error) {
                case LET_ERROR_STORAGE_WAL_CREATE_FAILED:
                    error_report.message = "Storage creation failed";
                    break;
                case LET_ERROR_STORAGE_WAL_WRITE_FAILED:
                    error_report.message = "Storage write failed";
                    break;
                case LET_ERROR_STORAGE_WAL_READ_FAILED:
                    error_report.message = "Storage read failed";
                    break;
                case LET_ERROR_STORAGE_WAL_SYNC_FAILED:
                    error_report.message = "Storage sync failed";
                    break;
                case LET_ERROR_STORAGE_WAL_SEEK_FAILED:
                    error_report.message = "Storage seek failed";
                    break;
                case LET_ERROR_STORAGE_WAL_INVALID_MAGIC:
                    error_report.message = "Storage received invalid magic";
                    break;
                case LET_ERROR_STORAGE_WAL_INVALID_VERSION:
                    error_report.message = "Storage received invalid version";
                    break;
                case LET_ERROR_STORAGE_WAL_NONCE_MISMATCH:
                    error_report.action = LET_ERROR_ACTION_REJECT;
                    error_report.message = "Storage received nonce mismatch";
                    break;
                case LET_ERROR_STORAGE_WAL_CHECKSUM_MISMATCH:
                    error_report.message = "Storage received checksum mismatch";
                    break;
                case LET_ERROR_STORAGE_WAL_INVALID_ENTRY_TYPE:
                    error_report.message = "Storage received invalid entry type";
                    break;
            }

            break;
        }
        case LET_ERROR_ID_CLI: {
            error_report.action = LET_ERROR_ACTION_FATAL;
            switch ((let_error_cli_t) error.error) {
                case LET_ERROR_CLI_INVALID_OPTION:
                    error_report.message = "Invalid CLI option given";
                    break;
                case LET_ERROR_CLI_INVALID_PORT:
                    error_report.message = "Invalid port number given";
                    break;
                case LET_ERROR_CLI_INVALID_BACKLOG:
                    error_report.message = "Invalid backlog size given";
                    break;
                case LET_ERROR_CLI_INVALID_READ_TIMEOUT:
                    error_report.message = "Invalid read timeout given";
                    break;
                case LET_ERROR_CLI_INVALID_WRITE_TIMEOUT:
                    error_report.message = "Invalid write timeout given";
                    break;
            }

            break;
        }
    }

    return error_report;
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

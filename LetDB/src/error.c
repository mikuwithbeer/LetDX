#include "let/error.h"

#include <stdio.h>

let_error_t let_error_none(void) {
    return (let_error_t){
        .id = LET_ERROR_ID_NONE,
        .error = 0
    };
}

let_error_t let_error_new(const let_error_id_t id,
                          const let_u8_t code) {
    const auto error = (let_error_t){
        .id = id,
        .error = code
    };

    const auto report = let_error_report(error);
    let_error_print(report);

    return error;
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

let_error_report_t let_error_report(const let_error_t error) {
    let_error_report_t error_report = {
        .timestamp = time(nullptr), // TODO: handle edge cases
        .action = LET_ERROR_ACTION_IGNORE,
        .message = "No error occurred"
    };

    switch (error.id) {
        case LET_ERROR_ID_NONE:
            break;
        case LET_ERROR_ID_ACCOUNT: {
            switch ((let_error_account_t) error.error) {
                case LET_ERROR_ACCOUNT_OUT_OF_MEMORY:
                    error_report.action = LET_ERROR_ACTION_FATAL;
                    error_report.message = "The account subsystem ran out of memory";
                    break;
                case LET_ERROR_ACCOUNT_NOT_FOUND:
                    error_report.action = LET_ERROR_ACTION_REJECT;
                    error_report.message = "The requested account cannot be resolved";
                    break;
                case LET_ERROR_ACCOUNT_CAPACITY_OVERFLOW:
                    error_report.action = LET_ERROR_ACTION_REJECT;
                    error_report.message = "The account registry is completely full";
                    break;
            }

            break;
        }
        case LET_ERROR_ID_STATE: {
            error_report.action = LET_ERROR_ACTION_FATAL;
            switch ((let_error_state_t) error.error) {
                case LET_ERROR_STATE_INVALID_ACCOUNT_LIST:
                    error_report.message = "The provided account list contains corrupted data";
                    break;
                case LET_ERROR_STATE_INVALID_TIME:
                    error_report.message = "The provided timestamp for state subsystem is invalid";
                    break;
            }

            break;
        }
        case LET_ERROR_ID_GUARD: {
            error_report.action = LET_ERROR_ACTION_REJECT;
            switch ((let_error_guard_t) error.error) {
                case LET_ERROR_GUARD_SAME_ACCOUNT:
                    error_report.message = "The source and destination accounts must be different";
                    break;
                case LET_ERROR_GUARD_ACCOUNT_NOT_FOUND:
                    error_report.message = "The transaction sender or recipient was not found";
                    break;
                case LET_ERROR_GUARD_INSUFFICIENT_BALANCE:
                    error_report.message = "The account balance is insufficient for this transfer";
                    break;
                case LET_ERROR_GUARD_TRANSACTION_OVERFLOW:
                    error_report.message = "The transaction registry is completely full";
                    break;
                case LET_ERROR_GUARD_ZERO_BALANCE:
                    error_report.message = "The account can only send positive amount of funds";
                    break;
                case LET_ERROR_GUARD_ACCOUNT_CANNOT_SEND:
                    error_report.message = "The account is not authorized to send funds";
                    break;
                case LET_ERROR_GUARD_ACCOUNT_CANNOT_RECEIVE:
                    error_report.message = "The account is not authorized to receive funds";
                    break;
                case LET_ERROR_GUARD_INVALID_STATE:
                    error_report.action = LET_ERROR_ACTION_FATAL;
                    error_report.message = "The guard subsystem has been corrupted unexpectedly";
                    break;
            }

            break;
        }
        case LET_ERROR_ID_NETWORK: {
            switch ((let_error_network_t) error.error) {
                case LET_ERROR_NETWORK_SERVER_CREATE_FAILED:
                    error_report.action = LET_ERROR_ACTION_FATAL;
                    error_report.message = "The server failed to create the TCP socket";
                    break;
                case LET_ERROR_NETWORK_SERVER_BIND_FAILED:
                    error_report.action = LET_ERROR_ACTION_FATAL;
                    error_report.message = "The server failed to bind the port";
                    break;
                case LET_ERROR_NETWORK_SERVER_LISTEN_FAILED:
                    error_report.action = LET_ERROR_ACTION_FATAL;
                    error_report.message = "The server failed to listen for incoming connections";
                    break;
                case LET_ERROR_NETWORK_SERVER_ACCEPT_FAILED:
                    error_report.action = LET_ERROR_ACTION_FATAL;
                    error_report.message = "The server failed to accept the incoming connection";
                    break;
                case LET_ERROR_NETWORK_SERVER_READ_FAILED:
                    error_report.action = LET_ERROR_ACTION_CLOSE;
                    error_report.message = "The system failed to read data from the network";
                    break;
                case LET_ERROR_NETWORK_SERVER_WRITE_FAILED:
                    error_report.action = LET_ERROR_ACTION_CLOSE;
                    error_report.message = "The system failed to write data to the network";
                    break;
                case LET_ERROR_NETWORK_SERVER_READ_TIMEOUT:
                    error_report.action = LET_ERROR_ACTION_CLOSE;
                    error_report.message = "The network read operation timed out";
                    break;
                case LET_ERROR_NETWORK_SERVER_WRITE_TIMEOUT:
                    error_report.action = LET_ERROR_ACTION_CLOSE;
                    error_report.message = "The network write operation timed out";
                    break;
                case LET_ERROR_NETWORK_SERVER_CLOSED:
                    error_report.action = LET_ERROR_ACTION_IGNORE;
                    error_report.message = "The network server is closed";
                    break;

                case LET_ERROR_NETWORK_REQUEST_EMPTY:
                    error_report.action = LET_ERROR_ACTION_REJECT;
                    error_report.message = "The received network request contains no data";
                    break;
                case LET_ERROR_NETWORK_REQUEST_MALFORMED:
                    error_report.action = LET_ERROR_ACTION_REJECT;
                    error_report.message = "The network request format is incorrect";
                    break;
                case LET_ERROR_NETWORK_REQUEST_INVALID_COMMAND:
                    error_report.action = LET_ERROR_ACTION_REJECT;
                    error_report.message = "The requested network command is unknown";
                    break;
                case LET_ERROR_NETWORK_REQUEST_INVALID_INTEGER:
                    error_report.action = LET_ERROR_ACTION_REJECT;
                    error_report.message = "The network request integer value cannot be parsed";
                    break;
                case LET_ERROR_NETWORK_REQUEST_INTEGER_OVERFLOW:
                    error_report.action = LET_ERROR_ACTION_REJECT;
                    error_report.message = "The network request integer value is too large";
                    break;
                case LET_ERROR_NETWORK_REQUEST_EXPECTED_NEW_LINE:
                    error_report.action = LET_ERROR_ACTION_REJECT;
                    error_report.message = "The network request is missing a final newline character";
                    break;
                case LET_ERROR_NETWORK_REQUEST_ARGUMENTS_MISSING:
                    error_report.action = LET_ERROR_ACTION_REJECT;
                    error_report.message = "The network request is missing required parameters";
                    break;

                case LET_ERROR_NETWORK_REQUEST_BUFFER_OVERFLOW:
                    error_report.action = LET_ERROR_ACTION_CLOSE;
                    error_report.message = "The incoming network request exceeds the buffer size";
                    break;
                case LET_ERROR_NETWORK_RESPONSE_BUFFER_OVERFLOW:
                    error_report.action = LET_ERROR_ACTION_CLOSE;
                    error_report.message = "The outgoing network response exceeds the buffer size";
                    break;
            }

            break;
        }
        case LET_ERROR_ID_STORAGE: {
            error_report.action = LET_ERROR_ACTION_FATAL;
            switch ((let_error_storage_t) error.error) {
                case LET_ERROR_STORAGE_WAL_CREATE_FAILED:
                    error_report.message = "The system failed to create a new log file";
                    break;
                case LET_ERROR_STORAGE_WAL_OPEN_FAILED:
                    error_report.message = "The system failed to open the log file";
                    break;
                case LET_ERROR_STORAGE_WAL_WRITE_FAILED:
                    error_report.message = "The system failed to write data to the log file";
                    break;
                case LET_ERROR_STORAGE_WAL_READ_FAILED:
                    error_report.message = "The system failed to read data from the log file";
                    break;
                case LET_ERROR_STORAGE_WAL_SYNC_FAILED:
                    error_report.message = "The system failed to sync the log data to disk";
                    break;
                case LET_ERROR_STORAGE_WAL_SEEK_FAILED:
                    error_report.message = "The system failed to reposition the log file pointer";
                    break;
                case LET_ERROR_STORAGE_WAL_INVALID_MAGIC:
                    error_report.message = "The log file contains an invalid magic number signature";
                    break;
                case LET_ERROR_STORAGE_WAL_INVALID_VERSION:
                    error_report.message = "The log file version is not supported by this engine";
                    break;
                case LET_ERROR_STORAGE_WAL_NONCE_MISMATCH:
                    error_report.action = LET_ERROR_ACTION_REJECT;
                    error_report.message = "The transaction nonce does not match the file metadata";
                    break;
                case LET_ERROR_STORAGE_WAL_CHECKSUM_MISMATCH:
                    error_report.message = "The log file data failed the checksum validation check";
                    break;
                case LET_ERROR_STORAGE_WAL_INVALID_ENTRY_TYPE:
                    error_report.message = "The log file contains an unknown tracking entry type";
                    break;
                case LET_ERROR_STORAGE_WAL_INVALID_TIME:
                    error_report.message = "The provided timestamp for storage subsystem is invalid";
                    break;
            }

            break;
        }
        case LET_ERROR_ID_CLI: {
            error_report.action = LET_ERROR_ACTION_FATAL;
            switch ((let_error_cli_t) error.error) {
                case LET_ERROR_CLI_INVALID_OPTION:
                    error_report.message = "The specified command line flag is unrecognized";
                    break;
                case LET_ERROR_CLI_INVALID_PORT:
                    error_report.message = "The specified network port number is out of range";
                    break;
                case LET_ERROR_CLI_INVALID_BACKLOG:
                    error_report.message = "The specified connection backlog size must be positive";
                    break;
                case LET_ERROR_CLI_INVALID_READ_TIMEOUT:
                    error_report.message = "The specified read timeout value must be positive";
                    break;
                case LET_ERROR_CLI_INVALID_WRITE_TIMEOUT:
                    error_report.message = "The specified write timeout value must be positive";
                    break;
            }

            break;
        }
    }

    return error_report;
}

void let_error_print(const let_error_report_t error_report) {
    if (error_report.action == LET_ERROR_ACTION_IGNORE) {
        return;
    }

    const auto header_info = localtime(&error_report.timestamp);
    if (header_info == nullptr) {
        return;
    }

    char header_buffer[LET_ERROR_HEADER_CAPACITY] = {};
    strftime(header_buffer, LET_ERROR_HEADER_CAPACITY, "[%Y-%m-%d %H:%M:%S]", header_info);

    switch (error_report.action) {
#ifndef NDEBUG
        case LET_ERROR_ACTION_REJECT:
            fprintf(stderr, "\033[0;33m%s [WARNING]: %s\033[0m\n", header_buffer, error_report.message);
            break;
#endif
        case LET_ERROR_ACTION_CLOSE:
            fprintf(stderr, "\033[0;31m%s [RECOVERABLE]: %s\033[0m\n", header_buffer, error_report.message);
            break;
        case LET_ERROR_ACTION_FATAL:
            fprintf(stderr, "\033[1;31m%s [CRITICAL]: %s\033[0m\n", header_buffer, error_report.message);
            break;
        default:
            break;
    }
}

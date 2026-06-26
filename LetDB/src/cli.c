/**
 * @file cli.c
 * @brief The command line interface (CLI) implementation.
 */

#include "let/cli.h"

#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// -----------------------------------------------------------------------------
// Data Structures
// -----------------------------------------------------------------------------

// The command line options for the application.
static const struct option let_cli_options[] = {
    {"help", no_argument, nullptr, 'h'},
    {"version", no_argument, nullptr, 'v'},
    {"port", required_argument, nullptr, 'p'},
    {"backlog", required_argument, nullptr, 'b'},
    {"log-level", required_argument, nullptr, 'l'},
    {"truncate-on-fail", no_argument, nullptr, 't'},
    {"file", required_argument, nullptr, 'f'},
    {"read-timeout", required_argument, nullptr, 0},
    {"write-timeout", required_argument, nullptr, 0},
    {nullptr, 0, nullptr, 0}
};

// -----------------------------------------------------------------------------
// Function Implementations
// -----------------------------------------------------------------------------

let_cli_t let_cli_empty(void) {
    return (let_cli_t){
        .storage_file = LET_CLI_DEFAULT_STORAGE_FILE,

        .read_timeout = LET_CLI_DEFAULT_READ_TIMEOUT,
        .write_timeout = LET_CLI_DEFAULT_WRITE_TIMEOUT,

        .port = LET_CLI_DEFAULT_PORT,
        .backlog = LET_CLI_DEFAULT_BACKLOG,

        .log_level = LET_CLI_DEFAULT_LOG_LEVEL,
        .truncate_on_fail = LET_CLI_DEFAULT_TRUNCATE_ON_FAIL,

        .help = false,
        .version = false,
    };
}

let_error_t let_cli_parse(let_cli_t *cli,
                          const int argc,
                          char **argv) {
    char *end;
    int option, index;

    // Parse the command line arguments.
    while ((option = getopt_long(argc, argv, "hvp:b:l:tf:", let_cli_options, &index)) != -1) {
        // Reset `errno` before each conversion to detect errors.
        errno = 0;

        // Defined to hold the converted value from the command line argument.
        let_i64_t value = 0;

        // Check which option was matched.
        switch (option) {
            case 0:
                if (strcmp(let_cli_options[index].name, "read-timeout") == 0) {
                    value = (let_i64_t) strtoul(optarg, &end, 10);
                    if (end == optarg || errno == ERANGE || value > LET_U32_MAX) {
                        return let_error_new(LET_ERROR_ID_CLI, LET_ERROR_CLI_INVALID_READ_TIMEOUT);
                    }

                    cli->read_timeout = (let_u32_t) value;
                } else if (strcmp(let_cli_options[index].name, "write-timeout") == 0) {
                    value = (let_i64_t) strtoul(optarg, &end, 10);
                    if (end == optarg || errno == ERANGE || value > LET_U32_MAX) {
                        return let_error_new(LET_ERROR_ID_CLI, LET_ERROR_CLI_INVALID_WRITE_TIMEOUT);
                    }

                    cli->write_timeout = (let_u32_t) value;
                }

                break;
            case 'h':
                cli->help = true;
                break;
            case 'v':
                cli->version = true;
                break;
            case 'p':
                value = (let_i64_t) strtoul(optarg, &end, 10);
                if (end == optarg || errno == ERANGE || value > LET_U16_MAX) {
                    return let_error_new(LET_ERROR_ID_CLI, LET_ERROR_CLI_INVALID_PORT);
                }

                cli->port = (let_u16_t) value;
                break;
            case 'b':
                value = (let_i64_t) strtoul(optarg, &end, 10);
                if (end == optarg || errno == ERANGE || value > LET_U16_MAX) {
                    return let_error_new(LET_ERROR_ID_CLI, LET_ERROR_CLI_INVALID_BACKLOG);
                }

                cli->backlog = (let_u16_t) value;
                break;
            case 'l':
                value = (let_i64_t) strtoul(optarg, &end, 10);
                if (end == optarg || errno == ERANGE || value > LET_LOG_LEVEL_NONE) {
                    return let_error_new(LET_ERROR_ID_CLI, LET_ERROR_CLI_INVALID_LOG_LEVEL);
                }

                cli->log_level = (let_log_level_t) value;
                break;
            case 't':
                cli->truncate_on_fail = true;
                break;
            case 'f':
                cli->storage_file = optarg;
                break;
            default:
                return let_error_new(LET_ERROR_ID_CLI, LET_ERROR_CLI_INVALID_OPTION);
        }
    }

    return let_error_none();
}

void let_cli_help(void) {
    puts("Usage: LetDB [OPTIONS]\n"
        "\n"
        "Options:\n"
        "-h, --help                 Show this help message and exit\n"
        "-v, --version              Show program version and exit\n"
        "-p, --port <PORT>          Specify the port number to listen on\n"
        "-b, --backlog <SIZE>       Set the connection backlog size\n"
        "-l, --log-level <LEVEL>    Set the log level\n"
        "-t, --truncate-on-fail     Truncate the storage on failure\n"
        "-f, --file <PATH>          Specify the file to use\n"
        "--read-timeout <TIME>      Set the read timeout in seconds\n"
        "--write-timeout <TIME>     Set the write timeout in seconds\n"
        "\n"
        "Examples:\n"
        "LetDB --port 8080 -b 4 --write-timeout 0\n"
        "LetDB --file debug.db -l 0 --read-timeout 60");
}

void let_cli_version(void) {
    puts(LET_VERSION); // Print the version defined inside CMake
}

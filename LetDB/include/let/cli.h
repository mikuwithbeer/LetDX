#ifndef LET_CLI_H
#define LET_CLI_H

/**
 * @file cli.h
 * @brief The command line interface (CLI) interface.
 *
 * This header defines the structures and functions for parsing command line arguments
 * and configuring the application.
 */

#include "let/error.h"
#include "let/log.h"

// -----------------------------------------------------------------------------
// Compile-Time Constants
// -----------------------------------------------------------------------------

constexpr char LET_CLI_DEFAULT_STORAGE_FILE[] = "LET__WAL"; // Default storage file name for the WAL

constexpr let_u32_t LET_CLI_DEFAULT_READ_TIMEOUT = 0; // Default read timeout in seconds (no timeout)
constexpr let_u32_t LET_CLI_DEFAULT_WRITE_TIMEOUT = 10; // Default write timeout in seconds

constexpr let_u16_t LET_CLI_DEFAULT_PORT = 55543; // Default network port for the server
constexpr let_u16_t LET_CLI_DEFAULT_BACKLOG = 1; // Default connection backlog size

constexpr let_log_level_t LET_CLI_DEFAULT_LOG_LEVEL = LET_LOG_LEVEL_INFO; // Default log level for logging output

// Default behavior for truncating the storage file on failure.
constexpr bool LET_CLI_DEFAULT_TRUNCATE_ON_FAIL = false;

// -----------------------------------------------------------------------------
// Data Structures
// -----------------------------------------------------------------------------

/**
 * @brief The command line interface (CLI) structure.
 *
 * This structure holds the configuration options parsed from the command line arguments.
 */
typedef struct {
    char *storage_file;

    let_u32_t read_timeout;
    let_u32_t write_timeout;

    let_u16_t port;
    let_u16_t backlog;

    let_log_level_t log_level;
    bool truncate_on_fail;

    bool help; // Flag to indicate if help message should be displayed
    bool version; // Flag to indicate if version information should be displayed
} let_cli_t;

// -----------------------------------------------------------------------------
// Function Declarations
// -----------------------------------------------------------------------------

/**
 * @brief Returns a default-initialized CLI struct.
 */
[[nodiscard]] let_cli_t let_cli_empty(void);

/**
 * @brief Parses the command line arguments and populates the CLI struct.
 */
let_error_t let_cli_parse(let_cli_t *cli,
                          int argc,
                          char **argv);

/**
 * @brief Prints the help message to stdout.
 */
void let_cli_help(void);

/**
 * @brief Prints the version information to stdout.
 */
void let_cli_version(void);

#endif //LET_CLI_H

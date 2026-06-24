#ifndef LET_LOG_H
#define LET_LOG_H

/**
 * @file log.h
 * @brief The logging interface.
 *
 * This header defines the structures and functions for logging messages with different severity levels.
 */

#include "let/common.h"

// -----------------------------------------------------------------------------
// Compile-Time Constants
// -----------------------------------------------------------------------------

constexpr auto LET_LOG_HEADER_CAPACITY = 1 << 5; // Log message header capacity

// -----------------------------------------------------------------------------
// Data Structures
// -----------------------------------------------------------------------------

/**
 * @brief The log levels for controlling the verbosity of log messages.
 */
typedef enum : let_u8_t {
    LET_LOG_LEVEL_DEBUG,
    LET_LOG_LEVEL_INFO,
    LET_LOG_LEVEL_WARNING,
    LET_LOG_LEVEL_ERROR,
    LET_LOG_LEVEL_FATAL,
    LET_LOG_LEVEL_NONE, // Used to disable logging entirely
} let_log_level_t;

/**
 * @brief The global log level that controls which messages are printed.
 *
 * @note Messages below this level will be ignored.
 */
extern let_log_level_t let_log_level;

// -----------------------------------------------------------------------------
// Function Declarations
// -----------------------------------------------------------------------------

/**
 * @brief Sets the global log level.
 */
void let_log_level_set(let_log_level_t log_level);

/**
 * @brief Prints a formatted log message.
 */
[[gnu::format(printf, 2, 3)]] void let_log_print(let_log_level_t log_level,
                                                 const char *format, ...);

#endif //LET_LOG_H

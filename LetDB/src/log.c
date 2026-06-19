#include "let/log.h"

#include <stdarg.h>
#include <stdio.h>

let_log_level_t let_log_level = LET_LOG_LEVEL_DEBUG;

static const char *let_log_level_attributes[] = {
    "DEBUG",
    "INFO",
    "WARNING",
    "ERROR",
    "FATAL",
    ""
};

static const char *let_log_level_colors[] = {
    "\033[1;36m",
    "\033[0;34m",
    "\033[0;33m",
    "\033[0;31m",
    "\033[1;31m",
    "\033[0m"
};

void let_log_level_set(const let_log_level_t log_level) {
    let_log_level = log_level;
}

void let_log_print(const let_log_level_t log_level,
                   const char *format, ...) {
    if (log_level < let_log_level) {
        return;
    }

    const auto time_now = time(nullptr);
    if (time_now == -1) {
        return;
    }

    const auto time_info = localtime(&time_now);
    if (time_info == nullptr) {
        return;
    }

    char header_buffer[LET_LOG_HEADER_CAPACITY] = {};
    strftime(header_buffer, LET_LOG_HEADER_CAPACITY, "%Y-%m-%d %H:%M:%S", time_info);

    fprintf(stderr,
            "%s[%s] (%s): ",
            let_log_level_colors[log_level],
            header_buffer,
            let_log_level_attributes[log_level]);

    va_list args = {};
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fprintf(stderr, "%s\n", let_log_level_colors[LET_LOG_LEVEL_NONE]);
}

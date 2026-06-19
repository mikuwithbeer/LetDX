#ifndef LET_LOG_H
#define LET_LOG_H

#include "let/common.h"

constexpr auto LET_LOG_HEADER_CAPACITY = 1 << 5;

typedef enum : let_u8_t {
    LET_LOG_LEVEL_DEBUG,
    LET_LOG_LEVEL_INFO,
    LET_LOG_LEVEL_WARNING,
    LET_LOG_LEVEL_ERROR,
    LET_LOG_LEVEL_FATAL,
    LET_LOG_LEVEL_NONE,
} let_log_level_t;

extern let_log_level_t let_log_level;

void let_log_level_set(let_log_level_t log_level);

void let_log_print(let_log_level_t log_level,
                   const char *message);

#endif //LET_LOG_H

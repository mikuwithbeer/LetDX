#ifndef LET_CLI_H
#define LET_CLI_H

#include "let/error.h"
#include "let/log.h"

constexpr char LET_CLI_DEFAULT_STORAGE_FILE[] = "LET__WAL";

constexpr let_u32_t LET_CLI_DEFAULT_READ_TIMEOUT = 0;
constexpr let_u32_t LET_CLI_DEFAULT_WRITE_TIMEOUT = 10;

constexpr let_u16_t LET_CLI_DEFAULT_PORT = 55543;
constexpr let_u16_t LET_CLI_DEFAULT_BACKLOG = 1 << 2;

constexpr let_log_level_t LET_CLI_DEFAULT_LOG_LEVEL = LET_LOG_LEVEL_INFO;
constexpr bool LET_CLI_DEFAULT_TRUNCATE_ON_FAIL = false;

typedef struct {
    char *storage_file;

    let_u32_t read_timeout;
    let_u32_t write_timeout;

    let_u16_t port;
    let_u16_t backlog;

    let_log_level_t log_level;
    bool truncate_on_fail;

    bool help;
    bool version;
} let_cli_t;

[[nodiscard]] let_cli_t let_cli_empty(void);

let_error_t let_cli_parse(let_cli_t *cli,
                          int argc,
                          char **argv);

void let_cli_help(void);

void let_cli_version(void);

#endif //LET_CLI_H

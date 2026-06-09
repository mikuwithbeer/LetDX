#ifndef LET_CLI_H
#define LET_CLI_H

#include "common.h"
#include "error.h"

constexpr char LET_CLI_DEFAULT_STORAGE_FILE[] = "LET__WAL";

constexpr let_u16_t LET_CLI_DEFAULT_PORT = 55543;
constexpr let_u16_t LET_CLI_DEFAULT_BACKLOG = 1 << 3;

typedef struct {
    char *storage_file;

    let_u16_t port;
    let_u16_t backlog;

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

#ifndef LET_H
#define LET_H

#include "let/network/server.h"
#include "let/storage/wal.h"
#include "let/guard.h"
#include "let/cli.h"

#include <signal.h>

typedef struct {
    volatile sig_atomic_t running;
    let_error_t error;

    let_account_list_t *account_list;
    let_state_t state;
    let_guard_t guard;

    let_network_server_t network_server;
    let_network_server_t network_client;

    let_storage_wal_t storage_wal;
} let_t;

extern let_t let;

void let_init(const let_cli_t *cli);

void let_run(const let_cli_t *cli);

void let_close(int signal);

void let_cleanup(void);

#endif //LET_H

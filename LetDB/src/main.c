#define _POSIX_C_SOURCE 200809L

#include "let/let.h"
#include "let/cli.h"

#include <signal.h>
#include <stdio.h>

static void apply_signal(void);

static bool apply_process(int argc,
                          char **argv);

int main(const int argc,
         char **argv) {
    apply_signal();

    const auto success = apply_process(argc, argv);
    if (!success) {
        const auto report = let_error_report(let.error);
        fprintf(stderr, "%s\n", report.message);
    }

    return success ? 0 : 1;
}

static void apply_signal(void) {
    struct sigaction signal_action = {};
    sigemptyset(&signal_action.sa_mask);

    signal_action.sa_handler = let_close;
    signal_action.sa_flags = 0;

    sigaction(SIGTERM, &signal_action, nullptr);
    sigaction(SIGINT, &signal_action, nullptr);

    struct sigaction pipe_action = {};
    sigemptyset(&pipe_action.sa_mask);

    pipe_action.sa_handler = SIG_IGN;

    sigaction(SIGPIPE, &pipe_action, nullptr);
}

static bool apply_process(const int argc,
                          char **argv) {
    auto let_cli = let_cli_empty();
    let.error = let_cli_parse(&let_cli, argc, argv);
    if (let_error_exists(let.error)) {
        return false;
    }

    if (let_cli.help) {
        let_cli_help();
        return true;
    }

    if (let_cli.version) {
        let_cli_version();
        return true;
    }

    let_init(&let_cli);
    if (let_error_exists(let.error)) {
        goto cleanup;
    }

    let_run(&let_cli);

cleanup:
    let_cleanup();
    return !let_error_exists(let.error);
}

#include "let/let.h"
#include "let/cli.h"

#include <stdio.h>
#include <stdlib.h>

int main(const int argc,
         char **argv) {
    auto success = EXIT_SUCCESS;

    struct sigaction signal_action = {};
    sigemptyset(&signal_action.sa_mask);

    signal_action.sa_handler = let_close;
    signal_action.sa_flags = 0;

    sigaction(SIGTERM, &signal_action, nullptr);
    sigaction(SIGINT, &signal_action, nullptr);

    auto cli = let_cli_empty();
    const auto cli_result = let_cli_parse(&cli, argc, argv);

    if (let_error_exists(cli_result)) {
        const auto error_report = let_error_report(cli_result);
        puts(error_report.message);

        success = EXIT_FAILURE;
        goto exit;
    }

    if (cli.help) {
        let_cli_help();
        goto exit;
    }

    if (cli.version) {
        let_cli_version();
        goto exit;
    }

    let_init(&cli);
    if (let_error_exists(let.error)) {
        const auto error_report = let_error_report(let.error);
        puts(error_report.message);

        success = EXIT_FAILURE;
        goto cleanup;
    }

    let_run(&cli);
    if (let_error_exists(let.error)) {
        const auto error_report = let_error_report(let.error);
        puts(error_report.message);

        success = EXIT_FAILURE;
        goto cleanup;
    }

cleanup:
    let_cleanup();
    if (let_error_exists(let.error)) {
        const auto error_report = let_error_report(let.error);
        puts(error_report.message);

        success = EXIT_FAILURE;
    }

exit:
    return success;
}

#include "let/let.h"
#include "let/cli.h"

int main(const int argc,
         char **argv) {
    auto success = EXIT_SUCCESS;
    char message[64] = {};

    struct sigaction signal_action = {};
    signal_action.sa_handler = let_close;

    sigaction(SIGTERM, &signal_action, nullptr);
    sigaction(SIGINT, &signal_action, nullptr);

    auto cli = let_cli_empty();
    const auto cli_result = let_cli_parse(&cli, argc, argv);

    if (let_error_exists(cli_result)) {
        let_error_message(cli_result, message);
        printf("[cli] %s\n", message);

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
        let_error_message(let.error, message);
        printf("[init] %s\n", message);

        success = EXIT_FAILURE;
        goto cleanup;
    }

    let_run();
    if (let_error_exists(let.error)) {
        let_error_message(let.error, message);
        printf("[run] %s\n", message);

        success = EXIT_FAILURE;
        goto cleanup;
    }

cleanup:
    let_cleanup();
    if (let_error_exists(let.error)) {
        let_error_message(let.error, message);
        printf("[cleanup] %s\n", message);

        success = EXIT_FAILURE;
    }

exit:
    return success;
}

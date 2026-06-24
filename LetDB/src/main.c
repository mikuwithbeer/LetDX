/**
 * @file main.c
 * @brief The entry point for the application.
 */

#include "let/let.h"

// -----------------------------------------------------------------------------
// Compile-Time Constants
// -----------------------------------------------------------------------------

constexpr auto EXIT_SUCCESS = 0;
constexpr auto EXIT_FAILURE = 1;

// -----------------------------------------------------------------------------
// Forward Declarations
// -----------------------------------------------------------------------------

static void apply_signal(void);

[[nodiscard]] static bool apply_process(int argc,
                                        char **argv);

// -----------------------------------------------------------------------------
// Main Function
// -----------------------------------------------------------------------------

int main(const int argc,
         char **argv) {
    apply_signal(); // Set up signal handlers
    return apply_process(argc, argv) ? EXIT_SUCCESS : EXIT_FAILURE; // Run the process
}

// -----------------------------------------------------------------------------
// Internal Functions
// -----------------------------------------------------------------------------

static void apply_signal(void) {
    struct sigaction signal_action = {};
    sigemptyset(&signal_action.sa_mask);
    signal_action.sa_handler = let_close;

    sigaction(SIGTERM, &signal_action, nullptr); // Handle termination signal (e.g., from `kill` command)
    sigaction(SIGINT, &signal_action, nullptr); // Handle interrupt signal (e.g., from Ctrl+C)

    // Writing to a socket whose peer has closed the connection may generate SIGPIPE.
    // The default action for SIGPIPE is process termination.
    // Ignoring SIGPIPE allows the network layer to handle the disconnect gracefully.
    struct sigaction pipe_action = {};
    sigemptyset(&pipe_action.sa_mask);
    pipe_action.sa_handler = SIG_IGN;

    sigaction(SIGPIPE, &pipe_action, nullptr);
}

static bool apply_process(const int argc,
                          char **argv) {
    // Parse the command line arguments into a CLI struct.
    auto let_cli = let_cli_empty();
    let.error = let_cli_parse(&let_cli, argc, argv);

    if (let_error_exists(let.error)) {
        return false;
    }

    // Handle help and version flags before proceeding with initialization.
    if (let_cli.help) {
        let_cli_help();
        return true;
    }

    if (let_cli.version) {
        let_cli_version();
        return true;
    }

    // Set the log level based on the CLI configuration.
    let_log_level_set(let_cli.log_level);

    // Initialize the singleton with the parsed CLI options.
    let_init(&let_cli);
    if (!let_error_exists(let.error)) {
        // Run the main loop to handle incoming requests.
        let_run(&let_cli);
    }

    // Perform cleanup of resources before exiting.
    let_cleanup();

    return !let_error_exists(let.error); // Return true if no errors occurred, false otherwise
}

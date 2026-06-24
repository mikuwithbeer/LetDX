#ifndef LET_H
#define LET_H

/**
 * @file let.h
 * @brief The monolithic singleton interface.
 *
 * This header defines the core application context and its associated functions.
 * It provides a unified structure for managing the state, network server, storage engine,
 * and other components of the application.
 */

#include "let/network/server.h"
#include "let/storage/wal.h"
#include "let/guard.h"
#include "let/cli.h"

#include <signal.h>

// -----------------------------------------------------------------------------
// Data Structures
// -----------------------------------------------------------------------------

/**
 * @brief The monolithic singleton structure that encapsulates the entire application context.
 */
typedef struct {
    // Marked `volatile sig_atomic_t` because these flags are mutated
    // asynchronously by OS signal handlers (e.g., SIGINT/SIGTERM).
    // Compiler must not cache these values in CPU registers.
    volatile sig_atomic_t accepting; // True if the network server is accepting new connections
    volatile sig_atomic_t executing; // True if the main processing loop is executing

    let_account_list_t *account_list;
    let_state_t state;
    let_guard_t guard;

    let_network_server_t network_server;
    let_network_server_t network_client;

    let_storage_wal_t storage_wal;

    let_error_t error; // The last error that occurred in the runtime context
} let_t;

/**
 * @brief The global singleton instance of the application context.
 */
extern let_t let;

// -----------------------------------------------------------------------------
// Function Declarations
// -----------------------------------------------------------------------------

/**
 * @brief Initializes the application context.
 */
void let_init(const let_cli_t *cli);

/**
 * @brief Runs the main application loop, handling incoming network requests and processing them.
 */
void let_run(const let_cli_t *cli);

/**
 * @brief Signal handler for graceful shutdown of the application.
 */
void let_close(int signal);

/**
 * @brief Cleans up resources and releases memory before exiting the application.
 */
void let_cleanup(void);

#endif //LET_H

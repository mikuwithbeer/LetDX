#ifndef LET_GUARD_H
#define LET_GUARD_H

/**
 * @file guard.h
 * @brief The guard interface.
 *
 * This header defines the structures and functions for the guard module,
 * which is responsible for validating operations on accounts and ensuring that
 * they adhere to the defined rules and constraints.
 */

#include "let/state.h"

// -----------------------------------------------------------------------------
// Data Structures
// -----------------------------------------------------------------------------

/**
 * @brief The guard structure.
 */
typedef struct {
    let_state_t *state; // Weak pointer to the state machine
} let_guard_t;

// -----------------------------------------------------------------------------
// Function Declarations
// -----------------------------------------------------------------------------

/**
 * @brief Returns an empty, zero-initialized guard struct.
 */
[[nodiscard]] let_guard_t let_guard_empty(void);

/**
 * @brief Initializes the guard with a given state machine.
 */
let_error_t let_guard_init(let_guard_t *guard,
                           let_state_t *state);

/**
 * @brief Validates a transfer request between two accounts.
 */
let_error_t let_guard_make_transfer(const let_guard_t *guard,
                                    let_u64_t from_account_id,
                                    let_u64_t to_account_id,
                                    let_u128_t amount);

/**
 * @brief Validates an account update request.
 */
let_error_t let_guard_update_account(const let_guard_t *guard,
                                     let_u64_t account_id);

#endif //LET_GUARD_H

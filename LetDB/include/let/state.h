#ifndef LET_STATE_H
#define LET_STATE_H

/**
 * @file state.h
 * @brief The state machine interface.
 *
 * The state machine is responsible for maintaining the current state of accounts and transactions.
 */

#include "let/account.h"

// -----------------------------------------------------------------------------
// Data Structures
// -----------------------------------------------------------------------------

/**
 * @brief The state machine structure.
 *
 * @warning The caller is responsible for ensuring that the `account_list` safely outlives this state machine.
 */
typedef struct {
    let_account_list_t *account_list;
} let_state_t;

// -----------------------------------------------------------------------------
// Function Declarations
// -----------------------------------------------------------------------------

/**
 * @brief Returns an empty, zero-initialized state machine struct.
 */
[[nodiscard]] let_state_t let_state_empty(void);


/**
 * @brief Initializes the state machine with a given account list.
 */
let_error_t let_state_init(let_state_t *state,
                           let_account_list_t *account_list);

/**
 * @brief Adds a new account to the state machine.
 */
let_error_t let_state_add_account(const let_state_t *state,
                                  let_account_t account,
                                  let_u64_t *account_id);

/**
 * @brief Makes a transfer between two accounts in the state machine.
 */
let_error_t let_state_make_transfer(const let_state_t *state,
                                    let_u64_t from_account_id,
                                    let_u64_t to_account_id,
                                    let_u128_t amount);

/**
 * @brief Updates an existing account in the state machine.
 */
let_error_t let_state_update_account(const let_state_t *state,
                                     let_u64_t account_id,
                                     let_account_flag_t flags);

#endif //LET_STATE_H

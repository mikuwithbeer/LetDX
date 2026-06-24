#ifndef LET_ACCOUNT_H
#define LET_ACCOUNT_H

/**
 * @file account.h
 * @brief The account interface.
 *
 * This header defines the structures and functions for managing accounts in the system.
 */

#include "let/common.h"
#include "let/error.h"

// -----------------------------------------------------------------------------
// Compile-Time Constants
// -----------------------------------------------------------------------------

constexpr let_u64_t LET_ACCOUNT_CAPACITY_DEFAULT = 1 << 10; // Default capacity for the account list

// -----------------------------------------------------------------------------
// Data Structures
// -----------------------------------------------------------------------------

/**
 * @brief Flags that define the capabilities and properties of an account.
 */
typedef enum : let_u8_t {
    LET_ACCOUNT_FLAG_NONE = 0, // No special capabilities
    LET_ACCOUNT_FLAG_CAN_SEND = 1 << 0, // Account can send funds
    LET_ACCOUNT_FLAG_CAN_RECEIVE = 1 << 1, // Account can receive funds
    LET_ACCOUNT_FLAG_CAN_DEBT = 1 << 2, // Account can have a negative balance (debt)
} let_account_flag_t;

/**
 * @brief Represents an account in the system, including its financial state and metadata.
 */
typedef struct {
    let_u128_t credits;
    let_u128_t debits;

    let_u64_t transactions;
    let_time_t created_at;
    let_time_t updated_at;

    let_account_flag_t flags;
} let_account_t;

/**
 * @brief A dynamic list of accounts, managing the collection of account structures and their metadata.
 */
typedef struct {
    let_account_t *accounts; // Pointer to an array of accounts

    let_u64_t capacity;
    let_u64_t length;
} let_account_list_t;

// -----------------------------------------------------------------------------
// Function Declarations
// -----------------------------------------------------------------------------

/**
 * @brief Creates a new account with the specified parameters.
 */
[[nodiscard]] let_account_t let_account_new(let_u128_t credits,
                                            let_u128_t debits,
                                            let_time_t created_at,
                                            let_account_flag_t flags);

/**
 * @brief Creates a new account list with default capacity.
 *
 * @return A pointer to the newly created account list, or nullptr if memory allocation fails.
 */
[[nodiscard]] let_account_list_t *let_account_list_new(void);

/**
 * @brief Adds an account to the account list.
 *
 * @note If the account list is full, it will attempt to resize the list.
 */
let_error_t let_account_list_add(let_account_list_t *account_list,
                                 let_account_t account);

/**
 * @brief Retrieves an account from the account list by its ID.
 */
let_error_t let_account_list_get(const let_account_list_t *account_list,
                                 let_u64_t account_id,
                                 let_account_t *account);

/**
 * @brief Updates an account in the account list by its ID.
 */
let_error_t let_account_list_update(const let_account_list_t *account_list,
                                    let_u64_t account_id,
                                    let_account_flag_t flags);

/**
 * @brief Frees the memory allocated for the account list and its accounts.
 */
void let_account_list_free(let_account_list_t *account_list);

#endif //LET_ACCOUNT_H

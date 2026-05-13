#include "guard.h"

#include <stdlib.h>

let_guard_t *let_guard_new(let_state_t *state) {
    let_guard_t *guard = malloc(sizeof(let_guard_t));
    if (guard == nullptr) {
        return nullptr;
    }

    guard->state = state;
    return guard;
}

let_guard_error_t let_guard_make_transfer(const let_guard_t *guard,
                                          const let_u64_t from_account_id,
                                          const let_u64_t to_account_id,
                                          const let_u128_t amount) {
    if (from_account_id == to_account_id) {
        return LET_GUARD_ERROR_SAME_ACCOUNT;
    }

    const auto account_count = guard->state->account_list->length;
    if (from_account_id >= account_count || to_account_id >= account_count) {
        return LET_GUARD_ERROR_ACCOUNT_NOT_FOUND;
    }

    const auto from_account = &guard->state->account_list->accounts[from_account_id];
    const auto to_account = &guard->state->account_list->accounts[to_account_id];

    const auto from_account_balance = from_account->debits - from_account->credits;
    if (amount > from_account_balance) {
        return LET_GUARD_ERROR_INSUFFICIENT_FUNDS;
    }

    if (from_account->transactions == LET_ACCOUNT_MAXIMUM_TRANSACTIONS
        || to_account->transactions == LET_ACCOUNT_MAXIMUM_TRANSACTIONS) {
        return LET_GUARD_ERROR_MAXIMUM_TRANSACTIONS;
    }

    return LET_GUARD_ERROR_NONE;
}

void let_guard_free(let_guard_t *guard) {
    if (guard != nullptr) {
        free(guard);
    }
}

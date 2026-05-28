#include "let/guard.h"

#include <stdlib.h>

let_guard_t *let_guard_new(let_state_t *state) {
    let_guard_t *guard = malloc(sizeof(let_guard_t));
    if (guard == nullptr) {
        return nullptr;
    }

    guard->state = state;
    return guard;
}

let_error_t let_guard_make_transfer(const let_guard_t *guard,
                                    const let_u64_t from_account_id,
                                    const let_u64_t to_account_id,
                                    const let_u128_t amount) {
    if (amount == 0) {
        return let_error_new(LET_ERROR_ID_GUARD, LET_ERROR_GUARD_ZERO_BALANCE);
    }

    if (from_account_id == to_account_id) {
        return let_error_new(LET_ERROR_ID_GUARD, LET_ERROR_GUARD_SAME_ACCOUNT);
    }

    const auto account_count = guard->state->account_list->length;
    if (from_account_id >= account_count || to_account_id >= account_count) {
        return let_error_new(LET_ERROR_ID_GUARD, LET_ERROR_ACCOUNT_NOT_FOUND);
    }

    const auto from_account = &guard->state->account_list->accounts[from_account_id];
    const auto to_account = &guard->state->account_list->accounts[to_account_id];

    if (from_account->debits < from_account->credits ||
        from_account->debits - from_account->credits < amount) {
        return let_error_new(LET_ERROR_ID_GUARD, LET_ERROR_GUARD_INSUFFICIENT_BALANCE);
    }

    if (from_account->transactions == LET_U64_MAX
        || to_account->transactions == LET_U64_MAX) {
        return let_error_new(LET_ERROR_ID_GUARD, LET_ERROR_GUARD_TRANSACTION_OVERFLOW);
    }

    return let_error_none();
}

void let_guard_free(let_guard_t *guard) {
    if (guard != nullptr) {
        free(guard);
    }
}

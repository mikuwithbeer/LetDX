#include "let/guard.h"

let_guard_t let_guard_empty(void) {
    return (let_guard_t){};
}

let_error_t let_guard_init(let_guard_t *guard,
                           let_state_t *state) {
    if (state == nullptr) {
        return let_error_new(LET_ERROR_ID_GUARD, LET_ERROR_GUARD_INVALID_STATE);
    }

    guard->state = state;
    return let_error_none();
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

    if (from_account_id >= account_count) {
        return let_error_new(LET_ERROR_ID_GUARD, LET_ERROR_GUARD_SENDER_NOT_FOUND);
    }

    if (to_account_id >= account_count) {
        return let_error_new(LET_ERROR_ID_GUARD, LET_ERROR_GUARD_RECIPIENT_NOT_FOUND);
    }

    const auto from_account = &guard->state->account_list->accounts[from_account_id];
    const auto to_account = &guard->state->account_list->accounts[to_account_id];

    if ((from_account->flags & LET_ACCOUNT_FLAG_CAN_SEND) == 0) {
        return let_error_new(LET_ERROR_ID_GUARD, LET_ERROR_GUARD_ACCOUNT_CANNOT_SEND);
    }

    if ((to_account->flags & LET_ACCOUNT_FLAG_CAN_RECEIVE) == 0) {
        return let_error_new(LET_ERROR_ID_GUARD, LET_ERROR_GUARD_ACCOUNT_CANNOT_RECEIVE);
    }

    if ((from_account->flags & LET_ACCOUNT_FLAG_CAN_DEBT) == 0) {
        if (LET_U128_MAX - from_account->credits < amount) {
            return let_error_new(LET_ERROR_ID_GUARD, LET_ERROR_GUARD_INSUFFICIENT_BALANCE);
        }

        if (from_account->debits < amount + from_account->credits) {
            return let_error_new(LET_ERROR_ID_GUARD, LET_ERROR_GUARD_INSUFFICIENT_BALANCE);
        }
    }

    if (from_account->transactions == LET_U64_MAX || to_account->transactions == LET_U64_MAX) {
        return let_error_new(LET_ERROR_ID_GUARD, LET_ERROR_GUARD_TRANSACTION_OVERFLOW);
    }

    return let_error_none();
}

let_error_t let_guard_update_account(const let_guard_t *guard,
                                     const let_u64_t account_id) {
    const auto account_count = guard->state->account_list->length;
    if (account_id >= account_count) {
        return let_error_new(LET_ERROR_ID_ACCOUNT, LET_ERROR_ACCOUNT_NOT_FOUND);
    }

    return let_error_none();
}

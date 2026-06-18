#include "let/state.h"

let_state_t let_state_empty(void) {
    return (let_state_t){};
}

let_error_t let_state_init(let_state_t *state,
                           let_account_list_t *account_list) {
    if (account_list == nullptr) {
        return let_error_new(LET_ERROR_ID_STATE, LET_ERROR_STATE_INVALID_ACCOUNT_LIST);
    }

    state->account_list = account_list;
    return let_error_none();
}

let_error_t let_state_add_account(const let_state_t *state,
                                  const let_account_t account,
                                  let_u64_t *account_id) {
    const auto current_length = state->account_list->length;
    const auto account_result = let_account_list_add(state->account_list, account);

    if (let_error_exists(account_result)) {
        return account_result;
    }

    *account_id = current_length;
    return let_error_none();
}

let_error_t let_state_make_transfer(const let_state_t *state,
                                    const let_u64_t from_account_id,
                                    const let_u64_t to_account_id,
                                    const let_u128_t amount) {
    const auto time_now = time(nullptr);
    if (time_now == -1) {
        return let_error_new(LET_ERROR_ID_STATE, LET_ERROR_STATE_INVALID_TIME);
    }

    const auto from_account = &state->account_list->accounts[from_account_id];
    const auto to_account = &state->account_list->accounts[to_account_id];

    from_account->credits += amount;
    to_account->debits += amount;

    from_account->transactions++;
    to_account->transactions++;

    from_account->updated_at = time_now;
    to_account->updated_at = time_now;

    return let_error_none();
}

let_error_t let_state_update_account(const let_state_t *state,
                                     const let_u64_t account_id,
                                     const let_account_flag_t flags) {
    return let_account_list_update(state->account_list, account_id, flags);
}

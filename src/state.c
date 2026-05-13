#include "state.h"

#include <stdlib.h>
#include <stddef.h>
#include <time.h>

let_state_t *let_state_new(let_account_list_t *account_list) {
    let_state_t *state = malloc(sizeof(let_state_t));
    if (state == nullptr) {
        return nullptr;
    }

    state->account_list = account_list;
    return state;
}

let_state_error_t let_state_add_account(const let_state_t *state,
                                        const let_account_t account,
                                        let_u64_t *account_id) {
    const auto current_length = state->account_list->length;
    switch (let_account_list_add(state->account_list, account)) {
        case LET_ACCOUNT_ERROR_NONE:
            *account_id = current_length;
            return LET_STATE_ERROR_NONE;
        case LET_ACCOUNT_ERROR_OUT_OF_MEMORY:
            return LET_STATE_ERROR_OUT_OF_MEMORY;
        default:
            unreachable();
    }
}

[[nodiscard]] let_state_error_t let_state_make_transfer(const let_state_t *state,
                                                        const let_u64_t from_account_id,
                                                        const let_u64_t to_account_id,
                                                        const let_u128_t amount) {
    const auto from_account = &state->account_list->accounts[from_account_id];
    const auto to_account = &state->account_list->accounts[to_account_id];

    from_account->credits += amount;
    to_account->debits += amount;

    from_account->transactions++;
    to_account->transactions++;

    const auto now = (let_u64_t) time(nullptr);

    from_account->updated_at = now;
    to_account->updated_at = now;

    return LET_STATE_ERROR_NONE;
}

void let_state_free(let_state_t *state) {
    if (state != nullptr) {
        free(state);
    }
}

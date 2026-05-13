#include "state.h"

#include <stdlib.h>
#include <stddef.h>

let_state_t *let_state_new(void) {
    let_state_t *state = malloc(sizeof(let_state_t));
    if (state == nullptr) {
        return nullptr;
    }

    state->account_list = let_account_list_new();
    if (state->account_list == nullptr) {
        free(state);
        return nullptr;
    }

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

void let_state_free(let_state_t *state) {
    if (state->account_list != nullptr) {
        let_account_list_free(state->account_list);
    }

    if (state != nullptr) {
        free(state);
    }
}

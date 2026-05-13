#include "state.h"

#include <stdio.h>
#include <time.h>

int main(void) {
    const auto state = let_state_new();
    if (state == nullptr) {
        return -1;
    }

    const let_account_t add_account = {
        .credits = 1000,
        .debits = 31,

        .created_at = time(nullptr),
        .updated_at = time(nullptr),

        .transactions = 0,
        .flags = LET_ACCOUNT_FLAG_CAN_SEND | LET_ACCOUNT_FLAG_CAN_RECEIVE
    };

    let_u64_t account_id;
    const auto add_account_result = let_state_add_account(state, add_account, &account_id);

    if (add_account_result != LET_STATE_ERROR_NONE) {
        printf("Failed to add account: %d\n", add_account_result);
        return -1;
    }

    printf("Account ID: %llu\n", account_id);

    let_state_free(state);
    return 0;
}

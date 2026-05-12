#include "account.h"

#include <stdio.h>
#include <time.h>

int main(void) {
    let_account_list_t *account_list = let_account_list_new();
    if (account_list == nullptr) {
        return -1;
    }

    const let_account_t add_account = {
        .credits = 1000,
        .debits = 31,

        .created_at = time(nullptr),
        .updated_at = time(nullptr),

        .transactions = 0,
        .flags = JET_ACCOUNT_FLAG_CAN_SEND | JET_ACCOUNT_FLAG_CAN_RECEIVE
    };

    if (!let_account_list_add(account_list, add_account)) {
        puts("Add account failed");
        return -1;
    }

    let_account_t get_account;
    if (!let_account_list_get(account_list,4, &get_account)) {
        puts("Get account failed on purpose");
    }

    if (!let_account_list_get(account_list, 0, &get_account)) {
        puts("Get account failed");
        return -1;
    }

    printf("Add Credits: %llu\n", (let_u64_t) add_account.credits);
    printf("Add Debits: %llu\n", (let_u64_t) add_account.debits);

    printf("Get Credits: %llu\n", (let_u64_t) get_account.credits);
    printf("Get Debits: %llu\n", (let_u64_t) get_account.debits);

    let_account_list_free(account_list);
    return 0;
}

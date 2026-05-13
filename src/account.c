#include "account.h"

#include <stdlib.h>

let_account_list_t *let_account_list_new(void) {
    let_account_list_t *account_list = malloc(sizeof(let_account_list_t));
    if (account_list == nullptr) {
        return nullptr;
    }

    account_list->capacity = LET_ACCOUNT_CAPACITY_DEFAULT;
    account_list->length = 0;

    let_account_t *accounts = malloc(account_list->capacity * sizeof(let_account_t));
    if (accounts == nullptr) {
        free(account_list);
        return nullptr;
    }

    account_list->accounts = accounts;
    return account_list;
}

let_account_error_t let_account_list_add(let_account_list_t *account_list,
                                         const let_account_t account) {
    if (account_list->length >= account_list->capacity) {
        const auto new_capacity = account_list->capacity * 2;
        let_account_t *accounts = realloc(account_list->accounts, sizeof(let_account_t) * new_capacity);
        if (accounts == nullptr) {
            return LET_ACCOUNT_ERROR_OUT_OF_MEMORY;
        }

        account_list->capacity = new_capacity;
        account_list->accounts = accounts;
    }

    account_list->accounts[account_list->length++] = account;
    return LET_ACCOUNT_ERROR_NONE;
}

let_account_error_t let_account_list_get(const let_account_list_t *account_list,
                                         const let_u64_t account_id,
                                         let_account_t *account) {
    if (account_id >= account_list->length) {
        return LET_ACCOUNT_ERROR_NOT_FOUND;
    }

    *account = account_list->accounts[account_id];
    return LET_ACCOUNT_ERROR_NONE;
}

void let_account_list_free(let_account_list_t *account_list) {
    if (account_list->accounts != nullptr) {
        free(account_list->accounts);
    }

    if (account_list != nullptr) {
        free(account_list);
    }
}

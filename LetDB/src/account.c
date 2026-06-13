#include "let/account.h"

#include <stdlib.h>
#include <stdckdint.h>

let_account_t let_account_new(const let_u128_t credits,
                              const let_u128_t debits,
                              const let_time_t created_at,
                              const let_account_flag_t flags) {
    return (let_account_t){
        .credits = credits,
        .debits = debits,

        .created_at = created_at,
        .updated_at = created_at,

        .transactions = 0,
        .flags = flags
    };
}

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

let_error_t let_account_list_add(let_account_list_t *account_list,
                                 const let_account_t account) {
    if (account_list->length >= account_list->capacity) {
        let_u64_t new_capacity;
        if (ckd_mul(&new_capacity, account_list->capacity, 2)) {
            return let_error_new(LET_ERROR_ID_ACCOUNT, LET_ERROR_ACCOUNT_CAPACITY_OVERFLOW);
        }

        let_account_t *accounts = realloc(account_list->accounts, sizeof(let_account_t) * new_capacity);
        if (accounts == nullptr) {
            return let_error_new(LET_ERROR_ID_ACCOUNT, LET_ERROR_ACCOUNT_OUT_OF_MEMORY);
        }

        account_list->capacity = new_capacity;
        account_list->accounts = accounts;
    }

    account_list->accounts[account_list->length++] = account;
    return let_error_none();
}

let_error_t let_account_list_get(const let_account_list_t *account_list,
                                 const let_u64_t account_id,
                                 let_account_t *account) {
    if (account_id >= account_list->length) {
        return let_error_new(LET_ERROR_ID_ACCOUNT, LET_ERROR_ACCOUNT_NOT_FOUND);
    }

    *account = account_list->accounts[account_id];
    return let_error_none();
}

let_error_t let_account_list_update(const let_account_list_t *account_list,
                                    const let_u64_t account_id,
                                    const let_account_flag_t flags) {
    account_list->accounts[account_id].flags = flags;
    return let_error_none();
}

void let_account_list_free(let_account_list_t *account_list) {
    if (account_list != nullptr) {
        free(account_list->accounts);
        free(account_list);
    }
}

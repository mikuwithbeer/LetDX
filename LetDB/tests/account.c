#include "let/account.h"

#include <assert.h>

static let_account_list_t *account_list;

static void test_account_new(void) {
    const auto account_1 = let_account_new(
        100,
        50,
        0,
        LET_ACCOUNT_FLAG_CAN_SEND | LET_ACCOUNT_FLAG_CAN_RECEIVE);

    const auto account_2 = (let_account_t){
        .credits = 100,
        .debits = 50,
        .created_at = 0,
        .updated_at = 0,
        .transactions = 0,
        .flags = LET_ACCOUNT_FLAG_CAN_SEND | LET_ACCOUNT_FLAG_CAN_RECEIVE
    };

    assert(account_1.credits == account_2.credits);
    assert(account_1.debits == account_2.debits);
    assert(account_1.created_at == account_2.created_at);
    assert(account_1.updated_at == account_2.updated_at);
    assert(account_1.transactions == account_2.transactions);
    assert(account_1.flags == account_2.flags);
}

static void test_account_list_new(void) {
    account_list = let_account_list_new();

    assert(account_list != nullptr);
    assert(account_list->length == 0);
    assert(account_list->capacity == LET_ACCOUNT_CAPACITY_DEFAULT);
    assert(account_list->accounts != nullptr);
}

static void test_account_list_add(void) {
    const auto account_1 = let_account_new(0, 50, 0, LET_ACCOUNT_FLAG_CAN_SEND);
    const auto account_2 = let_account_new(0, 50, 0, LET_ACCOUNT_FLAG_CAN_RECEIVE);

    assert(!let_error_exists(let_account_list_add(account_list, account_1)));
    assert(!let_error_exists(let_account_list_add(account_list, account_2)));
}

static void test_account_list_get(void) {
    let_account_t account;

    assert(!let_error_exists(let_account_list_get(account_list, 0, &account)));
    assert(account.credits == 0);
    assert(account.debits == 50);
    assert(account.flags == LET_ACCOUNT_FLAG_CAN_SEND);

    assert(!let_error_exists(let_account_list_get(account_list, 1, &account)));
    assert(account.credits == 0);
    assert(account.debits == 50);
    assert(account.flags == LET_ACCOUNT_FLAG_CAN_RECEIVE);

    assert(let_error_exists(let_account_list_get(account_list, 2, &account)));
}

static void test_account_list_update(void) {
    assert(!let_error_exists(let_account_list_update(account_list, 0, LET_ACCOUNT_FLAG_CAN_RECEIVE)));
    assert(!let_error_exists(let_account_list_update(account_list, 1, LET_ACCOUNT_FLAG_CAN_SEND)));
}

static void test_cleanup(void) {
    let_account_list_free(account_list);
    assert(true);
}

int main(void) {
    test_account_new();
    test_account_list_new();
    test_account_list_add();
    test_account_list_get();
    test_account_list_update();

    test_cleanup();
    return 0;
}

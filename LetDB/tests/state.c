#include "let/state.h"

#include <assert.h>

static let_state_t state;

static void test_state_init(void) {
    const auto accounts_list = let_account_list_new();
    assert(accounts_list != nullptr);

    state = let_state_empty();
    assert(state.account_list == nullptr);

    assert(!let_error_exists(let_state_init(&state, accounts_list)));
    assert(state.account_list == accounts_list);
}

static void test_state_add_account(void) {
    let_u64_t account_id;

    auto account = let_account_new(0, 50, 0, LET_ACCOUNT_FLAG_CAN_SEND);
    assert(!let_error_exists(let_state_add_account(&state, account, &account_id)));
    assert(account_id == 0);

    account = let_account_new(0, 100, 0, LET_ACCOUNT_FLAG_CAN_RECEIVE);
    assert(!let_error_exists(let_state_add_account(&state, account, &account_id)));
    assert(account_id == 1);
}

static void test_state_make_transfer(void) {
    let_account_t account;

    assert(!let_error_exists(let_state_make_transfer(&state, 0, 1, 25)));

    assert(!let_error_exists(let_account_list_get(state.account_list, 0, &account)));
    assert(account.credits == 25);
    assert(account.debits == 50);
    assert(account.transactions == 1);

    assert(!let_error_exists(let_account_list_get(state.account_list, 1, &account)));
    assert(account.credits == 0);
    assert(account.debits == 125);
    assert(account.transactions == 1);
}

static void test_state_update_account(void) {
    let_account_t account;

    assert(!let_error_exists(let_account_list_get(state.account_list, 0, &account)));
    assert(account.flags == LET_ACCOUNT_FLAG_CAN_SEND);

    assert(!let_error_exists(let_state_update_account(&state, 0, LET_ACCOUNT_FLAG_CAN_RECEIVE)));

    assert(!let_error_exists(let_account_list_get(state.account_list, 0, &account)));
    assert(account.flags == LET_ACCOUNT_FLAG_CAN_RECEIVE);
}

static void test_cleanup(void) {
    let_account_list_free(state.account_list);
    assert(true);
}

int main(void) {
    test_state_init();
    test_state_add_account();
    test_state_make_transfer();
    test_state_update_account();

    test_cleanup();
    return 0;
}

#include "let/guard.h"

#include <assert.h>

static let_state_t state;
static let_guard_t guard;

static void test_guard_init(void) {
    const auto accounts_list = let_account_list_new();
    assert(accounts_list != nullptr);

    state = let_state_empty();
    assert(state.account_list == nullptr);

    assert(!let_error_exists(let_state_init(&state, accounts_list)));
    assert(state.account_list == accounts_list);

    guard = let_guard_empty();
    assert(guard.state == nullptr);

    assert(!let_error_exists(let_guard_init(&guard, &state)));
    assert(guard.state == &state);
}

static void test_guard_make_transfer(void) {
    auto error = let_guard_make_transfer(&guard, 0, 0, 0);
    assert(error.id == LET_ERROR_ID_GUARD && error.error == LET_ERROR_GUARD_ZERO_BALANCE);

    error = let_guard_make_transfer(&guard, 0, 0, 1000);
    assert(error.id == LET_ERROR_ID_GUARD && error.error == LET_ERROR_GUARD_SAME_ACCOUNT);

    error = let_guard_make_transfer(&guard, 1, 0, 1000);
    assert(error.id == LET_ERROR_ID_GUARD && error.error == LET_ERROR_GUARD_SENDER_NOT_FOUND);

    auto account = let_account_new(0, 100, 0, LET_ACCOUNT_FLAG_CAN_RECEIVE);
    assert(!let_error_exists(let_account_list_add(state.account_list, account)));

    error = let_guard_make_transfer(&guard, 0, 1, 1000);
    assert(error.id == LET_ERROR_ID_GUARD && error.error == LET_ERROR_GUARD_RECIPIENT_NOT_FOUND);

    account = let_account_new(0, 100, 0, LET_ACCOUNT_FLAG_CAN_SEND);
    assert(!let_error_exists(let_account_list_add(state.account_list, account)));

    error = let_guard_make_transfer(&guard, 0, 1, 1000);
    assert(error.id == LET_ERROR_ID_GUARD && error.error == LET_ERROR_GUARD_ACCOUNT_CANNOT_SEND);

    const auto from_account = &state.account_list->accounts[0];
    const auto to_account = &state.account_list->accounts[1];

    from_account->flags |= LET_ACCOUNT_FLAG_CAN_SEND;

    error = let_guard_make_transfer(&guard, 0, 1, 1000);
    assert(error.id == LET_ERROR_ID_GUARD && error.error == LET_ERROR_GUARD_ACCOUNT_CANNOT_RECEIVE);

    to_account->flags |= LET_ACCOUNT_FLAG_CAN_RECEIVE;

    error = let_guard_make_transfer(&guard, 0, 1, 1000);
    assert(error.id == LET_ERROR_ID_GUARD && error.error == LET_ERROR_GUARD_INSUFFICIENT_BALANCE);

    error = let_guard_make_transfer(&guard, 0, 1, 100);
    assert(!let_error_exists(error));

    from_account->flags |= LET_ACCOUNT_FLAG_CAN_DEBT;

    error = let_guard_make_transfer(&guard, 0, 1, 1000);
    assert(!let_error_exists(error));
}

static void test_guard_update_account(void) {
    assert(!let_error_exists(let_guard_update_account(&guard, 0)));
    assert(!let_error_exists(let_guard_update_account(&guard, 1)));
    assert(let_error_exists(let_guard_update_account(&guard, 2)));
}

static void test_cleanup(void) {
    let_account_list_free(state.account_list);
    assert(true);
}

int main(void) {
    test_guard_init();
    test_guard_make_transfer();
    test_guard_update_account();

    test_cleanup();
    return 0;
}

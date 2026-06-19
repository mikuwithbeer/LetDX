#include "let/network/request/request.h"

#include <assert.h>

static let_network_request_t request;
static let_error_t error;

static void test_request_unknown(void) {
    let_u8_t test_1[] = "W1 0 0 ";
    error = let_network_request_decode(test_1, sizeof(test_1) - 1, &request);

    assert(error.id == LET_ERROR_ID_NETWORK);
    assert(error.error == LET_ERROR_NETWORK_REQUEST_INVALID_COMMAND);
}

static void test_request_malformed(void) {
    let_u8_t test_1[] = "?0 0";
    error = let_network_request_decode(test_1, sizeof(test_1) - 1, &request);

    assert(error.id == LET_ERROR_ID_NETWORK);
    assert(error.error == LET_ERROR_NETWORK_REQUEST_MALFORMED);
}

static void test_request_new_line(void) {
    let_u8_t test_1[] = ";?";
    error = let_network_request_decode(test_1, sizeof(test_1) - 1, &request);

    assert(error.id == LET_ERROR_ID_NETWORK);
    assert(error.error == LET_ERROR_NETWORK_REQUEST_EXPECTED_NEW_LINE);
}

static void test_request_missing(void) {
    let_u8_t test_1[] = "%0 0 0 ";
    error = let_network_request_decode(test_1, sizeof(test_1) - 1, &request);

    assert(error.id == LET_ERROR_ID_NETWORK);
    assert(error.error == LET_ERROR_NETWORK_REQUEST_ARGUMENTS_MISSING);
}

static void test_request_integer(void) {
    let_u8_t test_1[] = "+0 0 100 256 ";
    error = let_network_request_decode(test_1, sizeof(test_1) - 1, &request);

    assert(error.id == LET_ERROR_ID_NETWORK);
    assert(error.error == LET_ERROR_NETWORK_REQUEST_INTEGER_OVERFLOW);

    let_u8_t test_2[] = "%0 u0 u1 1000010000010 ";
    error = let_network_request_decode(test_2, sizeof(test_2) - 1, &request);

    assert(error.id == LET_ERROR_ID_NETWORK);
    assert(error.error == LET_ERROR_NETWORK_REQUEST_INVALID_INTEGER);
}

static void test_request_commands(void) {
    let_u8_t test_1[] = "+0 1_000 5_000 7 ";
    error = let_network_request_decode(test_1, sizeof(test_1) - 1, &request);

    assert(!let_error_exists(error));
    assert(request.type == LET_NETWORK_REQUEST_TYPE_ADD_ACCOUNT);
    assert(request.data.create_account.wal_id == 0);
    assert(request.data.create_account.credits == 1000);
    assert(request.data.create_account.debits == 5000);
    assert(request.data.create_account.flags == 7);

    let_u8_t test_2[] = "%1 0 1 1000 ";
    error = let_network_request_decode(test_2, sizeof(test_2) - 1, &request);
    assert(!let_error_exists(error));
    assert(request.type == LET_NETWORK_REQUEST_TYPE_MAKE_TRANSFER);
    assert(request.data.make_transfer.wal_id == 1);
    assert(request.data.make_transfer.from_id == 0);
    assert(request.data.make_transfer.to_id == 1);
    assert(request.data.make_transfer.amount == 1000);

    let_u8_t test_3[] = "?13_37 ";
    error = let_network_request_decode(test_3, sizeof(test_3) - 1, &request);
    assert(!let_error_exists(error));
    assert(request.type == LET_NETWORK_REQUEST_TYPE_GET_ACCOUNT);
    assert(request.data.get_account == 1337);

    let_u8_t test_4[] = "=2 0 3 ";
    error = let_network_request_decode(test_4, sizeof(test_4) - 1, &request);
    assert(!let_error_exists(error));
    assert(request.type == LET_NETWORK_REQUEST_TYPE_UPDATE_ACCOUNT);
    assert(request.data.update_account.wal_id == 2);
    assert(request.data.update_account.account_id == 0);
    assert(request.data.update_account.flags == 3);

    let_u8_t test_5[] = "; ";
    error = let_network_request_decode(test_5, sizeof(test_5) - 1, &request);
    assert(!let_error_exists(error));
    assert(request.type == LET_NETWORK_REQUEST_TYPE_MAGIC);

    let_u8_t test_6[] = "# ";
    error = let_network_request_decode(test_6, sizeof(test_6) - 1, &request);
    assert(!let_error_exists(error));
    assert(request.type == LET_NETWORK_REQUEST_TYPE_COUNT_ENTRIES);

    let_u8_t test_7[] = ". ";
    error = let_network_request_decode(test_7, sizeof(test_7) - 1, &request);
    assert(!let_error_exists(error));
    assert(request.type == LET_NETWORK_REQUEST_TYPE_CLOSE);
}

static void test_cleanup(void) {
    request = let_network_request_empty();
    error = let_error_none();
    assert(true);
}

int main(void) {
    test_request_unknown();
    test_request_malformed();
    test_request_new_line();
    test_request_missing();
    test_request_integer();
    test_request_commands();

    test_cleanup();
    return 0;
}

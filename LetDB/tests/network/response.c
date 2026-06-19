#include "let/network/response/response.h"

#include <assert.h>

static let_size_t written;
static let_u8_t buffer[32];

static let_network_response_t response;
static let_error_t error;

static void test_response_magic(void) {
    response.type = LET_NETWORK_RESPONSE_TYPE_MAGIC;

    error = let_network_response_encode(response, buffer, sizeof(buffer), &written);
    assert(!let_error_exists(error));
    assert(written == 4);

    assert(buffer[0] == 'L');
    assert(buffer[1] == 'E');
    assert(buffer[2] == 'T');
    assert(buffer[3] == '\n');
}

static void test_response_add_account(void) {
    response.type = LET_NETWORK_RESPONSE_TYPE_ADD_ACCOUNT;
    response.data.add_account = 42;

    error = let_network_response_encode(response, buffer, sizeof(buffer), &written);
    assert(!let_error_exists(error));
    assert(written == 7);

    assert(buffer[0] == 'A');
    assert(buffer[1] == 'I');
    assert(buffer[2] == 'D');
    assert(buffer[3] == ' ');
    assert(buffer[4] == '2');
    assert(buffer[5] == 'A');
    assert(buffer[6] == '\n');
}

static void test_response_get_account(void) {
    response.type = LET_NETWORK_RESPONSE_TYPE_GET_ACCOUNT;
    response.data.get_account.credits = 1000;
    response.data.get_account.debits = 256;
    response.data.get_account.flags = 7;

    error = let_network_response_encode(response, buffer, sizeof(buffer), &written);
    assert(!let_error_exists(error));
    assert(written == 17);

    assert(buffer[0] == 'A');
    assert(buffer[1] == 'C');
    assert(buffer[2] == 'C');
    assert(buffer[3] == ' ');
    assert(buffer[4] == '0');
    assert(buffer[5] == '3');
    assert(buffer[6] == 'E');
    assert(buffer[7] == '8');
    assert(buffer[8] == ' ');
    assert(buffer[9] == '0');
    assert(buffer[10] == '1');
    assert(buffer[11] == '0');
    assert(buffer[12] == '0');
    assert(buffer[13] == ' ');
    assert(buffer[14] == '0');
    assert(buffer[15] == '7');
    assert(buffer[16] == '\n');
}

static void test_response_count_entries(void) {
    response.type = LET_NETWORK_RESPONSE_TYPE_COUNT_ENTRIES;
    response.data.count_entries = 123456789;

    error = let_network_response_encode(response, buffer, sizeof(buffer), &written);
    assert(!let_error_exists(error));
    assert(written == 13);

    assert(buffer[0] == 'S');
    assert(buffer[1] == 'E');
    assert(buffer[2] == 'C');
    assert(buffer[3] == ' ');
    assert(buffer[4] == '0');
    assert(buffer[5] == '7');
    assert(buffer[6] == '5');
    assert(buffer[7] == 'B');
    assert(buffer[8] == 'C');
    assert(buffer[9] == 'D');
    assert(buffer[10] == '1');
    assert(buffer[11] == '5');
    assert(buffer[12] == '\n');
}

static void test_response_ok(void) {
    response.type = LET_NETWORK_RESPONSE_TYPE_OK;

    error = let_network_response_encode(response, buffer, sizeof(buffer), &written);
    assert(!let_error_exists(error));
    assert(written == 4);

    assert(buffer[0] == 'O');
    assert(buffer[1] == 'K');
    assert(buffer[2] == 'E');
    assert(buffer[3] == '\n');
}

static void test_response_error(void) {
    response.type = LET_NETWORK_RESPONSE_TYPE_ERROR;
    response.data.error = let_error_new(LET_ERROR_ID_ACCOUNT, LET_ERROR_ACCOUNT_NOT_FOUND);

    error = let_network_response_encode(response, buffer, sizeof(buffer), &written);
    assert(!let_error_exists(error));
    assert(written == 9);

    assert(buffer[0] == 'E');
    assert(buffer[1] == 'R');
    assert(buffer[2] == 'R');
    assert(buffer[3] == ' ');
    assert(buffer[4] == '0');
    assert(buffer[5] == '3');
    assert(buffer[6] == 'E');
    assert(buffer[7] == 'A');
    assert(buffer[8] == '\n');
}

static void test_response_buffer_overflow(void) {
    response.type = LET_NETWORK_RESPONSE_TYPE_GET_ACCOUNT;
    response.data.get_account.credits = 0xFFAAFFBBEEDDCCD;
    response.data.get_account.debits = 0x1337133769C0FFEE;
    response.data.get_account.flags = 0xFF;

    error = let_network_response_encode(response, buffer, sizeof(buffer), &written);
    assert(error.id == LET_ERROR_ID_NETWORK);
    assert(error.error == LET_ERROR_NETWORK_RESPONSE_BUFFER_OVERFLOW);
}

static void test_cleanup(void) {
    response = let_network_response_empty();
    error = let_error_none();
    assert(true);
}

int main(void) {
    test_response_magic();
    test_response_add_account();
    test_response_get_account();
    test_response_count_entries();
    test_response_ok();
    test_response_error();
    test_response_buffer_overflow();

    test_cleanup();
    return 0;
}

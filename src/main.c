#include "network.h"

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int main(void) {
    /*
    const auto account_list = let_account_list_new();
    const auto state = let_state_new(account_list);

    if (state == nullptr) {
        return -1;
    }

    const let_account_t add_account_1 = {
        .credits = 0,
        .debits = 100,

        .created_at = time(nullptr),
        .updated_at = time(nullptr),

        .transactions = 0,
        .flags = LET_ACCOUNT_FLAG_CAN_SEND
    };


    const let_account_t add_account_2 = {
        .credits = 0,
        .debits = 50,

        .created_at = time(nullptr),
        .updated_at = time(nullptr),

        .transactions = 0,
        .flags = LET_ACCOUNT_FLAG_CAN_RECEIVE
    };

    let_u64_t account_id_1, account_id_2;
    const auto add_account_result_1 = let_state_add_account(state, add_account_1, &account_id_1);
    const auto add_account_result_2 = let_state_add_account(state, add_account_2, &account_id_2);

    if (add_account_result_1 != LET_STATE_ERROR_NONE || add_account_result_2 != LET_STATE_ERROR_NONE) {
        printf("Failed to add account: %d\n", add_account_result_1);
        printf("Failed to add account: %d\n", add_account_result_2);
        return -1;
    }

    printf("Account ID: %llu\n", account_id_1);
    printf("Account ID: %llu\n", account_id_2);

    sleep(2);

    const auto transfer_result = let_state_make_transfer(state, account_id_1, account_id_2, 100);
    if (transfer_result != LET_STATE_ERROR_NONE) {
        printf("Failed to transfer: %d\n", transfer_result);
        return -1;
    }

    printf("Transfer successful\n");

    const auto account_1 = &state->account_list->accounts[account_id_1];
    const auto account_2 = &state->account_list->accounts[account_id_2];

    printf("Account 1 balance: %llu\n", (let_i64_t) (account_1->debits - account_1->credits));
    printf("Account 2 balance: %llu\n", (let_i64_t) (account_2->debits - account_2->credits));

    printf("Created at: %llu\n", account_1->created_at);
    printf("Updated at: %llu\n", account_1->updated_at);
    printf("Transactions: %llu\n", account_1->transactions);

    let_state_free(state);
    let_account_list_free(account_list);

    */

    let_network_server_t server = {
        .port = 3169
    };

    let_network_client_t client = {};

    if (let_network_server_init(&server) != LET_NETWORK_ERROR_NONE) {
        return -1;
    }

    if (let_network_server_accept(&server, &client) != LET_NETWORK_ERROR_NONE) {
        return -1;
    }

    int counter = 3;
    while (true) {
        char buffer[1024] = {0};
        if (let_network_client_read(&client, &buffer, 1024) != LET_NETWORK_ERROR_NONE) {
            return -1;
        }

        printf("%s", buffer);

        if (let_network_client_write(&client, buffer, strlen(buffer)) != LET_NETWORK_ERROR_NONE) {
            return -1;
        }

        counter--;
        if (counter == 0) {
            break;
        }
    }

    let_network_free(&client);
    let_network_free(&server);

    return 0;
}

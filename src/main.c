/*
 *
#include "let/network/server.h"
#include "let/guard.h"

#include <stdio.h>

int main(void) {
    const auto account_list = let_account_list_new();
    const auto state = let_state_new(account_list);
    const auto guard = let_guard_new(state);

    /*

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

    let_network_server_t server = {
        .port = 3169
    };

    let_network_server_t client = {0};

    if (let_network_server_init(&server).id != LET_ERROR_ID_NONE) {
        return -1;
    }

    if (let_network_server_accept(&server, &client).id != LET_ERROR_ID_NONE) {
        return -1;
    }

    while (true) {
        let_network_request_t request = let_network_request_new();
        let_network_response_t response = {0};

        const auto read_request = let_network_client_read(&client, &request);
        if (read_request.id != LET_ERROR_ID_NONE) {
            printf("Failed to read request: %d\n", read_request.error);
            break;
        }

        switch (request.id) {
            case LET_NETWORK_REQUEST_ID_MAGIC:
                response.id = LET_NETWORK_RESPONSE_ID_MAGIC;
                break;
            case LET_NETWORK_REQUEST_ID_ADD_ACCOUNT: {
                response.id = LET_NETWORK_RESPONSE_ID_ADD_ACCOUNT;

                const auto add_account = let_account_new(
                    0,
                    request.data.create_account.balance,
                    request.data.create_account.flags);

                let_u64_t account_id;

                const auto add_account_result = let_state_add_account(state, add_account, &account_id);
                if (add_account_result.id != LET_ERROR_ID_NONE) {
                    response.id = LET_NETWORK_RESPONSE_ID_ERROR;
                    response.data.error = add_account_result;
                } else {
                    response.data.add_account = account_id;
                }

                break;
            }
            case LET_NETWORK_REQUEST_ID_MAKE_TRANSFER: {
                response.id = LET_NETWORK_RESPONSE_ID_OK;

                const auto from_id = request.data.make_transfer.from_id;
                const auto to_id = request.data.make_transfer.to_id;
                const auto amount = request.data.make_transfer.amount;

                const auto guard_result = let_guard_make_transfer(guard, from_id, to_id, amount);
                if (guard_result.id != LET_ERROR_ID_NONE) {
                    response.id = LET_NETWORK_RESPONSE_ID_ERROR;
                    response.data.error = guard_result;
                    break;
                }

                const auto transfer_result = let_state_make_transfer(state, from_id, to_id, amount);
                if (transfer_result.id != LET_ERROR_ID_NONE) {
                    response.id = LET_NETWORK_RESPONSE_ID_ERROR;
                    response.data.error = transfer_result;
                }

                break;
            }
            case LET_NETWORK_REQUEST_ID_GET_BALANCE: {
                response.id = LET_NETWORK_RESPONSE_ID_GET_BALANCE;

                const auto account_id = request.data.get_balance;
                let_account_t account;

                const auto account_result = let_account_list_get(account_list, account_id, &account);
                if (account_result.id != LET_ERROR_ID_NONE) {
                    response.id = LET_NETWORK_RESPONSE_ID_ERROR;
                    response.data.error = account_result;
                } else {
                    response.data.get_balance = account.debits - account.credits;
                }

                break;
            }
            case LET_NETWORK_REQUEST_ID_CLOSE:
                response.id = LET_NETWORK_RESPONSE_ID_OK;
                break;
        }

        const auto write_response = let_network_client_write(&client, &response);
        if (write_response.id != LET_ERROR_ID_NONE) {
            printf("Failed to write response: %d\n", write_response.error);
        }
    }

    let_network_close(&client);
    let_network_close(&server);

    let_guard_free(guard);
    let_state_free(state);
    let_account_list_free(account_list);

    return 0;
}

*/


#include "let/storage/wal.h"

int main(void) {
    const auto account_list = let_account_list_new();

    auto state = let_state_empty();
    let_state_init(&state, account_list);

    auto storage_wal = let_storage_wal_empty();
    if (let_storage_wal_init(&storage_wal, &state, "wal.db").id != LET_ERROR_ID_NONE) {
        return -1;
    }

    const auto replay_result = let_storage_wal_replay(&storage_wal);

    let_storage_wal_entry_t entry_1 = let_storage_wal_entry_new(storage_wal.transactions, LET_STORAGE_WAL_ENTRY_TYPE_ADD_ACCOUNT);
    let_storage_wal_entry_t entry_2 = let_storage_wal_entry_new(storage_wal.transactions + 1, LET_STORAGE_WAL_ENTRY_TYPE_MAKE_TRANSFER);

    entry_1.data.add_account = (let_storage_wal_entry_add_account_t){
        .balance = 10,
        .flags = 0
    };


    entry_2.data.make_transfer = (let_storage_wal_entry_make_transfer_t){
        .from_id = 0,
        .to_id = 1,
        .amount = 100
    };


    let_storage_wal_write(&storage_wal, &entry_1);
    let_storage_wal_write(&storage_wal, &entry_2);

    let_storage_wal_close(&storage_wal);

    let_account_list_free(account_list);

    return 0;
}

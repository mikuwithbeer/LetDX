#include "let/network/server.h"
#include "let/storage/wal.h"
#include "let/guard.h"

#include <signal.h>

volatile sig_atomic_t keep_running = true;

void on_shutdown(int _) {
    keep_running = false;
}

int main(void) {
    signal(SIGTERM, on_shutdown);
    signal(SIGINT, on_shutdown);

    const auto account_list = let_account_list_new();
    auto state = let_state_empty();

    auto global_result = let_state_init(&state, account_list);
    if (global_result.id != LET_ERROR_ID_NONE) {
        let_account_list_free(account_list);
        return -1;
    }

    auto guard = let_guard_empty();

    global_result = let_guard_init(&guard, &state);
    if (global_result.id != LET_ERROR_ID_NONE) {
        let_account_list_free(account_list);
        return -1;
    }

    auto storage_wal = let_storage_wal_empty();

    global_result = let_storage_wal_init(&storage_wal, &state, "let__wal");
    if (global_result.id != LET_ERROR_ID_NONE) {
        let_account_list_free(account_list);
        return -1;
    }

    global_result = let_storage_wal_replay(&storage_wal);
    if (global_result.id != LET_ERROR_ID_NONE) {
        let_account_list_free(account_list);
        return -1;
    }

    auto network_server = let_network_server_empty();
    auto network_client = let_network_server_empty();

    global_result = let_network_server_init(&network_server, 3169);
    if (global_result.id != LET_ERROR_ID_NONE) {
        let_account_list_free(account_list);
        return -1;
    }

    while (keep_running) {
        global_result = let_network_server_accept(&network_server, &network_client);
        if (global_result.id != LET_ERROR_ID_NONE) {
            break;
        }

        while (keep_running) {
            auto network_request = let_network_request_empty();
            auto network_response = let_network_response_empty();

            global_result = let_network_client_read(&network_client, &network_request);
            if (global_result.id != LET_ERROR_ID_NONE) {
                break;
            }

            switch (network_request.type) {
                case LET_NETWORK_REQUEST_TYPE_MAGIC: {
                    network_response.id = LET_NETWORK_RESPONSE_ID_MAGIC;
                    break;
                }
                case LET_NETWORK_REQUEST_TYPE_ADD_ACCOUNT: {
                    network_response.id = LET_NETWORK_RESPONSE_ID_ADD_ACCOUNT;

                    auto storage_wal_entry = let_storage_wal_entry_new(
                        network_request.data.create_account.wal_id,
                        LET_STORAGE_WAL_ENTRY_TYPE_ADD_ACCOUNT);

                    storage_wal_entry.data.add_account = (let_storage_wal_entry_add_account_t){
                        .balance = network_request.data.create_account.balance,
                        .flags = network_request.data.create_account.flags
                    };

                    global_result = let_storage_wal_write(&storage_wal, &storage_wal_entry);
                    if (global_result.id != LET_ERROR_ID_NONE) {
                        network_response.id = LET_NETWORK_RESPONSE_ID_ERROR;
                        network_response.data.error = global_result;
                        break;
                    }

                    const auto add_account = let_account_new(
                        0,
                        network_request.data.create_account.balance,
                        network_request.data.create_account.flags);

                    let_u64_t account_id;
                    global_result = let_state_add_account(&state, add_account, &account_id);
                    if (global_result.id != LET_ERROR_ID_NONE) {
                        network_response.id = LET_NETWORK_RESPONSE_ID_ERROR;
                        network_response.data.error = global_result;
                    } else {
                        network_response.data.add_account = account_id;
                    }

                    break;
                }
                case LET_NETWORK_REQUEST_TYPE_MAKE_TRANSFER: {
                    network_response.id = LET_NETWORK_RESPONSE_ID_OK;

                    const auto from_id = network_request.data.make_transfer.from_id;
                    const auto to_id = network_request.data.make_transfer.to_id;
                    const auto amount = network_request.data.make_transfer.amount;

                    global_result = let_guard_make_transfer(&guard, from_id, to_id, amount);
                    if (global_result.id != LET_ERROR_ID_NONE) {
                        network_response.id = LET_NETWORK_RESPONSE_ID_ERROR;
                        network_response.data.error = global_result;
                        break;
                    }

                    auto storage_wal_entry = let_storage_wal_entry_new(
                        network_request.data.make_transfer.wal_id,
                        LET_STORAGE_WAL_ENTRY_TYPE_MAKE_TRANSFER);

                    storage_wal_entry.data.make_transfer = (let_storage_wal_entry_make_transfer_t){
                        .from_id = from_id,
                        .to_id = to_id,
                        .amount = amount
                    };

                    global_result = let_storage_wal_write(&storage_wal, &storage_wal_entry);
                    if (global_result.id != LET_ERROR_ID_NONE) {
                        network_response.id = LET_NETWORK_RESPONSE_ID_ERROR;
                        network_response.data.error = global_result;
                        break;
                    }

                    global_result = let_state_make_transfer(&state, from_id, to_id, amount);
                    if (global_result.id != LET_ERROR_ID_NONE) {
                        network_response.id = LET_NETWORK_RESPONSE_ID_ERROR;
                        network_response.data.error = global_result;
                    }

                    break;
                }
                case LET_NETWORK_REQUEST_TYPE_GET_BALANCE: {
                    network_response.id = LET_NETWORK_RESPONSE_ID_GET_BALANCE;

                    const auto account_id = network_request.data.get_balance;

                    let_account_t account;
                    global_result = let_account_list_get(account_list, account_id, &account);
                    if (global_result.id != LET_ERROR_ID_NONE) {
                        network_response.id = LET_NETWORK_RESPONSE_ID_ERROR;
                        network_response.data.error = global_result;
                    } else {
                        network_response.data.get_balance = account.debits - account.credits;
                    }

                    break;
                }
                case LET_NETWORK_REQUEST_TYPE_CLOSE: {
                    network_response.id = LET_NETWORK_RESPONSE_ID_OK;
                    break;
                }
            }

            global_result = let_network_client_write(&network_client, &network_response);
            if (global_result.id != LET_ERROR_ID_NONE) {
                break;
            }

            if (network_request.type == LET_NETWORK_REQUEST_TYPE_CLOSE) {
                break;
            }
        }

        let_network_close(&network_client);
    }

    if (global_result.id != LET_ERROR_ID_NONE) {
        printf("Error Code: %d", let_error_code(global_result));
    }

    global_result = let_storage_wal_sync(&storage_wal);
    if (global_result.id != LET_ERROR_ID_NONE) {
        printf("Error Code: %d", let_error_code(global_result));
    }

    let_network_close(&network_server);

    let_storage_wal_close(&storage_wal);

    let_account_list_free(account_list);

    return 0;
}

#include "let/let.h"

let_t let = {};

void let_init(void) {
    let.account_list = let_account_list_new();
    let.state = let_state_empty();

    let.error = let_state_init(&let.state, let.account_list);
    if (let_error_exists(let.error)) {
        return;
    }

    let.guard = let_guard_empty();

    let.error = let_guard_init(&let.guard, &let.state);
    if (let_error_exists(let.error)) {
        return;
    }

    let.storage_wal = let_storage_wal_empty();

    let.error = let_storage_wal_init(&let.storage_wal, &let.state, "let__wal");
    if (let_error_exists(let.error)) {
        return;
    }

    let.error = let_storage_wal_replay(&let.storage_wal);
    if (let_error_exists(let.error)) {
        return;
    }

    let.network_server = let_network_server_empty();
    let.network_client = let_network_server_empty();

    let.error = let_network_server_init(&let.network_server, 8081);
    if (let_error_exists(let.error)) {
        return;
    }

    let.running = true;
}

void let_run(void) {
    while (let.running) {
        let.error = let_network_server_accept(&let.network_server, &let.network_client);
        if (let_error_exists(let.error)) {
            break;
        }

        while (let.running) {
            auto network_request = let_network_request_empty();
            auto network_response = let_network_response_empty();

            let.error = let_network_client_read(&let.network_client, &network_request);
            if (let_error_exists(let.error)) {
                printf("read error: %d\n", let_error_code(let.error));
                break;
            }

            let_error_t runtime_error = let_error_none();
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

                    runtime_error = let_storage_wal_write(&let.storage_wal, &storage_wal_entry);
                    if (let_error_exists(runtime_error)) {
                        break;
                    }

                    const auto add_account = let_account_new(
                        0,
                        network_request.data.create_account.balance,
                        network_request.data.create_account.flags);

                    let_u64_t account_id;
                    runtime_error = let_state_add_account(&let.state, add_account, &account_id);
                    if (let_error_exists(runtime_error)) {
                        break;
                    }

                    network_response.data.add_account = account_id;
                    break;
                }
                case LET_NETWORK_REQUEST_TYPE_MAKE_TRANSFER: {
                    network_response.id = LET_NETWORK_RESPONSE_ID_OK;

                    const auto from_id = network_request.data.make_transfer.from_id;
                    const auto to_id = network_request.data.make_transfer.to_id;
                    const auto amount = network_request.data.make_transfer.amount;

                    runtime_error = let_guard_make_transfer(&let.guard, from_id, to_id, amount);
                    if (let_error_exists(runtime_error)) {
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

                    runtime_error = let_storage_wal_write(&let.storage_wal, &storage_wal_entry);
                    if (let_error_exists(runtime_error)) {
                        break;
                    }

                    runtime_error = let_state_make_transfer(&let.state, from_id, to_id, amount);
                    break;
                }
                case LET_NETWORK_REQUEST_TYPE_GET_BALANCE: {
                    network_response.id = LET_NETWORK_RESPONSE_ID_GET_BALANCE;

                    const auto account_id = network_request.data.get_balance;

                    let_account_t account;
                    runtime_error = let_account_list_get(let.account_list, account_id, &account);
                    if (let_error_exists(runtime_error)) {
                        break;
                    }

                    network_response.data.get_balance = account.debits - account.credits;
                    break;
                }
                case LET_NETWORK_REQUEST_TYPE_CLOSE: {
                    network_response.id = LET_NETWORK_RESPONSE_ID_OK;
                    break;
                }
            }

            if (let_error_exists(runtime_error)) {
                network_response.id = LET_NETWORK_RESPONSE_ID_ERROR;
                network_response.data.error = runtime_error;
            }

            let.error = let_network_client_write(&let.network_client, &network_response);
            if (let_error_exists(let.error)) {
                printf("write error: %d\n", let_error_code(let.error));
                break;
            }

            if (network_request.type == LET_NETWORK_REQUEST_TYPE_CLOSE) {
                break;
            }
        }

        let_network_close(&let.network_client);
    }
}

void let_close(const int signal) {
    (void) signal;
    let.running = false;
}

void let_cleanup(void) {
    let.error = let_storage_wal_sync(&let.storage_wal);

    let_network_close(&let.network_server);
    let_storage_wal_close(&let.storage_wal);
    let_account_list_free(let.account_list);
}

int main(void) {
    int success = EXIT_SUCCESS;

    struct sigaction signal_action = {};
    signal_action.sa_handler = let_close;

    sigaction(SIGTERM, &signal_action, nullptr);
    sigaction(SIGINT, &signal_action, nullptr);

    let_init();
    if (let_error_exists(let.error)) {
        printf("init error: %d\n", let_error_code(let.error));
        success = EXIT_FAILURE;
        goto cleanup;
    }

    let_run();
    if (let_error_exists(let.error)) {
        printf("runtime error: %d\n", let_error_code(let.error));
        success = EXIT_FAILURE;
    }

cleanup:
    let_cleanup();
    if (let_error_exists(let.error)) {
        printf("cleanup error: %d\n", let_error_code(let.error));
        success = EXIT_FAILURE;
    }

    return success;
}

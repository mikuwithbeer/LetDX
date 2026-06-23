#include "let/let.h"
#include "let/cli.h"

let_t let = {};

static let_error_t let_request(const let_network_request_t *network_request,
                               let_network_response_t *network_response);

void let_init(const let_cli_t *cli) {
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
    let.error = let_storage_wal_init(&let.storage_wal, &let.state, cli->storage_file);
    if (let_error_exists(let.error)) {
        return;
    }

    let.error = let_storage_wal_replay(&let.storage_wal, cli->truncate_on_fail);
    if (let_error_exists(let.error)) {
        return;
    }

    let.network_server = let_network_server_empty();
    let.network_client = let_network_server_empty();

    let.error = let_network_server_init(&let.network_server, cli->port, cli->backlog);
    if (let_error_exists(let.error)) {
        return;
    }

    let_log_print(LET_LOG_LEVEL_INFO,
                  "Application initialized on port:%u, backlog:%u",
                  cli->port,
                  cli->backlog);

    let.accepting = true;
}

void let_run(const let_cli_t *cli) {
    while (let.accepting) {
        let_log_print(LET_LOG_LEVEL_DEBUG,
                      "Awaiting incoming connection with rt:%us, wt:%us",
                      cli->read_timeout,
                      cli->write_timeout);

        let.error = let_network_server_accept(&let.network_server,
                                              cli->read_timeout,
                                              cli->write_timeout,
                                              &let.network_client);
        if (let_error_exists(let.error)) {
            break;
        }

        let_log_print(LET_LOG_LEVEL_DEBUG,
                      "Connection accepted from id:%u",
                      let.network_client.handle);

        let.executing = true;
        while (let.executing) {
            auto network_request = let_network_request_empty();
            auto network_response = let_network_response_empty();

            let.error = let_network_client_read(&let.network_client, &network_request);
            if (!let_error_exists(let.error)) {
                let.error = let_request(&network_request, &network_response);
            }

            auto error_report = let_error_report(let.error);
            switch (error_report.action) {
                case LET_ERROR_ACTION_FATAL:
                    let.executing = false;
                    let.accepting = false;
                    continue;
                case LET_ERROR_ACTION_IGNORE:
                    let.executing = false;
                    let.error = let_error_none();
                    break;
                case LET_ERROR_ACTION_CLOSE:
                    let.executing = false;
                    continue;
                case LET_ERROR_ACTION_REJECT:
                    network_response.type = LET_NETWORK_RESPONSE_TYPE_ERROR;
                    network_response.data.error = let.error;
                    let.error = let_error_none();
                    break;
                default:
                    break;
            }

            let.error = let_network_client_write(&let.network_client, network_response);
            if (let_error_exists(let.error)) {
                error_report = let_error_report(let.error);
                if (error_report.action == LET_ERROR_ACTION_FATAL) {
                    let.accepting = false;
                }

                let.executing = error_report.action != LET_ERROR_ACTION_CLOSE;
                continue;
            }

            if (network_request.type == LET_NETWORK_REQUEST_TYPE_CLOSE) {
                let_log_print(LET_LOG_LEVEL_INFO,
                              "Requested graceful closure from id:%u",
                              let.network_client.handle);

                let.executing = false;
            }
        }

        let_network_close(&let.network_client);
    }
}

void let_close([[maybe_unused]] int signal) {
    let.executing = false;
    let.accepting = false;
}

void let_cleanup(void) {
    let_network_close(&let.network_server);
    let_storage_wal_close(&let.storage_wal);
    let_account_list_free(let.account_list);
}

static let_error_t let_request(const let_network_request_t *network_request,
                               let_network_response_t *network_response) {
    auto request_error = let_error_none();

    const auto time_now = time(nullptr);
    if (time_now == -1) {
        return let_error_new(LET_ERROR_ID_STATE, LET_ERROR_STATE_INVALID_TIME);
    }

    switch (network_request->type) {
        case LET_NETWORK_REQUEST_TYPE_MAGIC:
            network_response->type = LET_NETWORK_RESPONSE_TYPE_MAGIC;
            break;
        case LET_NETWORK_REQUEST_TYPE_ADD_ACCOUNT: {
            network_response->type = LET_NETWORK_RESPONSE_TYPE_ADD_ACCOUNT;

            const auto credits = network_request->data.create_account.credits;
            const auto debits = network_request->data.create_account.debits;
            const auto flags = network_request->data.create_account.flags;

            auto storage_wal_entry = let_storage_wal_entry_new(
                network_request->data.create_account.wal_id,
                time_now,
                LET_STORAGE_WAL_ENTRY_TYPE_ADD_ACCOUNT);

            storage_wal_entry.data.add_account = (let_storage_wal_entry_add_account_t){
                .credits = credits,
                .debits = debits,
                .flags = flags
            };

            request_error = let_storage_wal_write(&let.storage_wal, &storage_wal_entry);
            if (let_error_exists(request_error)) {
                break;
            }

            const auto add_account = let_account_new(credits, debits, time_now, flags);

            let_u64_t account_id;
            request_error = let_state_add_account(&let.state, add_account, &account_id);
            if (let_error_exists(request_error)) {
                break;
            }

            network_response->data.add_account = account_id;
            break;
        }
        case LET_NETWORK_REQUEST_TYPE_MAKE_TRANSFER: {
            network_response->type = LET_NETWORK_RESPONSE_TYPE_OK;

            const auto from_id = network_request->data.make_transfer.from_id;
            const auto to_id = network_request->data.make_transfer.to_id;
            const auto amount = network_request->data.make_transfer.amount;

            request_error = let_guard_make_transfer(&let.guard, from_id, to_id, amount);
            if (let_error_exists(request_error)) {
                break;
            }

            auto storage_wal_entry = let_storage_wal_entry_new(
                network_request->data.make_transfer.wal_id,
                time_now,
                LET_STORAGE_WAL_ENTRY_TYPE_MAKE_TRANSFER);

            storage_wal_entry.data.make_transfer = (let_storage_wal_entry_make_transfer_t){
                .from_id = from_id,
                .to_id = to_id,
                .amount = amount
            };

            request_error = let_storage_wal_write(&let.storage_wal, &storage_wal_entry);
            if (let_error_exists(request_error)) {
                break;
            }

            request_error = let_state_make_transfer(&let.state, from_id, to_id, amount);
            break;
        }
        case LET_NETWORK_REQUEST_TYPE_GET_ACCOUNT: {
            network_response->type = LET_NETWORK_RESPONSE_TYPE_GET_ACCOUNT;

            let_account_t account;
            const auto account_id = network_request->data.get_account;

            request_error = let_account_list_get(let.account_list, account_id, &account);
            if (let_error_exists(request_error)) {
                break;
            }

            network_response->data.get_account.credits = account.credits;
            network_response->data.get_account.debits = account.debits;
            network_response->data.get_account.flags = account.flags;
            break;
        }
        case LET_NETWORK_REQUEST_TYPE_COUNT_ENTRIES:
            network_response->type = LET_NETWORK_RESPONSE_TYPE_COUNT_ENTRIES;
            network_response->data.count_entries = let.storage_wal.transactions;
            break;
        case LET_NETWORK_REQUEST_TYPE_UPDATE_ACCOUNT: {
            network_response->type = LET_NETWORK_RESPONSE_TYPE_OK;

            const auto account_id = network_request->data.update_account.account_id;
            const auto flags = network_request->data.update_account.flags;

            request_error = let_guard_update_account(&let.guard, account_id);
            if (let_error_exists(request_error)) {
                break;
            }

            auto storage_wal_entry = let_storage_wal_entry_new(
                network_request->data.update_account.wal_id,
                time_now,
                LET_STORAGE_WAL_ENTRY_TYPE_UPDATE_ACCOUNT);

            storage_wal_entry.data.update_account = (let_storage_wal_entry_update_account_t){
                .account_id = account_id,
                .flags = flags
            };

            request_error = let_storage_wal_write(&let.storage_wal, &storage_wal_entry);
            if (let_error_exists(request_error)) {
                break;
            }

            request_error = let_state_update_account(&let.state, account_id, flags);
            break;
        }
        case LET_NETWORK_REQUEST_TYPE_CLOSE:
            network_response->type = LET_NETWORK_RESPONSE_TYPE_OK;
            break;
    }

    return request_error;
}

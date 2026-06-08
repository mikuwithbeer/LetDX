#include "let/storage/wal.h"
#include "let/storage/crc.h"

#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

let_error_t let_storage_wal_init(let_storage_wal_t *storage_wal,
                                 let_state_t *state,
                                 const char *path) {
    storage_wal->state = state;

    auto file_descriptor = open(path, O_RDWR | O_CREAT | O_EXCL, 0644);
    if (file_descriptor >= 0) {
        storage_wal->file = fdopen(file_descriptor, "w+b");
        if (storage_wal->file == nullptr) {
            close(file_descriptor);
            return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_CREATE_FAILED);
        }

        auto write_result = fwrite(&LET_STORAGE_WAL_HEADER_MAGIC, sizeof(LET_STORAGE_WAL_HEADER_MAGIC), 1,
                                   storage_wal->file);
        if (write_result != 1) {
            fclose(storage_wal->file);
            return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_WRITE_FAILED);
        }

        write_result = fwrite(&LET_STORAGE_WAL_HEADER_VERSION, sizeof(LET_STORAGE_WAL_HEADER_VERSION), 1,
                              storage_wal->file);
        if (write_result != 1) {
            fclose(storage_wal->file);
            return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_WRITE_FAILED);
        }

        const auto sync_result = let_storage_wal_sync(storage_wal);
        if (let_error_exists(sync_result)) {
            fclose(storage_wal->file);
            return sync_result;
        }
    } else if (errno == EEXIST) {
        file_descriptor = open(path, O_RDWR);
        if (file_descriptor < 0) {
            return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_CREATE_FAILED);
        }

        storage_wal->file = fdopen(file_descriptor, "r+b");
        if (storage_wal->file == nullptr) {
            close(file_descriptor);
            return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_CREATE_FAILED);
        }

        typeof_unqual(LET_STORAGE_WAL_HEADER_MAGIC) wal_magic = 0;
        typeof_unqual(LET_STORAGE_WAL_HEADER_VERSION) wal_version = 0;

        auto read_result = fread(&wal_magic, sizeof(wal_magic), 1, storage_wal->file);
        if (read_result != 1) {
            fclose(storage_wal->file);
            return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_READ_FAILED);
        }

        read_result = fread(&wal_version, sizeof(wal_version), 1, storage_wal->file);
        if (read_result != 1) {
            fclose(storage_wal->file);
            return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_READ_FAILED);
        }

        if (wal_magic != LET_STORAGE_WAL_HEADER_MAGIC) {
            fclose(storage_wal->file);
            return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_INVALID_MAGIC);
        }

        if (wal_version != LET_STORAGE_WAL_HEADER_VERSION) {
            fclose(storage_wal->file);
            return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_INVALID_VERSION);
        }
    } else {
        return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_CREATE_FAILED);
    }

    const auto seek_result = fseek(storage_wal->file, 0, SEEK_END);
    if (seek_result != 0) {
        fclose(storage_wal->file);
        return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_SEEK_FAILED);
    }

    return let_error_none();
}

let_error_t let_storage_wal_replay(let_storage_wal_t *storage_wal) {
    auto seek_result = fseek(storage_wal->file, LET_STORAGE_WAL_HEADER_LENGTH, SEEK_SET);
    if (seek_result != 0) {
        return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_SEEK_FAILED);
    }

    storage_wal->transactions = 0;

    let_storage_wal_entry_safe_t safe_entry;
    while (fread(&safe_entry, sizeof(safe_entry), 1, storage_wal->file) == 1) {
        const auto crc_result = let_storage_crc32c(&safe_entry.entry, sizeof(let_storage_wal_entry_t));
        if (crc_result != safe_entry.checksum) {
            return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_CHECKSUM_MISMATCH);
        }

        switch (safe_entry.entry.header.type) {
            case LET_STORAGE_WAL_ENTRY_TYPE_ADD_ACCOUNT: {
                const auto add_account = safe_entry.entry.data.add_account;
                const auto account = let_account_new(0, add_account.balance, add_account.flags);

                let_u64_t account_id;
                const auto account_result = let_state_add_account(storage_wal->state, account, &account_id);

                if (let_error_exists(account_result)) {
                    return account_result;
                }

                break;
            }
            case LET_STORAGE_WAL_ENTRY_TYPE_MAKE_TRANSFER: {
                const auto make_transfer = safe_entry.entry.data.make_transfer;
                const auto transfer_result = let_state_make_transfer(
                    storage_wal->state,
                    make_transfer.from_id,
                    make_transfer.to_id,
                    make_transfer.amount);

                if (let_error_exists(transfer_result)) {
                    return transfer_result;
                }

                break;
            }
            case LET_STORAGE_WAL_ENTRY_TYPE_UPDATE_ACCOUNT: {
                const auto update_account = safe_entry.entry.data.update_account;
                const auto account_result = let_state_update_account(
                    storage_wal->state,
                    update_account.account_id,
                    update_account.flags);

                if (let_error_exists(account_result)) {
                    return account_result;
                }

                break;
            }
        }

        storage_wal->transactions++;
    }

    clearerr(storage_wal->file);

    seek_result = fseek(storage_wal->file, 0, SEEK_END);
    if (seek_result != 0) {
        return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_SEEK_FAILED);
    }

    return let_error_none();
}

let_error_t let_storage_wal_write(let_storage_wal_t *storage_wal,
                                  const let_storage_wal_entry_t *entry) {
    if (entry->header.id != storage_wal->transactions) {
        return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_NONCE_MISMATCH);
    }

    if (storage_wal->batch_count >= LET_STORAGE_WAL_BATCH_SIZE) {
        return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_BATCH_OVERFLOW);
    }

    const auto safe_entry = (let_storage_wal_entry_safe_t){
        .entry = *entry,
        .checksum = let_storage_crc32c(entry, sizeof(let_storage_wal_entry_t))
    };

    storage_wal->batch_buffer[storage_wal->batch_count++] = safe_entry;
    if (storage_wal->batch_count == LET_STORAGE_WAL_BATCH_SIZE) {
        const auto sync_result = let_storage_wal_sync(storage_wal);
        if (let_error_exists(sync_result)) {
            return sync_result;
        }
    }

    storage_wal->transactions++;
    return let_error_none();
}

let_error_t let_storage_wal_sync(let_storage_wal_t *storage_wal) {
    if (storage_wal->batch_count != 0) {
        const auto write_result = fwrite(storage_wal->batch_buffer,
                                         sizeof(let_storage_wal_entry_safe_t),
                                         storage_wal->batch_count, storage_wal->file);

        if (write_result != storage_wal->batch_count) {
            return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_WRITE_FAILED);
        }

        storage_wal->batch_count = 0;
    }

    const auto file_descriptor = fileno(storage_wal->file);
    auto sync_success = false;

#if defined(__APPLE__) && defined(__MACH__)
    sync_success = fcntl(file_descriptor, F_BARRIERFSYNC) == 0;
#else
    sync_success = fsync(file_descriptor) == 0;
#endif

    if (!sync_success) {
        return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_SYNC_FAILED);
    }

    return let_error_none();
}

void let_storage_wal_close(let_storage_wal_t *storage_wal) {
    if (storage_wal->file != nullptr) {
        fclose(storage_wal->file);
        storage_wal->file = nullptr;
    }
}

let_storage_wal_entry_t let_storage_wal_entry_new(const let_u64_t id,
                                                  const let_storage_wal_entry_type_t type) {
    const auto time_now = (let_time_t) time(nullptr);

    return (let_storage_wal_entry_t){
        .header = {
            .id = id,
            .timestamp = time_now,
            .type = type
        },
        .data = {}
    };
}

let_storage_wal_t let_storage_wal_empty(void) {
    return (let_storage_wal_t){};
}

#include "let/storage/wal.h"
#include "let/storage/crc.h"

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

let_error_t let_storage_wal_init(let_storage_wal_t *storage_wal,
                                 let_state_t *state,
                                 const char *path) {
    storage_wal->state = state;

    auto descriptor = open(path, O_RDWR | O_CREAT | O_EXCL, 0644);
    if (descriptor >= 0) {
        storage_wal->descriptor = descriptor;

        auto write_result = write(descriptor, &LET_STORAGE_WAL_HEADER_MAGIC, sizeof(LET_STORAGE_WAL_HEADER_MAGIC));
        if (write_result != sizeof(LET_STORAGE_WAL_HEADER_MAGIC)) {
            close(descriptor);
            storage_wal->descriptor = -1;
            return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_WRITE_FAILED);
        }

        write_result = write(descriptor, &LET_STORAGE_WAL_HEADER_VERSION, sizeof(LET_STORAGE_WAL_HEADER_VERSION));
        if (write_result != sizeof(LET_STORAGE_WAL_HEADER_VERSION)) {
            close(descriptor);
            storage_wal->descriptor = -1;
            return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_WRITE_FAILED);
        }
    } else if (errno == EEXIST) {
        descriptor = open(path, O_RDWR);
        if (descriptor < 0) {
            return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_OPEN_FAILED);
        }

        storage_wal->descriptor = descriptor;

        typeof_unqual(LET_STORAGE_WAL_HEADER_MAGIC) wal_magic = 0;
        typeof_unqual(LET_STORAGE_WAL_HEADER_VERSION) wal_version = 0;

        auto read_result = read(descriptor, &wal_magic, sizeof(wal_magic));
        if (read_result != sizeof(wal_magic)) {
            close(descriptor);
            storage_wal->descriptor = -1;
            return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_READ_FAILED);
        }

        read_result = read(descriptor, &wal_version, sizeof(wal_version));
        if (read_result != sizeof(wal_version)) {
            close(descriptor);
            storage_wal->descriptor = -1;
            return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_READ_FAILED);
        }

        if (wal_magic != LET_STORAGE_WAL_HEADER_MAGIC) {
            close(descriptor);
            storage_wal->descriptor = -1;
            return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_INVALID_MAGIC);
        }

        if (wal_version != LET_STORAGE_WAL_HEADER_VERSION) {
            close(descriptor);
            storage_wal->descriptor = -1;
            return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_INVALID_VERSION);
        }
    } else {
        return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_CREATE_FAILED);
    }

    const auto seek_result = lseek(storage_wal->descriptor, 0, SEEK_END);
    if (seek_result == (typeof(seek_result)) -1) {
        close(storage_wal->descriptor);
        storage_wal->descriptor = -1;
        return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_SEEK_FAILED);
    }

    return let_error_none();
}

let_error_t let_storage_wal_replay(let_storage_wal_t *storage_wal) {
    auto seek_result = lseek(storage_wal->descriptor, LET_STORAGE_WAL_HEADER_LENGTH, SEEK_SET);
    if (seek_result == (typeof(seek_result)) -1) {
        return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_SEEK_FAILED);
    }

    let_storage_wal_entry_safe_t safe_entry;
    let_i64_t bytes_read;

    for (storage_wal->transactions = 0
         ; (bytes_read = read(storage_wal->descriptor, &safe_entry, sizeof(safe_entry))) == sizeof(safe_entry)
         ; storage_wal->transactions++) {
        const auto crc_result = let_storage_crc32c(&safe_entry.entry, sizeof(let_storage_wal_entry_t));
        if (crc_result != safe_entry.checksum) {
            return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_CHECKSUM_MISMATCH);
        }

        switch (safe_entry.entry.header.type) {
            case LET_STORAGE_WAL_ENTRY_TYPE_ADD_ACCOUNT: {
                const auto add_account = safe_entry.entry.data.add_account;
                const auto account = let_account_new(
                    add_account.credits,
                    add_account.debits,
                    safe_entry.entry.header.timestamp,
                    add_account.flags);

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
            default:
                return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_INVALID_ENTRY_TYPE);
        }
    }

    if (bytes_read > 0 && (let_size_t) bytes_read < sizeof(safe_entry)) {
        return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_READ_FAILED);
    }

    if (bytes_read < 0) {
        return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_READ_FAILED);
    }

    seek_result = lseek(storage_wal->descriptor, 0, SEEK_END);
    if (seek_result == (typeof(seek_result)) -1) {
        return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_SEEK_FAILED);
    }

    return let_error_none();
}

let_error_t let_storage_wal_write(let_storage_wal_t *storage_wal,
                                  const let_storage_wal_entry_t *entry) {
    if (entry->header.id != storage_wal->transactions) {
        return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_NONCE_MISMATCH);
    }

    const auto safe_entry = (let_storage_wal_entry_safe_t){
        .entry = *entry,
        .checksum = let_storage_crc32c(entry, sizeof(let_storage_wal_entry_t))
    };

    const auto write_result = write(storage_wal->descriptor, &safe_entry, sizeof(safe_entry));
    if (write_result != sizeof(safe_entry)) {
        return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_WRITE_FAILED);
    }

    const auto sync_result = let_storage_wal_sync(storage_wal);
    if (let_error_exists(sync_result)) {
        return sync_result;
    }

    storage_wal->transactions++;
    return let_error_none();
}

let_error_t let_storage_wal_sync(const let_storage_wal_t *storage_wal) {
#if defined(__APPLE__) && defined(__MACH__)
    const bool sync_success = fcntl(storage_wal->descriptor, F_BARRIERFSYNC) == 0;
#else
    const bool sync_success = fsync(storage_wal->descriptor) == 0;
#endif

    if (!sync_success) {
        return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_SYNC_FAILED);
    }

    return let_error_none();
}

void let_storage_wal_close(let_storage_wal_t *storage_wal) {
    if (storage_wal->descriptor >= 0) {
        close(storage_wal->descriptor);
        storage_wal->descriptor = -1;
    }
}

let_storage_wal_entry_t let_storage_wal_entry_new(const let_u64_t id,
                                                  const let_time_t timestamp,
                                                  const let_storage_wal_entry_type_t type) {
    return (let_storage_wal_entry_t){
        .header = {
            .id = id,
            .timestamp = timestamp,
            .type = type
        },
        .data = {}
    };
}

let_storage_wal_t let_storage_wal_empty(void) {
    return (let_storage_wal_t){};
}

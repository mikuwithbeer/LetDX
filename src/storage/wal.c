#include "let/storage/wal.h"
#include "let/storage/crc.h"

#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

let_error_t let_storage_wal_init(let_storage_wal_t *storage_wal,
                                 let_state_t *state,
                                 const char *path) {
    storage_wal->file = nullptr;
    storage_wal->state = state;
    storage_wal->transactions = 0;

    auto file_descriptor = open(path, O_RDWR | O_CREAT | O_EXCL, 0644);
    if (file_descriptor >= 0) {
        storage_wal->file = fdopen(file_descriptor, "w+b");
        if (storage_wal->file == nullptr) {
            close(file_descriptor);
            return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_CREATE_FAILED);
        }

        auto write_result = fwrite(&LET_STORAGE_WAL_MAGIC, sizeof(LET_STORAGE_WAL_MAGIC), 1, storage_wal->file);
        if (write_result != 1) {
            fclose(storage_wal->file);
            return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_WRITE_FAILED);
        }

        write_result = fwrite(&LET_STORAGE_WAL_VERSION, sizeof(LET_STORAGE_WAL_VERSION), 1, storage_wal->file);
        if (write_result != 1) {
            fclose(storage_wal->file);
            return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_WRITE_FAILED);
        }

        const auto sync_result = fsync(fileno(storage_wal->file));
        if (sync_result != 0) {
            fclose(storage_wal->file);
            return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_SYNC_FAILED);
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

        typeof_unqual(LET_STORAGE_WAL_MAGIC) wal_magic = 0;
        typeof_unqual(LET_STORAGE_WAL_VERSION) wal_version = 0;

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

        if (wal_magic != LET_STORAGE_WAL_MAGIC) {
            fclose(storage_wal->file);
            return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_INVALID_MAGIC);
        }

        if (wal_version != LET_STORAGE_WAL_VERSION) {
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


let_error_t let_storage_wal_write(let_storage_wal_t *storage_wal,
                                  const let_storage_wal_entry_t *entry) {
    if (entry->header.id != storage_wal->transactions) {
        return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_NONCE_MISMATCH);
    }

    const let_storage_wal_entry_safe_t safe_entry = {
        .entry = *entry,
        .checksum = let_storage_crc32c(entry, sizeof(let_storage_wal_entry_t))
    };

    const auto write_result = fwrite(&safe_entry, sizeof(safe_entry), 1, storage_wal->file);
    if (write_result != 1) {
        return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_WRITE_FAILED);
    }

    const auto sync_result = fsync(fileno(storage_wal->file));
    if (sync_result != 0) {
        return let_error_new(LET_ERROR_ID_STORAGE, LET_ERROR_STORAGE_WAL_SYNC_FAILED);
    }

    storage_wal->transactions++;
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
        .data = {0}
    };
}

let_storage_wal_t let_storage_wal_empty(void) {
    return (let_storage_wal_t){0};
}

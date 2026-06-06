#ifndef LET_STORAGE_WAL_H
#define LET_STORAGE_WAL_H

#include "let/state.h"

#include <stdio.h>

constexpr let_u64_t LET_STORAGE_WAL_HEADER_MAGIC = 0x4C'41'57'5F'5F'54'45'4C;
constexpr let_u16_t LET_STORAGE_WAL_HEADER_VERSION = 1;

constexpr let_size_t LET_STORAGE_WAL_HEADER_LENGTH = sizeof(LET_STORAGE_WAL_HEADER_MAGIC)
                                                     + sizeof(LET_STORAGE_WAL_HEADER_VERSION);

constexpr let_u16_t LET_STORAGE_WAL_BATCH_SIZE = 1 << 6;

#pragma pack(push, 1)

typedef enum : let_u8_t {
    LET_STORAGE_WAL_ENTRY_TYPE_ADD_ACCOUNT = 1,
    LET_STORAGE_WAL_ENTRY_TYPE_MAKE_TRANSFER,
} let_storage_wal_entry_type_t;

typedef struct {
    let_u64_t id;
    let_time_t timestamp;
    let_storage_wal_entry_type_t type;
} let_storage_wal_entry_header_t;

typedef struct {
    let_u128_t balance;
    let_u8_t flags;
} let_storage_wal_entry_add_account_t;

typedef struct {
    let_u64_t from_id;
    let_u64_t to_id;
    let_u128_t amount;
} let_storage_wal_entry_make_transfer_t;

typedef union {
    let_storage_wal_entry_add_account_t add_account;
    let_storage_wal_entry_make_transfer_t make_transfer;
} let_storage_wal_entry_data_t;

typedef struct {
    let_storage_wal_entry_header_t header;
    let_storage_wal_entry_data_t data;
} let_storage_wal_entry_t;

typedef struct {
    let_storage_wal_entry_t entry;
    let_u32_t checksum;
} let_storage_wal_entry_safe_t;

#pragma pack(pop)

typedef struct {
    FILE *file;
    let_state_t *state;
    let_u64_t transactions;

    let_storage_wal_entry_safe_t batch_buffer[LET_STORAGE_WAL_BATCH_SIZE];
    typeof_unqual(LET_STORAGE_WAL_BATCH_SIZE) batch_count;
} let_storage_wal_t;

[[nodiscard]] let_storage_wal_entry_t let_storage_wal_entry_new(let_u64_t id,
                                                                let_storage_wal_entry_type_t type);

[[nodiscard]] let_storage_wal_t let_storage_wal_empty(void);

let_error_t let_storage_wal_init(let_storage_wal_t *storage_wal,
                                 let_state_t *state,
                                 const char *path);

let_error_t let_storage_wal_replay(let_storage_wal_t *storage_wal);

let_error_t let_storage_wal_write(let_storage_wal_t *storage_wal,
                                  const let_storage_wal_entry_t *entry);

let_error_t let_storage_wal_sync(let_storage_wal_t *storage_wal);

void let_storage_wal_close(let_storage_wal_t *storage_wal);

#endif //LET_STORAGE_WAL_H

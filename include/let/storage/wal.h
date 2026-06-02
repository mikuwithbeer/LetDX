#ifndef LET_STORAGE_WAL_H
#define LET_STORAGE_WAL_H

#include "let/common.h"
#include "let/error.h"

#include <stdio.h>

constexpr let_u64_t LET_STORAGE_WAL_MAGIC = 0x4C'41'57'5F'5F'54'45'4C;
constexpr let_u16_t LET_STORAGE_WAL_VERSION = 1;

typedef struct {
    FILE *file;
    let_u64_t transactions;
} let_storage_wal_t;

typedef enum [[nodiscard]] : let_u8_t {
    LET_STORAGE_WAL_ENTRY_TYPE_ADD_ACCOUNT = 1,
    LET_STORAGE_WAL_ENTRY_TYPE_MAKE_TRANSFER,
} let_storage_wal_entry_type_t;

#pragma pack(push, 1)

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

let_error_t let_storage_wal_init(let_storage_wal_t *storage_wal,
                                 const char *path);

let_error_t let_storage_wal_write(let_storage_wal_t *storage_wal,
                                  const let_storage_wal_entry_t *entry);

void let_storage_wal_close(let_storage_wal_t *storage_wal);

#endif //LET_STORAGE_WAL_H

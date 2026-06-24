#ifndef LET_STORAGE_WAL_H
#define LET_STORAGE_WAL_H

/**
 * @file wal.h
 * @brief The Write-Ahead Log (WAL) interface.
 *
 * The WAL is a critical part for ensuring data durability and consistency.
 * It records all transactions before they are applied to the in-memory state,
 * allowing for recovery in case of crashes or unexpected shutdowns.
 *
 * @warning Do not modify the structures defined in this file without bumping the version number.
 */

#include "let/state.h"

// -----------------------------------------------------------------------------
// Compile-Time Constants
// -----------------------------------------------------------------------------

// Resolves to the string "LET__WAL" when read in Little Endian.
constexpr let_u64_t LET_STORAGE_WAL_HEADER_MAGIC = 0x4C'41'57'5F'5F'54'45'4C;

// Bump this version immediately if ANY changes are made to the structs below.
constexpr let_u16_t LET_STORAGE_WAL_HEADER_VERSION = 1;

constexpr let_size_t LET_STORAGE_WAL_HEADER_LENGTH = sizeof(let_u64_t) + sizeof(let_u16_t); // Magic + Version

// -----------------------------------------------------------------------------
// Data Structures
// -----------------------------------------------------------------------------

/**
 * @brief The seek modes for repositioning the file pointer in the WAL.
 */
typedef enum : let_u8_t {
    LET_STORAGE_WAL_SEEK_START, // SEEK_SET
    LET_STORAGE_WAL_SEEK_CURRENT, // SEEK_CUR
    LET_STORAGE_WAL_SEEK_END, // SEEK_END
} let_storage_wal_seek_t;

#pragma pack(push, 1) // Ensure no padding is added by the compiler

/**
 * @brief The types of transactions that can be recorded in the WAL.
 */
typedef enum : let_u8_t {
    LET_STORAGE_WAL_ENTRY_TYPE_ADD_ACCOUNT = 1,
    LET_STORAGE_WAL_ENTRY_TYPE_MAKE_TRANSFER,
    LET_STORAGE_WAL_ENTRY_TYPE_UPDATE_ACCOUNT,
} let_storage_wal_entry_type_t;

/**
 * @brief The header for a WAL entry, containing metadata about the transaction.
 */
typedef struct {
    let_u64_t id; // The sequential ID of the transaction
    let_time_t timestamp; // The time the transaction was created
    let_storage_wal_entry_type_t type; // The type of transaction being recorded
} let_storage_wal_entry_header_t;

/**
 * @brief The specific data for an add account transaction.
 */
typedef struct {
    let_u128_t credits;
    let_u128_t debits;
    let_u8_t flags;
} let_storage_wal_entry_add_account_t;

/**
 * @brief The specific data for a make transfer transaction.
 */
typedef struct {
    let_u64_t from_id;
    let_u64_t to_id;
    let_u128_t amount;
} let_storage_wal_entry_make_transfer_t;

/**
 * @brief The specific data for an update account transaction.
 */
typedef struct {
    let_u64_t account_id;
    let_u8_t flags;
} let_storage_wal_entry_update_account_t;

/**
 * @brief A union representing the data associated with different types of WAL entries.
 */
typedef union {
    let_storage_wal_entry_add_account_t add_account;
    let_storage_wal_entry_make_transfer_t make_transfer;
    let_storage_wal_entry_update_account_t update_account;
} let_storage_wal_entry_data_t;

/**
 * @brief The complete representation of a WAL entry, including its header and data.
 *
 * @warning This structure is not safe for direct disk storage, use `let_storage_wal_entry_safe_t` for disk storage.
 */
typedef struct {
    let_storage_wal_entry_header_t header;
    let_storage_wal_entry_data_t data;
} let_storage_wal_entry_t;

/**
 * @brief A safe representation of a WAL entry for disk storage, including a checksum for integrity verification.
 */
typedef struct {
    let_storage_wal_entry_t entry;
    let_u32_t checksum; // CRC32C checksum for integrity verification
} let_storage_wal_entry_safe_t;

#pragma pack(pop) // Restore standard compiler padding rules

/**
 * @brief The Write-Ahead Log (WAL) structure, managing the state and file descriptor for the log.
 */
typedef struct {
    let_state_t *state; // Weak pointer to the state machine
    let_u64_t transactions;
    let_i32_t descriptor;
} let_storage_wal_t;

// -----------------------------------------------------------------------------
// Function Declarations
// -----------------------------------------------------------------------------

/**
 * @brief Constructs a new WAL entry with the specified parameters.
 */
[[nodiscard]] let_storage_wal_entry_t let_storage_wal_entry_new(let_u64_t id,
                                                                let_time_t timestamp,
                                                                let_storage_wal_entry_type_t type);

/**
 * @brief Returns an empty, zero-initialized WAL struct.
 */
[[nodiscard]] let_storage_wal_t let_storage_wal_empty(void);

/**
 * @brief Initializes the WAL with a given state machine and file path.
 *
 * @note Creates the file if it does not exist.
 */
let_error_t let_storage_wal_init(let_storage_wal_t *storage_wal,
                                 let_state_t *state,
                                 const char *path);

/**
 * @brief Replays the WAL to rebuild the in-memory state machine.
 */
let_error_t let_storage_wal_replay(let_storage_wal_t *storage_wal,
                                   bool truncate_on_failure);

/**
 * @brief Writes a new entry to the WAL, ensuring it is persisted to disk.
 *
 * @warning The entry ID must match the current transaction count to maintain order.
 */
let_error_t let_storage_wal_write(let_storage_wal_t *storage_wal,
                                  const let_storage_wal_entry_t *entry);

/**
 * @brief Cleanly releases the file descriptor.
 */
void let_storage_wal_close(let_storage_wal_t *storage_wal);

#endif //LET_STORAGE_WAL_H

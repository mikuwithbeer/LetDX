#ifndef LET_NETWORK_RESPONSE_H
#define LET_NETWORK_RESPONSE_H

#include "let/common.h"
#include "let/error.h"

constexpr let_u8_t LET_NETWORK_RESPONSE_MAGIC[3] = {0x4C, 0x45, 0x54};
constexpr let_u8_t LET_NETWORK_RESPONSE_SIZE_MAX = 128;

typedef enum : let_u8_t {
    LET_NETWORK_RESPONSE_TYPE_MAGIC,
    LET_NETWORK_RESPONSE_TYPE_ADD_ACCOUNT,
    LET_NETWORK_RESPONSE_TYPE_GET_BALANCE,
    LET_NETWORK_RESPONSE_TYPE_COUNT_ENTRIES,
    LET_NETWORK_RESPONSE_TYPE_OK,
    LET_NETWORK_RESPONSE_TYPE_ERROR,
} let_network_response_type_t;

typedef union {
    let_u128_t get_balance;
    let_u64_t add_account;
    let_u64_t count_entries;

    let_error_t error;
} let_network_response_data_t;

typedef struct {
    let_network_response_type_t type;
    let_network_response_data_t data;
} let_network_response_t;

[[nodiscard]] let_network_response_t let_network_response_empty(void);

[[nodiscard]] let_size_t let_network_response_to_bytes(const let_network_response_t *response,
                                                       let_u8_t *output);

#endif //LET_NETWORK_RESPONSE_H

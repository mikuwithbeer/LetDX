#ifndef LET_NETWORK_RESPONSE_H
#define LET_NETWORK_RESPONSE_H

#include "let/common.h"
#include "let/error.h"

constexpr let_u8_t LET_NETWORK_RESPONSE_MAGIC[3] = {0x4C, 0x45, 0x54};

typedef enum : let_u8_t {
    LET_NETWORK_RESPONSE_ID_MAGIC,
    LET_NETWORK_RESPONSE_ID_ADD_ACCOUNT,
    LET_NETWORK_RESPONSE_ID_GET_BALANCE,
    LET_NETWORK_RESPONSE_ID_OK,
    LET_NETWORK_RESPONSE_ID_ERROR,
} let_network_response_id_t;

typedef union {
    let_u128_t get_balance;
    let_u64_t add_account;
    let_error_t error;
} let_network_response_data_t;

typedef struct {
    let_network_response_id_t id;
    let_network_response_data_t data;
} let_network_response_t;

[[nodiscard]] let_network_response_t let_network_response_empty(void);

[[nodiscard]] let_size_t let_network_response_to_bytes(const let_network_response_t *response,
                                                       let_u8_t *output);

#endif //LET_NETWORK_RESPONSE_H

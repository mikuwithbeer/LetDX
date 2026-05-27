#ifndef LET_NETWORK_RESPONSE_H
#define LET_NETWORK_RESPONSE_H

#include "let/common.h"

constexpr let_u8_t LET_NETWORK_RESPONSE_MAGIC[4] = {0x4C, 0x65, 0x74, 0x2E};

typedef enum : let_u8_t {
    LET_NETWORK_RESPONSE_ID_MAGIC,
    LET_NETWORK_RESPONSE_ID_ADD_ACCOUNT,
    LET_NETWORK_RESPONSE_ID_OK,
    LET_NETWORK_RESPONSE_ID_ERROR,
} let_network_response_id_t;

typedef union {
    let_u64_t add_account;

    struct {
        let_u8_t namespace;
        let_u8_t error;
    } error;
} let_network_response_data_t;

typedef struct {
    let_network_response_id_t id;
    let_network_response_data_t data;
} let_network_response_t;

[[nodiscard]] let_network_response_t let_network_response_new(void);

[[nodiscard]] let_size_t let_network_response_to_bytes(const let_network_response_t *response,
                                                       let_u8_t *output);

#endif //LET_NETWORK_RESPONSE_H

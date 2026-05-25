#ifndef LET_NETWORK_RESPONSE_H
#define LET_NETWORK_RESPONSE_H

#include "let/common.h"

typedef enum : let_u8_t {
    LET_NETWORK_RESPONSE_ID_OK,
    LET_NETWORK_RESPONSE_ID_ERROR,
} let_network_response_id_t;

typedef union {
} let_network_response_data_t;

typedef struct {
    let_network_response_id_t id;
    let_network_response_data_t data;
} let_network_response_t;

#endif //LET_NETWORK_RESPONSE_H

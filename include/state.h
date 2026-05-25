#ifndef LET_STATE_H
#define LET_STATE_H

#include "account.h"

typedef enum [[nodiscard]] : let_u8_t {
    LET_STATE_ERROR_NONE,
    LET_STATE_ERROR_OUT_OF_MEMORY
} let_state_error_t;

typedef struct {
    let_account_list_t *account_list;
} let_state_t;

[[nodiscard]] let_state_t *let_state_new(let_account_list_t *account_list);

let_state_error_t let_state_add_account(const let_state_t *state,
                                        let_account_t account,
                                        let_u64_t *account_id);

let_state_error_t let_state_make_transfer(const let_state_t *state,
                                          let_u64_t from_account_id,
                                          let_u64_t to_account_id,
                                          let_u128_t amount);

void let_state_free(let_state_t *state);

#endif //LET_STATE_H

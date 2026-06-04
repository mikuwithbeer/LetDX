#ifndef LET_STATE_H
#define LET_STATE_H

#include "let/account.h"

typedef struct {
    let_account_list_t *account_list;
} let_state_t;

[[nodiscard]] let_state_t let_state_empty(void);

let_error_t let_state_init(let_state_t *state,
                           let_account_list_t *account_list);

let_error_t let_state_add_account(const let_state_t *state,
                                  let_account_t account,
                                  let_u64_t *account_id);

let_error_t let_state_make_transfer(const let_state_t *state,
                                    let_u64_t from_account_id,
                                    let_u64_t to_account_id,
                                    let_u128_t amount);

#endif //LET_STATE_H

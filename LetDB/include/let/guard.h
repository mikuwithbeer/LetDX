#ifndef LET_GUARD_H
#define LET_GUARD_H

#include "let/state.h"

typedef struct {
    let_state_t *state;
} let_guard_t;

[[nodiscard]] let_guard_t let_guard_empty(void);

let_error_t let_guard_init(let_guard_t *guard,
                           let_state_t *state);

let_error_t let_guard_make_transfer(const let_guard_t *guard,
                                    let_u64_t from_account_id,
                                    let_u64_t to_account_id,
                                    let_u128_t amount);

let_error_t let_guard_update_account(const let_guard_t *guard,
                                     let_u64_t account_id);

#endif //LET_GUARD_H

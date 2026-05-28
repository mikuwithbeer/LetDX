#ifndef LET_GUARD_H
#define LET_GUARD_H

#include "let/state.h"

typedef struct {
    let_state_t *state;
} let_guard_t;

[[nodiscard]] let_guard_t *let_guard_new(let_state_t *state);

let_error_t let_guard_make_transfer(const let_guard_t *guard,
                                    let_u64_t from_account_id,
                                    let_u64_t to_account_id,
                                    let_u128_t amount);

void let_guard_free(let_guard_t *guard);

#endif //LET_GUARD_H

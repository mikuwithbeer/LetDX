#ifndef LET_GUARD_H
#define LET_GUARD_H

#include "let/state.h"

typedef enum [[nodiscard]] : let_u8_t {
    LET_GUARD_ERROR_NONE,
    LET_GUARD_ERROR_SAME_ACCOUNT,
    LET_GUARD_ERROR_ACCOUNT_NOT_FOUND,
    LET_GUARD_ERROR_INSUFFICIENT_FUNDS,
    LET_GUARD_ERROR_MAXIMUM_TRANSACTIONS,
} let_guard_error_t;

typedef struct {
    let_state_t *state;
} let_guard_t;

[[nodiscard]] let_guard_t *let_guard_new(let_state_t *state);

let_guard_error_t let_guard_make_transfer(const let_guard_t *guard,
                                          let_u64_t from_account_id,
                                          let_u64_t to_account_id,
                                          let_u128_t amount);

void let_guard_free(let_guard_t *guard);

#endif //LET_GUARD_H

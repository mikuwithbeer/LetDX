#ifndef LET_ACCOUNT_H
#define LET_ACCOUNT_H

#include "let/common.h"
#include "let/error.h"

constexpr let_u64_t LET_ACCOUNT_CAPACITY_DEFAULT = 1 << 10;

typedef enum : let_u8_t {
    LET_ACCOUNT_FLAG_NONE = 0,
    LET_ACCOUNT_FLAG_CAN_SEND = 1 << 0,
    LET_ACCOUNT_FLAG_CAN_RECEIVE = 1 << 1
} let_account_flag_t;

typedef struct {
    let_u128_t credits;
    let_u128_t debits;

    let_u64_t transactions;
    let_time_t created_at;
    let_time_t updated_at;

    let_account_flag_t flags;
} let_account_t;

typedef struct {
    let_u64_t capacity;
    let_u64_t length;

    let_account_t *accounts;
} let_account_list_t;

[[nodiscard]] let_account_t let_account_new(let_u128_t credits,
                                            let_u128_t debits,
                                            let_time_t created_at,
                                            let_account_flag_t flags);

[[nodiscard]] let_account_list_t *let_account_list_new(void);

let_error_t let_account_list_add(let_account_list_t *account_list,
                                 let_account_t account);

let_error_t let_account_list_get(const let_account_list_t *account_list,
                                 let_u64_t account_id,
                                 let_account_t *account);

let_error_t let_account_list_update(const let_account_list_t *account_list,
                                    let_u64_t account_id,
                                    let_account_flag_t flags);

void let_account_list_free(let_account_list_t *account_list);

#endif //LET_ACCOUNT_H

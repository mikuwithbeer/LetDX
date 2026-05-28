#include "let/error.h"

let_error_t let_error_new(const let_error_id_t id,
                          const let_u8_t code) {
    return (let_error_t){
        .id = id,
        .code = code
    };
}

let_error_t let_error_none(void) {
    return (let_error_t){
        .id = LET_ERROR_ID_NONE,
        .code = 0
    };
}

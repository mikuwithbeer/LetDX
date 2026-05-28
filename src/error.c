#include "let/error.h"

let_error_t let_error_new(const let_error_id_t id,
                          const let_u8_t code) {
    return (let_error_t){
        .id = id,
        .error = code
    };
}

let_error_t let_error_none(void) {
    return (let_error_t){
        .id = LET_ERROR_ID_NONE,
        .error = 0
    };
}

let_error_code_t let_error_code(const let_error_t error) {
    if (error.id == LET_ERROR_ID_NONE) {
        return 0;
    }

    let_error_code_t code = 1000;
    code *= error.id;
    code += error.error;

    return code;
}

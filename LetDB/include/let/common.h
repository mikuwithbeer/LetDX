#ifndef LET_COMMON_H
#define LET_COMMON_H

#include <stdint.h>
#include <time.h>

typedef int8_t let_i8_t;
typedef int16_t let_i16_t;
typedef int32_t let_i32_t;
typedef int64_t let_i64_t;
typedef _BitInt(128) let_i128_t;

typedef uint8_t let_u8_t;
typedef uint16_t let_u16_t;
typedef uint32_t let_u32_t;
typedef uint64_t let_u64_t;
typedef unsigned _BitInt(128) let_u128_t;

typedef size_t let_size_t;
typedef time_t let_time_t;

typedef void *let_pointer_t;

constexpr let_u8_t LET_U8_MAX = 0xFF;
constexpr let_u16_t LET_U16_MAX = 0xFFFF;
constexpr let_u32_t LET_U32_MAX = 0xFFFFFFFF;
constexpr let_u64_t LET_U64_MAX = 0xFFFFFFFFFFFFFFFF;
constexpr let_u128_t LET_U128_MAX = ~(let_u128_t) 0;

#endif //LET_COMMON_H

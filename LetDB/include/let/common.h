#ifndef LET_COMMON_H
#define LET_COMMON_H

/**
 * @file common.h
 * @brief Common type definitions and constants used throughout the project.
 */

#include <stdint.h>
#include <time.h>

typedef int8_t let_i8_t; // Signed 8-bit integer
typedef int16_t let_i16_t; // Signed 16-bit integer
typedef int32_t let_i32_t; // Signed 32-bit integer
typedef int64_t let_i64_t; // Signed 64-bit integer
typedef _BitInt(128) let_i128_t; // Signed 128-bit integer

typedef uint8_t let_u8_t; // Unsigned 8-bit integer
typedef uint16_t let_u16_t; // Unsigned 16-bit integer
typedef uint32_t let_u32_t; // Unsigned 32-bit integer
typedef uint64_t let_u64_t; // Unsigned 64-bit integer
typedef unsigned _BitInt(128) let_u128_t; // Unsigned 128-bit integer

typedef size_t let_size_t; // Size type for memory allocation and array indexing
typedef time_t let_time_t; // Time type for timestamps

constexpr let_u8_t LET_U8_MAX = 0xFF; // Maximum value for an unsigned 8-bit integer
constexpr let_u16_t LET_U16_MAX = 0xFFFF; // Maximum value for an unsigned 16-bit integer
constexpr let_u32_t LET_U32_MAX = 0xFFFFFFFF; // Maximum value for an unsigned 32-bit integer
constexpr let_u64_t LET_U64_MAX = 0xFFFFFFFFFFFFFFFF; // Maximum value for an unsigned 64-bit integer
constexpr let_u128_t LET_U128_MAX = ~(let_u128_t) 0; // Maximum value for an unsigned 128-bit integer

#endif //LET_COMMON_H

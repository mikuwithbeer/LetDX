/**
 * @file crc.c
 * @brief The CRC checksum implementation.
 */

#include "let/storage/crc.h"

// -----------------------------------------------------------------------------
// Function Implementations
// -----------------------------------------------------------------------------

let_u32_t let_storage_crc32c(const void *bytes,
                             let_size_t length) {
    let_u32_t crc = 0x00000000u;

    // Return a zero checksum for null or empty input.
    if (!bytes || length == 0) {
        return crc;
    }

    crc = 0xFFFFFFFFu;
    auto pointer = (const let_u8_t *) bytes;

    // Process the input in 4-byte chunks for efficiency.
    for (; length >= 4; length -= 4) {
        // Process each byte of the 4-byte chunk.
        crc = crc >> 8 ^ LET_STORAGE_CRC_TABLE_CRC32C[(crc ^ *pointer++) & 0xFF];
        crc = crc >> 8 ^ LET_STORAGE_CRC_TABLE_CRC32C[(crc ^ *pointer++) & 0xFF];
        crc = crc >> 8 ^ LET_STORAGE_CRC_TABLE_CRC32C[(crc ^ *pointer++) & 0xFF];
        crc = crc >> 8 ^ LET_STORAGE_CRC_TABLE_CRC32C[(crc ^ *pointer++) & 0xFF];
    }

    // Clean trailing bytes.
    while (length--) {
        crc = crc >> 8 ^ LET_STORAGE_CRC_TABLE_CRC32C[(crc ^ *pointer++) & 0xFF];
    }

    return crc ^ 0xFFFFFFFFu; // Invert all bits to yield the true mathematical checksum
}

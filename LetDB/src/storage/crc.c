#include "let/storage/crc.h"

let_u32_t let_storage_crc32c(const void *bytes,
                             let_size_t length) {
    if (!bytes || length == 0) {
        return 0x00000000u;
    }

    auto pointer = (const let_u8_t *) bytes;

    let_u32_t crc;
    for (crc = 0xFFFFFFFFu; length >= 4; length -= 4) {
        crc = crc >> 8 ^ LET_STORAGE_CRC_TABLE_CRC32C[(crc ^ *pointer++) & 0xFF];
        crc = crc >> 8 ^ LET_STORAGE_CRC_TABLE_CRC32C[(crc ^ *pointer++) & 0xFF];
        crc = crc >> 8 ^ LET_STORAGE_CRC_TABLE_CRC32C[(crc ^ *pointer++) & 0xFF];
        crc = crc >> 8 ^ LET_STORAGE_CRC_TABLE_CRC32C[(crc ^ *pointer++) & 0xFF];
    }

    while (length--) {
        crc = crc >> 8 ^ LET_STORAGE_CRC_TABLE_CRC32C[(crc ^ *pointer++) & 0xFF];
    }

    return crc ^ 0xFFFFFFFFu;
}

#include "let/storage/crc.h"

let_u32_t let_storage_crc32c(const void *bytes,
                             const let_size_t length) {
    const auto pointer = (const let_u8_t *) bytes;
    auto crc = 0xFFFFFFFFu;

    for (let_size_t index = 0; index < length; index++) {
        const auto table_index = (let_u8_t) ((crc ^ pointer[index]) & 0xFF);
        crc = crc >> 8 ^ LET_STORAGE_CRC_TABLE_CRC32C[table_index];
    }

    return crc ^ 0xFFFFFFFFu;
}

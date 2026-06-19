#include "let/storage/crc.h"

#include <assert.h>
#include <string.h>

static void test_single_byte(void) {
    constexpr let_u8_t byte = 0x00;
    const auto crc = let_storage_crc32c(&byte, 1);
    assert(crc != 0);
}

static void test_empty_string(void) {
    const auto crc = let_storage_crc32c("", 0);
    assert(crc == 0x00000000u);
}

static void test_known_string_1(void) {
    constexpr char string[] = "LetDB";
    const auto crc = let_storage_crc32c(string, strlen(string));

    // CRC-32/ISCSI = 0x54651153
    assert(crc == 0x54651153u);
}

static void test_known_string_2(void) {
    constexpr char string[] = "123456789";
    const auto crc = let_storage_crc32c(string, strlen(string));

    // CRC-32/ISCSI = 0xE3069283
    assert(crc == 0xE3069283u);
}

static void test_binary_data(void) {
    constexpr let_u8_t data[] = {0x0A, 0xFF, 0x31, 0x69, 0x42, 0x00};

    const auto crc1 = let_storage_crc32c(data, sizeof(data));
    const auto crc2 = let_storage_crc32c(data, sizeof(data));

    assert(crc1 == crc2);
}

static void test_cleanup(void) {
    assert(true);
}

int main(void) {
    test_single_byte();
    test_empty_string();
    test_known_string_1();
    test_known_string_2();
    test_binary_data();

    test_cleanup();
    return 0;
}

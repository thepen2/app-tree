#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include <cmocka.h>

#include "transaction/serialize.h"
#include "transaction/deserialize.h"
#include "transaction/types.h"

#include "common/base58.h"
#include "common/buffer.h"
#include "common/write.h"

#include <stddef.h>  // size_t

void write_u16_le(uint8_t *ptr, size_t offset, uint16_t value) {
    ptr[offset + 0] = (uint8_t)(value >> 0);
    ptr[offset + 1] = (uint8_t)(value >> 8);
}

void write_u32_le(uint8_t *ptr, size_t offset, uint32_t value) {
    ptr[offset + 0] = (uint8_t)(value >> 0);
    ptr[offset + 1] = (uint8_t)(value >> 8);
    ptr[offset + 2] = (uint8_t)(value >> 16);
    ptr[offset + 3] = (uint8_t)(value >> 24);
}

void write_u64_le(uint8_t *ptr, size_t offset, uint64_t value) {
    ptr[offset + 0] = (uint8_t)(value >> 0);
    ptr[offset + 1] = (uint8_t)(value >> 8);
    ptr[offset + 2] = (uint8_t)(value >> 16);
    ptr[offset + 3] = (uint8_t)(value >> 24);
    ptr[offset + 4] = (uint8_t)(value >> 32);
    ptr[offset + 5] = (uint8_t)(value >> 40);
    ptr[offset + 6] = (uint8_t)(value >> 48);
    ptr[offset + 7] = (uint8_t)(value >> 56);
}

static void test_tx_deserialization_1(void **state) {
    (void) state;

    transaction_t tx;
    // clang-format off

    uint8_t raw_tx[] = {
        // timestamp (10)
        0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30,
        // colon
        0x3a,
        // tempPubKey (33)
        0x03,   // IF THIS BYTE IS 0x01, REST IS TEXT STRING
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
        // colon
        0x3a,
        // blockchain (8)
        0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
        // colon
        0x3a,
        // startup addr (34)
        0x48, 0x4c, 0x72, 0x50, 0x35, 0x4b, 0x48, 0x41, 0x48, 0x33, 
        0x33, 0x43, 0x6b, 0x78, 0x66, 0x36, 0x76, 0x46, 0x39, 0x67, 
        0x35, 0x69, 0x4a, 0x73, 0x65, 0x4c, 0x35, 0x78, 0x39, 0x67, 
        0x34, 0x55, 0x46, 0x63};

    buffer_t buf = {.ptr = raw_tx, .size = sizeof(raw_tx), .offset = 0};

    parser_status_e status = transaction_deserialize_1(&buf, &tx);

    assert_int_equal(status, PARSING_OK);

    uint8_t raw_tx2[] = {
        // timestamp (10)
        0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30,
        // colon
        0x3a,
        // tempPubKey (33)
        0x01,   // IF THIS BYTE IS 0x01, REST IS TEXT STRING
        0x70, 0x77, 0x64, 0x20, 0x63, 0x68, 0x61, 0x6e, 
        0x67, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        // colon
        0x3a,
        // blockchain (8)
        0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
        // colon
        0x3a,
        // startup addr (34)
        0x48, 0x4c, 0x72, 0x50, 0x35, 0x4b, 0x48, 0x41, 0x48, 0x33, 
        0x33, 0x43, 0x6b, 0x78, 0x66, 0x36, 0x76, 0x46, 0x39, 0x67, 
        0x35, 0x69, 0x4a, 0x73, 0x65, 0x4c, 0x35, 0x78, 0x39, 0x67, 
        0x34, 0x55, 0x46, 0x63};

    buffer_t buf2 = {.ptr = raw_tx2, .size = sizeof(raw_tx2), .offset = 0};

    parser_status_e status2 = transaction_deserialize_1(&buf2, &tx);

    assert_int_equal(status2, PARSING_OK);


}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_tx_deserialization_1)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}

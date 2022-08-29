#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <cmocka.h>

#include "common/base58.h"

static void test_is_base58(void **state) {
   (void) state;

    unsigned char base58_chars[] = {
    '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',  //
    'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',  //
    'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'm',  //
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};

    assert_true( isBase58( (unsigned char*) base58_chars, 0, 58));

    unsigned char not_base58_char[] = {'0', 'I'};

    assert_false(isBase58( (unsigned char*) not_base58_char, 0, 2));
}

static void test_decimal(void **state) {
    (void) state;

    unsigned char decimal_chars[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

    assert_true( isDecimal( (unsigned char *) decimal_chars, 0, 10));

    unsigned char not_decimal_chars[] = {'a', 'b'};

    assert_false( isDecimal( (unsigned char *) not_decimal_chars, 0, 2));
}


static void test_hexadecimal(void **state) {
    (void) state;

    unsigned char hexadecimal_chars[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 
                                          'a', 'b', 'c', 'd', 'e', 'f', 
                                          'A', 'B', 'C', 'D', 'E', 'F'};

    assert_true( isHexadecimal( (unsigned char *) hexadecimal_chars, 0, 22));

    unsigned char not_hexadecimal_chars[] = {'g', 'G'};

    assert_false( isHexadecimal( (unsigned char *) not_hexadecimal_chars, 0, 2));
}


static void test_text_string(void **state) {
    (void) state;

    unsigned char text_string_chars[] = {'p', 'w', 'd', ' ', 'c', 'h', 'a', 'n', 'g', 'e', 
                                          0x00};

    assert_true( isTextString( (unsigned char *) text_string_chars, 0, 11));

    unsigned char not_text_string_chars[] = {':', 0x01};

    assert_false( isTextString( (unsigned char *) not_text_string_chars, 0, 2));
}


int main() {
    const struct CMUnitTest tests[] = {cmocka_unit_test(test_is_base58),
                                       cmocka_unit_test(test_decimal),
                                       cmocka_unit_test(test_text_string),
                                       cmocka_unit_test(test_hexadecimal)};

    return cmocka_run_group_tests(tests, NULL, NULL);
}

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <cmocka.h>

#include "common/base58.h"
#include "../src/ui/extra2.h"

static void test_decimal(void **state) {
   (void) state;
    unsigned char amt1[] = {0x30, 0x30, 0x30, 0x30, 
                            0x31, 0x32, 0x33, 0x34, 
                            0x35, 0x36, 0x30, 0x30, 
                            0x30, 0x30, 0x30, 0x30};
    char outChars1[17];
    dotDecimal( (unsigned char * ) amt1, (char * ) outChars1);
    char test_out1[17] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 
                      0x2e, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00};
    assert_string_equal( (char *) outChars1, (char *) test_out1);

    unsigned char amt2[] = {0x30, 0x30, 0x30, 0x30, 
                            0x30, 0x30, 0x30, 0x30, 
                            0x30, 0x30, 0x31, 0x32, 
                            0x33, 0x34, 0x35, 0x36};
    char outChars2[17];
    dotDecimal( (unsigned char *) amt2, (char *) outChars2);
    char test_out2[17] = {0x30, 0x2e, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x00};
    assert_string_equal( (char *) outChars2, (char *) test_out2);

    char test_out3[17] = {0x30, 0x2e, 0x31, 0x00};
    assert_string_not_equal( (char *) outChars2, (char *) test_out3);


}

int main() {
    const struct CMUnitTest tests[] = {cmocka_unit_test(test_decimal)};

    return cmocka_run_group_tests(tests, NULL, NULL);
}

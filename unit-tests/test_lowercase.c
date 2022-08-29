#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <cmocka.h>

#include "../src/ui/extra2.h"

static void test_lowercase(void **state) {
   (void) state;
    unsigned char upperChars[] = {0x41, 0x42, 0x43, 0x00};
    unsigned char upperChars2[] = {0x41, 0x42, 0x43, 0x00};
    unsigned char lowerChars[] = {0x61, 0x62, 0x63, 0x00};

    chars_to_lowerCase( (unsigned char * ) upperChars, 3);

    assert_string_equal( (char *) upperChars, (char *) lowerChars);
    assert_string_not_equal( (char *) upperChars, (char *) upperChars2);

}

int main() {
    const struct CMUnitTest tests[] = {cmocka_unit_test(test_lowercase)};

    return cmocka_run_group_tests(tests, NULL, NULL);
}

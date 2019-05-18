#include "bps.c"
#include "utest.h"

TEST(test_get_interesting_bit_count_1, "get_interesting_bit_count")
    unsigned int bc = 0;
    get_interesting_bit_count("11", &bc);
    CHECK_EQ_INT(bc, 11);
}

TEST(test_hexdigit2uchar, "hexdigit2uchar")
    unsigned char c = 0;
    hexdigit2byte('f', '8', &c);
    CHECK_EQ_INT(c, 0xf8);

    c = 0;
    hexdigit2byte('0', '3', &c);
    CHECK_EQ_INT(c, 0x03);

    c = 0;
    hexdigit2byte('a', '1', &c);
    CHECK_EQ_INT(c, 0xa1);
}

TEST(test_make_pattern_1, "make_pattern")
    size_t len = 0;
    uint8_t *p = NULL;
    make_pattern("f8c", &p, &len);
    CHECK_EQ_INTL(len, 2L);
    CHECK_EQ_INT(p[0], 0xf8);
    CHECK_EQ_INT(p[1], 0xC0);
    free(p);
}

TEST(test_get_buf_bit_1, "get_buf_bit")
    // 1111 1000 1100 0000
    // 0123 4567 89AB CDEF
    unsigned char buf[] = { 0xf8, 0xc0 };

    int b2 = get_buf_bit(buf, 2);
    CHECK_EQ_INT(b2, 1);

    int b4 = get_buf_bit(buf, 4);
    CHECK_EQ_INT(b4, 1);

    int b5 = get_buf_bit(buf, 5);
    CHECK_EQ_INT(b5, 0);

    int b9 = get_buf_bit(buf, 9);
    CHECK_EQ_INT(b9, 1);
}

int main() {
    test_get_interesting_bit_count_1();
    test_hexdigit2uchar();
    test_make_pattern_1();
    test_get_buf_bit_1();
    return 1;
}

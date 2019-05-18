/**
 * A quick and dirty unit testing tool.
 */

#ifndef UTEST_H
#define UTEST_H

#define TEST(test_func, name) \
void test_func() { \
    const char *test_name = (name); \
    printf("Test: %s\n", (test_name)); \

#define CHECK_EQ_INT(val, expected) \
do { \
    if ((val) != (expected)) { \
        printf("Failed: " #val " %d != " #expected " %d\n", (val), (expected)); \
        exit(1); \
    } \
} while (0)

#define CHECK_EQ_INTL(val, expected) \
do { \
    if ((val) != (expected)) { \
        printf("Failed: " #val " %ld != " #expected " %ld\n", (val), (expected)); \
        exit(1); \
    } \
} while (0)

#endif

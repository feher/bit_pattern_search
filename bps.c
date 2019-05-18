#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>

#include "bps.h"

#define BYTE_BITS 8

//#define DEBUG

#ifdef DEBUG
static void debug_dump_bin(char byte) {
    for (int i = 0; i < BYTE_BITS; ++i, byte <<= 1) {
        if ((byte & 0x80) != 0) {
            putc('1', stdout);
        } else {
            putc('0', stdout);
        }
    }
}

static void debug_dump_buf_bin(unsigned char *buf, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        debug_dump_bin(buf[i]);
        putc(' ', stdout);
    }
    putc('\n', stdout);
}

static void debug_dump_buf_char(unsigned char *buf, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        putc(buf[i], stdout);
    }
    putc('\n', stdout);
}

#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#define DEBUG_RUN(exp) (exp)

#else
#define DEBUG_PRINT(...)
#define DEBUG_RUN(exp)
#endif // DEBUG

#define RET_IF_ERR(exp) \
do { \
    int err = (exp); \
    if (err) { \
        return err; \
    } \
} while(0)

#define GOTO_IF_ERR(exp, label) \
do { \
    ret = (exp); \
    if (ret) { \
        goto label; \
    } \
} while(0)

// Number of bytes to read ahead from the input stream.
#define BUF_CAPACITY 4096

/**
 * Converts a string hex byte to a byte.
 * 
 * E.g. If digit1 is 'f' and digit2 is 'e' then returns 0xfe. 
 * 
 * @param digit1 In. 1st digit of the hex byte.
 * @param digit2 In. 2nd digit of the hex byte.
 * @param res Out. The byte representation of the string hex byte.
 * 
 * @return 0 success, non-zero failure
 */
static int hexdigit2byte(char digit1, char digit2, uint8_t *res) {
    char num[3] = { digit1, digit2, '\0' };
    char *end;
    uint8_t byte = strtoul(num, &end, 16);
    if (*end != '\0') {
        fprintf(stderr, "Bad hex digit: %s\n", num);
        return BPS_ERR_BAD_HEX_DIGIT;
    }
    *res = byte;
    return 0;
}

/**
 * Converts the pattern represented as a string of hex digits into a byte array.
 * 
 * E.g. "fe8" -> { 0xfe, 0x80 }
 * 
 * @param hex_string In. The hex string.
 * @param pattern Out. A newly allocated array. The caller must free it.
 * @param pattern_len Out. The length of the new array in bytes
 * 
 * @return 0 success, non-zero failure
 */
static int make_pattern(const char *hex_string, uint8_t **pattern, size_t *pattern_len) {
    size_t digit_count = strlen(hex_string);
    size_t p_len = (digit_count + 1) / 2;
    uint8_t *p = (uint8_t *)malloc(p_len * sizeof(uint8_t));
    size_t pi = 0;
    for (size_t hi = 0; hi < digit_count; hi += 2, ++pi) {
        if (hi == digit_count - 1) {
            RET_IF_ERR(
                hexdigit2byte(hex_string[hi], '0', &p[pi]));
        } else {
            RET_IF_ERR(
                hexdigit2byte(hex_string[hi], hex_string[hi + 1], &p[pi]));
        }
    }
    *pattern = p;
    *pattern_len = p_len;
    return 0;
}

/**
 * Converts the "interesting bit count" of the pattern from string to unsigned int.
 * 
 * @param bit_count_string In.
 * @param bit_count Out. The result is stored here.
 * 
 * @return 0 success, non-zero failure
 */
static int get_interesting_bit_count(const char *bit_count_string, unsigned int *bit_count) {
    char *end;
    unsigned int bcount = strtol(bit_count_string, &end, 10);
    if (*end != '\0') {
        fprintf(stderr, "Bad bit_count: %s\n", bit_count_string);
        return BPS_ERR_BAD_BIT_COUNT;
    }
    *bit_count = bcount;
    return 0;
}

/**
 * Returns the bit_num'th bit of the given byte array.
 */
static inline int get_buf_bit(const uint8_t *buf, size_t bit_num) {
    uint8_t byte = *(buf + (bit_num / BYTE_BITS));
    size_t bit_num_end = (BYTE_BITS - 1 - (bit_num % BYTE_BITS));
    return (byte & ((1 << (bit_num_end + 1)) - 1)) >> bit_num_end;
}

/**
 * Reads data into the given buffer.
 * 
 * @param fd In. The input file to read from.
 * @param buf In/out. The buffer to read to.
 * @param buf_capacity In. The max number of bytes the buf can hold.
 * @param buf_len In/out. The buffer length in bytes.
 * @param consumed_bit_count The number of already consumed bits in the buffer. Starting from the start of the buffer.
 * 
 * @returns 0 success, non-zero failure
 */
static int read_buf(int fd,
                    uint8_t *buf,
                    size_t buf_capacity,
                    size_t *buf_len,
                    size_t *consumed_bit_count) {
    // Remove the fully consumed bytes from buf by shifting it to
    // left (to the start of buf).
    size_t consumed_byte_count = *consumed_bit_count / BYTE_BITS;
    memmove(buf, buf + consumed_byte_count, *buf_len - consumed_byte_count);
    *consumed_bit_count -= (consumed_byte_count * BYTE_BITS);
    *buf_len -= consumed_byte_count;

    // Read new bytes into the end of buf
    ssize_t read_bytes = read(fd, buf + *buf_len, buf_capacity - *buf_len);
    if (read_bytes < 0) {
        fprintf(stderr, "Cannot read input\n");
        return BPS_ERR_CANNOT_READ_INPUT;
    }
    *buf_len += read_bytes;
    return 0;
}

/**
 * Ensures that we have enough available (i.e. unconsumed) bits in the buffer.
 *
 * @param fd In. The input file to read from.
 * @param buf In/out. The buffer to read to.
 * @param buf_capacity In. The max number of bytes the buf can hold.
 * @param buf_len In/out. The buffer length in bytes.
 * @param consumed_bit_count The number of already consumed bits in the buffer. Starting from the start of the buffer.
 * @param needed_bit_count In. The number of unconsumed bits buf should have after this operation.
 *
 * @returns 0 success, non-zero failure
 */
static int ensure_buf(int fd,
                      uint8_t *buf,
                      size_t buf_capacity,
                      size_t *buf_len,
                      size_t *consumed_bit_count,
                      size_t needed_bit_count) {
    size_t avail_bit_count = *buf_len * BYTE_BITS - *consumed_bit_count;
    if (avail_bit_count < needed_bit_count) {
        RET_IF_ERR(
            read_buf(fd, buf, buf_capacity, buf_len, consumed_bit_count));
        DEBUG_PRINT("Buf: ");
        DEBUG_RUN(debug_dump_buf_char(buf, *buf_len));
        DEBUG_PRINT("Buf: ");
        DEBUG_RUN(debug_dump_buf_bin(buf, *buf_len));
        avail_bit_count = *buf_len * BYTE_BITS - *consumed_bit_count;
        if (avail_bit_count < needed_bit_count) {
            return BPS_ERR_END_OF_INPUT;
        }
    }
    return 0;
}

/**
 * @param has_match Out. 1 pattern was found, 0 pattern was not found.
 * 
 * @returns 0 success, non-zero failure
 */
static int match(int fd, uint8_t *pattern, size_t pattern_bit_count, int *has_match) {
    int ret = 0;
    uint8_t *buf = malloc(BUF_CAPACITY * sizeof(uint8_t));
    size_t buf_len = BUF_CAPACITY;
    size_t buf_bit_num = buf_len * BYTE_BITS;
    size_t pattern_bit_num = 0;

    do {
        GOTO_IF_ERR(
            ensure_buf(fd, buf, BUF_CAPACITY, &buf_len, &buf_bit_num, pattern_bit_count),
            out);
        int pattern_bit = get_buf_bit(pattern, pattern_bit_num);
        int buf_bit = get_buf_bit(buf, buf_bit_num);
        DEBUG_PRINT("%d", buf_bit);
        if (pattern_bit != buf_bit) {
            DEBUG_PRINT(" mismatch\n");
            buf_bit_num -= pattern_bit_num;
            ++buf_bit_num;
            pattern_bit_num = 0;
        } else {
            ++buf_bit_num;
            ++pattern_bit_num;
        }
        if (pattern_bit_num == pattern_bit_count) {
            *has_match = 1;
            DEBUG_PRINT(" match\n");
            goto out;
        }
    } while (1);

out:
    free(buf);
    return ret;
}

int bps(int fd, const char *pattern_hex_string, const char *pattern_bit_count_string, int *has_match) {
    int ret = 0;
    size_t pattern_len = 0;
    uint8_t *pattern = NULL;
    GOTO_IF_ERR(
        make_pattern(pattern_hex_string, &pattern, &pattern_len),
        out);

    DEBUG_PRINT("Pattern: ");
    DEBUG_RUN(debug_dump_buf_bin(pattern, pattern_len));

    unsigned int bit_count = 0;
    GOTO_IF_ERR(
        get_interesting_bit_count(pattern_bit_count_string, &bit_count),
        out);
    if (bit_count > pattern_len * BYTE_BITS) {
        ret = BPS_ERR_LARGE_BIT_COUNT;
        goto out;
    }

    ret = match(fd, pattern, bit_count, has_match);

out:
    free(pattern);
    return ret;
}


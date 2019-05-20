/*
 * Creates a binary test file.
 * 
 * Usage: gentestbin {MB
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <assert.h>

#define BYTE_BITS 8

/**
 * Returns the bit_num'th bit of the given byte array.
 */
static inline int get_buf_bit(const char *buf, size_t buf_len, size_t bit_num) {
    size_t byte_num = (bit_num / BYTE_BITS);
    if (byte_num > buf_len) {
        return 0;
    }
    uint8_t byte = *(buf + byte_num);
    size_t bit_num_end = (BYTE_BITS - 1 - (bit_num % BYTE_BITS));
    return (byte & ((1 << (bit_num_end + 1)) - 1)) >> bit_num_end;
}

int main(int argc, const char **argv) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s {sentence} {start-bit} {bit-count}\n", argv[0]);
        fprintf(stderr, "Interval is exclusive: [start-bit, start-bit + bit-count)");
        return EXIT_FAILURE;
    }

    char *end;
    const unsigned int startbit = strtoul(argv[2], &end, 10);
    if (*end != '\0') {
        fprintf(stderr, "Bad start-bit\n");
        return EXIT_FAILURE;
    }

    const unsigned int bitcount = strtoul(argv[3], &end, 10);
    if (*end != '\0') {
        fprintf(stderr, "Bad bit-count\n");
        return EXIT_FAILURE;
    }

    const char *sentence = argv[1];
    const size_t slen = strlen(sentence);
    uint8_t nibble = 0;
    size_t extra_bits = 0;
    if ((bitcount % 4) != 0) {
        extra_bits = 4 - (bitcount % 4);
    }
    const size_t endbit = startbit + bitcount;
    for (size_t b = startbit; b < endbit + extra_bits; ++b) {
        const int bit = get_buf_bit(sentence, slen, b);
        const size_t bitnum = b - startbit;
        nibble = (nibble << 1) | bit;
        if ((bitnum % 4) == 3) {
            // Complete nibble
            if (b == endbit + extra_bits - 1) {
                // Last bit (last nibble)
                //printf("last, %ld\n", extra_bits);
                nibble &= ~((1 << extra_bits) - 1);
            }
            printf("%x", nibble);
            nibble = 0;
        }
    }

    return EXIT_SUCCESS;
}

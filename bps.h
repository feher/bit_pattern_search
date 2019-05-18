#ifndef BPS_H
#define BPS_H

#define BPS_ERR_BAD_BIT_COUNT -1 // Invalid pattern_bit_count
#define BPS_ERR_BAD_HEX_DIGIT -2 // Invalid hex digit in pattern
#define BPS_ERR_LARGE_BIT_COUNT -3 // Too large pattern_bit_count
#define BPS_ERR_CANNOT_READ_INPUT -4 // Input error
#define BPS_ERR_END_OF_INPUT -5 // Premature end of input

/**
 * Bit pattern search.
 * 
 * Searches the given bit pattern in the input byte stream.
 * 
 * @param fd In. The input stream's file descriptor.
 * @param pattern In. The pattern as a string of hex digits.
 * @param pattern_bit_count In. The number of significant bits to consider from pattern.
 * @param has_match Out. 1 found a match. 0 no match was found.
 * 
 * @return 0 success, non-zero failure (see the BPS_ERR_* error codes).
 */
int bps(int fd, const char *pattern, const char *pattern_bit_count, int *has_match);

#endif

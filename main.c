#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "bps.h"

int main(int argc, const char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s {pattern} {bit-count}\n", argv[0]);
        return EXIT_FAILURE;
    }
    int has_match = 0;
    int err = bps(STDIN_FILENO, argv[1], argv[2], &has_match);
    if (err) {
        return EXIT_FAILURE;
    }
    return has_match ? EXIT_SUCCESS : EXIT_FAILURE;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>

int write_bytes(const char *sentence, const char *last_sentence, size_t out_bytes) {
    size_t slen = strlen(sentence);
    for (size_t b = 0; b < out_bytes; ) {
        ssize_t wb = write(STDOUT_FILENO, sentence, slen);
        if (wb <= 0) {
            fprintf(stderr, "Cannot write bytes: %ld\n", wb);
            return 0;
        }
        b += wb;
    }
    size_t lslen = strlen(last_sentence);
    ssize_t wb = write(STDOUT_FILENO, last_sentence, lslen);
    if (wb != lslen) {
        fprintf(stderr, "Cannot write last bytes: %ld\n", wb);
        return 0;
    }
    return 1;
}

int main(int argc, const char **argv) {
    if (argc < 4) {
        fprintf(stderr, "Writes the sentence multiple times to stdout.\n");
        fprintf(stderr, "Then writes the last sentence once at the end to stdout.\n");
        fprintf(stderr, "It writes approximately {MB} megabytes bytes of data.\n");
        fprintf(stderr, "Usage: %s {MB} {sentence} {last-sentence}\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *end;
    unsigned int out_megs = strtoul(argv[1], &end, 10);
    if (*end != '\0') {
        fprintf(stderr, "Bad out_megs\n");
        return EXIT_FAILURE;
    }

    const char *sentence = argv[2];
    const char *last_sentence = argv[3];

    if (!write_bytes(sentence, last_sentence, out_megs * 1024 * 1024)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

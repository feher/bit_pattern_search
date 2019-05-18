CC := gcc
OPTIONS := -Wall -O2

.PHONY: all bitmatch test clean

all: bitmatch test

bitmatch: main.c bps.h bps.c
	${CC} ${OPTIONS} -o bitmatch main.c bps.c

test: test.c bps.h bps.c
	${CC} ${OPTIONS} -o test test.c

clean:
	rm bitmatch test

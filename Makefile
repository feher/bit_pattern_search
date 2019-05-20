CC := gcc
OPTIONS := -Wall

.PHONY: all bitmatch test clean

all: bitmatch test

bitmatch: main.c bps.h bps.c
	${CC} ${OPTIONS} -o bitmatch main.c bps.c

test: test.c bps.h bps.c gentestbin.c bitrange.c
	${CC} ${OPTIONS} -o test test.c
	${CC} ${OPTIONS} -o gentestbin gentestbin.c
	${CC} ${OPTIONS} -o bitrange bitrange.c

clean:
	rm bitmatch test gentestbin bitrange


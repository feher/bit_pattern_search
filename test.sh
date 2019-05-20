#!/bin/bash

FOUND=0
NOT_FOUND=1

fail()
{
    echo "Failed: $1"
    exit 1
}

echo -n 'hello' | ./bitmatch 68656c6c6f 40
[[ $? -eq $FOUND ]] || fail "test 1"

echo -n 'hello' | ./bitmatch 68656c6c6c 40
[[ $? -eq $NOT_FOUND ]] || fail "test 1"

echo -n 'h>0?' | ./bitmatch f8c 11
[[ $? -eq $FOUND ]] || fail "test 1"

echo -n 'h<0?' | ./bitmatch f8c 11
[[ $? -eq $NOT_FOUND ]] || fail "test 2"

./gentestbin 1 'hello world' 'hello worlda' > test.bin
PATTERN=68656c6c6f20776f726c6461 # 'hello worlda'
cat test.bin | ./bitmatch $PATTERN $((${#PATTERN} * 4))
[[ $? -eq $FOUND ]] || fail "test 4"

./gentestbin 1 'hello world' 'hello worlda' > test.bin
PATTERN=68656c6c6f20776f726c6477 # 'hello worldw'
cat test.bin | ./bitmatch $PATTERN $((${#PATTERN} * 4))
[[ $? -eq $NOT_FOUND ]] || fail "test 5"

./gentestbin 1 'hello world' 'hello worlda' > test.bin
PATTERN=`./bitrange 'worlda' 3 45`
cat test.bin | ./bitmatch $PATTERN 45
[[ $? -eq $FOUND ]] || fail "test 6"

./gentestbin 1 'hello world' 'hello world' > test.bin
PATTERN=`./bitrange 'worlda' 3 45`
cat test.bin | ./bitmatch $PATTERN 45
[[ $? -eq $NOT_FOUND ]] || fail "test 6"

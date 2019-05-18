Description
-------------
A small C program that reads binary data from stdin and looks
for a given, arbitrary-length bit pattern in it. The bit pattern and
length are input as command-line arguments. The program does not produce
any output (apart from usage and possible error messages). If the bit
pattern is detected, the program exits with 0. Otherwise, it exits with 1.

The command takes two command-line arguments:
1. A sequence of hexadecimal digits representing the bit pattern.
2. A base-10 unsigned integer indicating the number of interesting bits
   in the bit pattern.

Example:

   $ # look for 1111 1000 110 in the input
   $ echo 'h>0?' | ./bitmatch f8c 11 || echo not found
   $ echo 'h<0?' | ./bitmatch f8c 11 || echo not found
   not found
   $

Build instructions
--------------------
Requirements:
* gcc
* make

$ make bitmatch

This will produce the ./bitmatch executable.

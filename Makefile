CFLAGS= -O2

all:
	gcc -g -Wextra -Wall freq/test.c timedifference/correlation.c freq/goertzel.c common/wavheader.c -lfftw3 -lm -o test

valgrind:
	make all && valgrind ./test -file 20khz/pierce.wav -siren -spec 256 -showhead

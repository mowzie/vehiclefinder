CFLAGS= -O2

all:
	gcc freq/test.c freq/goertzel.c common/wavheader.c -lfftw3 -lm -o test
valgrind:
	make all && valgrind ./test -file 20khz/pierce.wav -siren -spec 256 -showhead

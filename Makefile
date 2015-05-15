CFLAGS= -O2

all:
	gcc freq/test.c freq/goertzel.c common/wavheader.c -lfftw3 -lm -o test
	gcc tools/combineWav.c common/wavheader.c -lm -o combineWav

test:
	gcc freq/test.c freq/goertzel.c common/wavheader.c -lfftw3 -lm -o test

combineWav:
	gcc tools/combineWav.c common/wavheader.c -lm -g -o combineWav

valgrind:
	make all && valgrind ./test -file 20khz/pierce.wav -siren -spec 256 -showhead

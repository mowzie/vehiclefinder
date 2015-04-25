CFLAGS= -O2

all:
	gcc freq/test.c freq/goertzel.c common/wavheader.c -lm -o test
run:
	gcc -Wall common\test.c common\goertzel.c common\wavheader.c -lm -o test && ./test
valgrind:
	make all && valgrind ./test test.wav

CC=g++
CFLAGS=-Wall

all: vocoder
vocoder: vocoder.o
vocoder.o: vocoder.cpp

clean:
	rm -f vocoder vocoder.o
run: vocoder
	./vocoder

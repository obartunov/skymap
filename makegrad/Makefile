CC=gcc
AR=ar
CFLAGS=-O
LIBS=-L./ -lgd -lm

all: makegradient

makegradient: makegrad.o
	$(CC) makegrad.o -o makegrad	$(LIBS)

clean:
	rm -f *.o makegrad


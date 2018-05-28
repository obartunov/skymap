CC=gcc
AR=ar
CFLAGS=-O
LIBS=-L./ -lgd -lm

all: maker

maker : mr.o
	$(CC) mr.o -o makering	$(LIBS)

clean:
	rm -f *.o makering


CC=gcc
AR=ar
CFLAGS=-O
LIBS=-L./ -lgd -lm

all: makehoriz

makehoriz: makehor.o gd.h
	$(CC) makehor.o -o makehor	$(LIBS)

clean:
	rm -f *.o makehor


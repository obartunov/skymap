CC=gcc
AR=ar
CFLAGS=-O3 -fPIC -Wall -I../gd1.2c/ -I../ClibPDF/source/ -I../cgic/ -I/usr/local/pgsql/include/ 
LIBS=-L./ -L../ClibPDF/source/ -L../cgic/ -L../gd1.2c/ -L/usr/local/pgsql/lib/ -lgd -lm -lcgic -lcpdf -lpq
ADDLIBS=-lgetnear
MAKE=make

all: sky libgetnear.a

sky: gd12c cpdf cgic \
     skyc2.o draw_functions.o coord.o read_a20.o read_tycho.o \
     galaxies.o pol_init.o mid_init.o equ_init.o boundaries.o \
     prepare.o misc.o rc3_read.o otherobjs.o objects_read.o hl_fetch.o
	$(CC) skyc2.o draw_functions.o coord.o \
              read_a20.o read_tycho.o galaxies.o \
              pol_init.o mid_init.o equ_init.o \
              boundaries.o prepare.o misc.o rc3_read.o \
              otherobjs.o objects_read.o hl_fetch.o -o skyc2.cgi $(LIBS)

gd12c:
	$ cd ../gd1.2c/ && $(MAKE)

cpdf:
	$ cd ../ClibPDF/source/ && $(MAKE) lib

cgic:
	$ cd ../cgic/ && $(MAKE)

libgetnear.a: get_near.h get_near.o rc3_read.o read_tycho.o read_a20.o coord.o
	rm -f libgetnear.a
	$(AR) rc libgetnear.a get_near.o rc3_read.o read_tycho.o read_a20.o coord.o

clean:
	rm -f *.o skyc2.cgi libgetnear.a

distclean: clean
	cd ../cgic && $(MAKE) clean
	cd ../ClibPDF/source && $(MAKE) clean
	cd ../gd1.2c && $(MAKE) clean

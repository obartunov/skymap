CC=gcc
AR=ar
CFLAGS=-O3 -I./ -I../gd-2.0.15gif/ -I../ClibPDF/source/ -I../libastro/ -I../cgic/
LIBS=-L../gd-2.0.15gif/ -L../libastro/ -L../ClibPDF/source/ -L../cgic/ -lgd -lm -lastro -lcpdf -lcgic -ljpeg -lpng -lz
MAKE=make

all: sky_cgi

sky_cgi: libgd.a libcpdf.a libastro.a libcgic.a skyc.o
	$(CC) skyc.o -o skyc.cgi	$(LIBS)

libgd.a:
	(file ../gd-2.0.15gif/libgd.a) || (cd ../gd-2.0.15gif/ && sh configure && $(MAKE) && cp .libs/libgd.a .)
libastro.a:
	cd ../libastro/ && $(MAKE)
libcpdf.a:
	cd ../ClibPDF/source/ && $(MAKE) lib
libcgic.a:
	cd ../cgic/ && $(MAKE)

clean:
	rm -f *.o skyc.cgi

distclean: clean
	cd ../cgic/ && $(MAKE) clean
	cd ../ClibPDF/source/ && $(MAKE) clean
	cd ../gd-2.0.15gif/ && $(MAKE) clean
	cd ../libastro/ && $(MAKE) clean

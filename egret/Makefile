# Makefile for egret
# This version is designed to compile into object files which may then 
# be linked into a larger program. 
# "make egret" compiles such a program. This should be used as a model
# for any makefile for a larger program.
#
# NED NASA/IPAC Extragalacatic Database

CC = gcc
CFLAGS = -O3 -fPIC
FLEX = /usr/bin/flex
LIBDIR = ./
MANDIR = ./
INCLUDEDIR = ./
LIBEGRETSRC = egret.rect.c lex.yy.c egret.util.c yywrap.c

all: ${LIBDIR}/libegret.a

egret: egret.o  ${LIBDIR}/libegret.a
	${CC} ${CFLAGS} egret.o -L${LIBDIR} -legret  -lfl -lm -o egret
	${RM} *.o

lex.yy.c: muscles
	@test "$(FLEX)" && $(FLEX) muscles ||\
	echo "flex, not available, cannot make the scanner"

muscles: micro_db egretc
	./egretc  -c -L micro_db

egret.rect.c: micro_db egretc
	./egretc -L micro_db

egretc: egretc.c
	${CC} ${CFLAGS} egretc.c -o egretc

clean:
	${RM} -f egret.rect.* ${LIBDIR}/libegret.a lex.yy.c muscles egretc *.o

${LIBDIR}/libegret.a: $(LIBEGRETSRC)
	@ mkdir -p ${LIBDIR} 
	for file in $(LIBEGRETSRC); do ${CC} ${CFLAGS} -c  $${file}; done;
	ar -csr ${LIBDIR}/libegret.a *.o
	${RM} *.o

#############################################################################

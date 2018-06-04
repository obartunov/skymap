AR = ar clq
RM = rm -f
RANLIB = ranlib
CFLAGS = -I../libastro-3.7.5
OBJS = jupiter.1020.o  mars.1020.o  saturne.1020.o  uranus.1020.o jupiter.9910.o  mars.9910.o  saturne.9910.o uranus.9910.o

libastrodata.a:  $(OBJS) 
	$(RM) $@
	$(AR) $@  $(OBJS)
	$(RANLIB) $@

clean: 
	$(RM) $(OBJS) libastrodata.a


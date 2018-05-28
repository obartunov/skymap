#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "egret.h"

#define KEY(x)  x, strlen(x)
#define ADDINTKEY(h,x,val)      hv_store((h), x, strlen(x), newSViv(val), 0)
#define ADDFLOATKEY(h,x,val)    hv_store((h), x, strlen(x), newSVnv((double)val), 0)
#define ADDCHARKEY(h,x,val)     if (val) hv_store((h), x, strlen(x), newSVpv(val,strlen(val)), 0)

static int
not_here(char *s)
{
    croak("%s not implemented on this architecture", s);
    return -1;
}

static double
constant(char *name, int arg)
{
    errno = 0;
    switch (*name) {
    }
    errno = EINVAL;
    return 0;

not_there:
    errno = ENOENT;
    return 0;
}

MODULE = Astro::HL_Client               PACKAGE = Astro::HL_Client

double
constant(name,arg)
        char *          name
        int             arg

PROTOTYPES: DISABLE

int
PHL_designation(objname, opo)
	char *objname
	char * opo
	CODE:
	list_of_name lof;
	int status;
	char *strout=NULL;

	lof = egret(objname);
	if(lof->status < 0) {
	  status=-1;
	}
	else if (lof->names != NULL && lof->mesgs == NULL){
	  status=0;
	  strout=(char *)malloc(sizeof(char)*strlen(lof->names->str));
	  strcpy(strout,lof->names->str);
	}
	else if (lof->names == NULL) {
	  status=1;
	}
	else {
	  if(lof->names->next) {
	    status=2;
	  }
	}
	if(status!=0){
	  strout=(char *)malloc(sizeof(char)*strlen(objname));
	  strcpy(strout,objname);
	}
	RETVAL=status;
	opo=strout;
	OUTPUT:
	RETVAL
	opo


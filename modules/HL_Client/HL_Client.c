/*
 * This file was generated automatically by ExtUtils::ParseXS version 2.18 from the
 * contents of HL_Client.xs. Do not edit this file, edit HL_Client.xs instead.
 *
 *	ANY CHANGES MADE HERE WILL BE LOST! 
 *
 */

#line 1 "HL_Client.xs"
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

#ifndef PERL_UNUSED_VAR
#  define PERL_UNUSED_VAR(var) if (0) var = var
#endif

#line 51 "HL_Client.c"

XS(XS_Astro__HL_Client_constant); /* prototype to pass -Wmissing-prototypes */
XS(XS_Astro__HL_Client_constant)
{
#ifdef dVAR
    dVAR; dXSARGS;
#else
    dXSARGS;
#endif
    if (items != 2)
       Perl_croak(aTHX_ "Usage: %s(%s)", "Astro::HL_Client::constant", "name, arg");
    PERL_UNUSED_VAR(cv); /* -W */
    {
	char *	name = (char *)SvPV_nolen(ST(0));
	int	arg = (int)SvIV(ST(1));
	double	RETVAL;
	dXSTARG;

	RETVAL = constant(name, arg);
	XSprePUSH; PUSHn((double)RETVAL);
    }
    XSRETURN(1);
}


XS(XS_Astro__HL_Client_PHL_designation); /* prototype to pass -Wmissing-prototypes */
XS(XS_Astro__HL_Client_PHL_designation)
{
#ifdef dVAR
    dVAR; dXSARGS;
#else
    dXSARGS;
#endif
    if (items != 2)
       Perl_croak(aTHX_ "Usage: %s(%s)", "Astro::HL_Client::PHL_designation", "objname, opo");
    PERL_UNUSED_VAR(cv); /* -W */
    {
	char *	objname = (char *)SvPV_nolen(ST(0));
	char *	opo = (char *)SvPV_nolen(ST(1));
	int	RETVAL;
	dXSTARG;
#line 51 "HL_Client.xs"
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
#line 121 "HL_Client.c"
	sv_setpv((SV*)ST(1), opo);
	SvSETMAGIC(ST(1));
	XSprePUSH; PUSHi((IV)RETVAL);
    }
    XSRETURN(1);
}

#ifdef __cplusplus
extern "C"
#endif
XS(boot_Astro__HL_Client); /* prototype to pass -Wmissing-prototypes */
XS(boot_Astro__HL_Client)
{
#ifdef dVAR
    dVAR; dXSARGS;
#else
    dXSARGS;
#endif
    char* file = __FILE__;

    PERL_UNUSED_VAR(cv); /* -W */
    PERL_UNUSED_VAR(items); /* -W */
    XS_VERSION_BOOTCHECK ;

        newXS("Astro::HL_Client::constant", XS_Astro__HL_Client_constant, file);
        newXS("Astro::HL_Client::PHL_designation", XS_Astro__HL_Client_PHL_designation, file);
    XSRETURN_YES;
}


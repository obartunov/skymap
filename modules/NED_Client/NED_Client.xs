#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include <stdio.h>
#include <stdlib.h>

#include "ned_client.h"

#define KEY(x)  x, strlen(x)
#define ADDINTKEY(h,x,val)      hv_store((h), x, strlen(x), newSViv(val), 0)
#define ADDFLOATKEY(h,x,val)    hv_store((h), x, strlen(x), newSVnv((double)val), 0)
#define ADDCHARKEY(h,x,val)     if (val) hv_store((h), x, strlen(x), newSVpv(val,strlen(val)), 0)

extern int   ned_errno;

static void
CrossID2h(CrossID *u, HV *h){
	ADDCHARKEY(h,"objname",u->objname);
	ADDCHARKEY(h,"objtype",u->objtype);
}

static void
MoreData2h(MoreData *u, HV *h){
	MoreData *ptr=u->next;
	HV *next=newHV();
	int i=0;
	ADDCHARKEY(h,"data_typec",u->data_typec);
	ADDCHARKEY(h,"data",u->data);
}

static void
ObjInfo2h(ObjInfo *u, HV *h){
	CrossID *ptrcp;
	AV *cp=newAV();
	MoreData *ptrmdp;
	AV *mdp=newAV();
	int i;

	ADDINTKEY(h,"no_crossid",u->no_crossid);
	ADDFLOATKEY(h,"dist",u->dist);
	ADDINTKEY(h,"no_ref",u->no_ref);
	ADDINTKEY(h,"no_note",u->no_note);
	ADDINTKEY(h,"no_photom",u->no_photom);
	ADDCHARKEY(h,"objtype",u->objtype);
	ADDFLOATKEY(h,"ra",u->ra);
	ADDFLOATKEY(h,"dec",u->dec);
	ADDFLOATKEY(h,"unc_maj",u->unc_maj);
	ADDFLOATKEY(h,"unc_min",u->unc_min);
	ADDFLOATKEY(h,"unc_ang",u->unc_ang);
	ADDCHARKEY(h,"refcode",u->refcode);
	for(i=0, ptrcp=u->cp; i<u->no_crossid; i++, ptrcp++){
		HV *cp_cur=newHV();
		CrossID2h(ptrcp,cp_cur);
		av_push(cp, newRV_noinc((SV*)cp_cur));
	}
	hv_store(h, "cp", strlen("cp"), newRV_noinc((SV*)cp),0);
	ptrmdp=u->mdp;
	while(ptrmdp->next) {
		HV *mdp_cur=newHV();
		MoreData2h(ptrmdp,mdp_cur);
		av_push(mdp, newRV_noinc((SV*)mdp_cur));
		ptrmdp=ptrmdp->next;
	}
	hv_store(h, "mdp", strlen("mdp"), newRV_noinc((SV*)mdp),0);
}

static void
NedRef2h(NedRef *u, HV *h){
	ADDCHARKEY(h,"refcode",u->refcode);
	ADDCHARKEY(h,"pub_name",u->pub_name);
	ADDINTKEY(h,"year",u->year);
	ADDCHARKEY(h,"vol",u->vol);
	ADDCHARKEY(h,"page",u->page);
	ADDCHARKEY(h,"title1",u->title1);
	ADDCHARKEY(h,"title2",u->title2);
	ADDCHARKEY(h,"author1",u->author1);
	ADDCHARKEY(h,"author2",u->author2);
}

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

MODULE = Astro::NED_Client               PACKAGE = Astro::NED_Client

double
constant(name,arg)
        char *          name
        int             arg

PROTOTYPES: DISABLE

int
PNED_obj_byname(objname, opo)
	char *objname
	SV * opo
	CODE:
	int st;
	ObjInfo *op, *tmpop;
	CrossID *cp, *tmpcp;
	MoreData *mdatap;
	AV *objinf;
	int    no_obj;
	int    i, j;

	RETVAL=-1000;
	objinf=(AV*)SvRV(opo);
	st = ned_connect();
        if (st != 0) {
	   fprintf(stderr,"Impossible to set up connection\n");
	   RETVAL=-1;
	   av_push(objinf,newSViv(-1));
	} else {
	  st = ned_obj_byname(objname, &no_obj, &op, &cp);
	  RETVAL=(int)st;
	  if (st < 0) {
	    char *tststr=(char*)malloc(100*sizeof(char));
	    /* for simple error message */
	    av_push(objinf,newSViv(ned_errno));
	    switch (ned_errno) {
	      case NE_NAME:
	         sprintf(tststr, 
	         "name %s can't be recognized by NED name interpreter\n",
	          objname);
	         break;
	      case NE_AMBN:
	         sprintf(tststr, "%d ambiguous name: \n", no_obj);
	         /*for (i=0, tmpcp = cp; i<no_obj; i++, tmpcp++)*/
	         /*   sprintf(tststr, "%s \n", tmpcp->objname);*/
	         break;
	      case NE_NOBJ:
	         sprintf(tststr, "object %s is not in NED database\n",
	            cp->objname);
	         break;
	      case NE_NOSPACE:
	         sprintf(tststr, "memory allocation error happened \n");
	         break;
	      case NE_QUERY:
	         sprintf(tststr, "Can't send query to the NED server\n");
	         break;
	      case NE_BROKENC:
	         sprintf(tststr, "The connection to server is broken\n");
        	 break;
	    }
	    av_push(objinf,newSVpv(tststr,strlen(tststr)));
	    free(tststr);
	  } /* -1 return code */
	  else {
	    for (i=0, tmpop = op; i<no_obj; i++, tmpop++) {
	      HV *objinf_cur=newHV();
	      ObjInfo2h(tmpop,objinf_cur);
	      av_push(objinf, newRV_noinc((SV*)objinf_cur));
	    }
	  }

	if (cp)
	   ned_free_cp(cp);
	if (op)
	   ned_free_objp(op, no_obj);
	ned_disconnect();
	}
	OUTPUT:
	RETVAL
	opo


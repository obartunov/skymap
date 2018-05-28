#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "get_near.h"

#define KEY(x)  x, strlen(x)
#define ADDINTKEY(h,x,val)      hv_store((h), x, strlen(x), newSViv(val), 0)
#define ADDFLOATKEY(h,x,val)    hv_store((h), x, strlen(x), newSVnv((double)val), 0)
#define ADDCHARKEY(h,x,val)     if (val) hv_store((h), x, strlen(x), newSVpv(val,strlen(val)), 0)

static void
Gal2h(gal_rc3 *u, HV *h) {
	ADDFLOATKEY(h,"ra",u->ra);
	ADDFLOATKEY(h,"dec",u->dec);
	ADDFLOATKEY(h,"mag",u->mag);
	ADDFLOATKEY(h,"fi",u->fi);
	ADDFLOATKEY(h,"Asize",u->Asize);
	ADDFLOATKEY(h,"Bsize",u->Bsize);
	ADDCHARKEY(h,"p_name",u->p_name);
	ADDCHARKEY(h,"name2",u->name2);
	ADDCHARKEY(h,"name3",u->name3);
	ADDCHARKEY(h,"name4",u->name4);
	ADDINTKEY(h,"type_obj",u->type_obj);
}

static void
Star2h(star *u, HV *h) {
	ADDFLOATKEY(h,"ra",u->ra);
	ADDFLOATKEY(h,"dec",u->dec);
	ADDFLOATKEY(h,"mag",u->mag);
	ADDFLOATKEY(h,"b_v",u->b_v);
	ADDFLOATKEY(h,"pm_ra",u->pm_ra);
	ADDFLOATKEY(h,"pm_dec",u->pm_dec);
	ADDFLOATKEY(h,"par",u->par);
	ADDINTKEY(h,"bmag",u->bmag);
	ADDINTKEY(h,"rmag",u->rmag);
}

static void
Par2h(MapParamsPtr u, HV *h) {
	ADDFLOATKEY(h,"RA_center",u->RA_center);
	ADDFLOATKEY(h,"Dec_center",u->Dec_center);
	ADDFLOATKEY(h,"zoomk",u->zoomk);
	ADDFLOATKEY(h,"st_maglim",u->st_maglim);
	ADDFLOATKEY(h,"gal_maglim",u->gal_maglim);
	ADDFLOATKEY(h,"st_mlpr_b",u->st_mlpr_b);
	ADDFLOATKEY(h,"st_mlpr_f",u->st_mlpr_f);
	ADDFLOATKEY(h,"RA_grid_cellsize",u->RA_grid_cellsize);
	ADDFLOATKEY(h,"Dec_grid_cellsize",u->Dec_grid_cellsize);
	ADDFLOATKEY(h,"imzoom",u->imzoom);
	ADDINTKEY(h,"SIZEX",u->SIZEX);
	ADDINTKEY(h,"SIZEY",u->SIZEY);
	ADDINTKEY(h,"DX",u->DX);
	ADDINTKEY(h,"DY",u->DY);
	ADDINTKEY(h,"pdf",u->pdf);
	ADDINTKEY(h,"display_grids",u->display_grids);
	ADDINTKEY(h,"display_constbnd",u->display_constbnd);
	ADDINTKEY(h,"print_starsmag",u->print_starsmag);
	ADDINTKEY(h,"display_galaxies",u->display_galaxies);
	ADDINTKEY(h,"suppress_unkmag_gal",u->suppress_unkmag_gal);
	ADDINTKEY(h,"print_galngc",u->print_galngc);
	ADDINTKEY(h,"print_galic",u->print_galic);
	ADDINTKEY(h,"print_galugc",u->print_galugc);
	ADDINTKEY(h,"print_galpgc",u->print_galpgc);
	ADDINTKEY(h,"print_galmag",u->print_galmag);
	ADDINTKEY(h,"mirrorx",u->mirrorx);
	ADDINTKEY(h,"mirrory",u->mirrory);
	ADDINTKEY(h,"plot_add_data",u->plot_add_data);
	ADDCHARKEY(h,"title",u->title);
	ADDCHARKEY(h,"plotlabels",u->plotlabels);
	ADDINTKEY(h,"gifpar",0);
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

MODULE = SkyMap::GetNear               PACKAGE = SkyMap::GetNear

double
constant(name,arg)
        char *          name
        int             arg

PROTOTYPES: DISABLE

void *
PStruct_Init(RA_center,Dec_center,st_maglim,gal_maglim,zoomk,SIZEX,suppress_unkmag_gal)
	double	RA_center
	double	Dec_center
	double	st_maglim
	double	gal_maglim
	double	zoomk
	int	SIZEX
	int	suppress_unkmag_gal
	CODE:
	RETVAL=calloc(1,sizeof(MapParams));
	((MapParamsPtr)RETVAL)->RA_center=RA_center;
	((MapParamsPtr)RETVAL)->Dec_center=Dec_center;
	((MapParamsPtr)RETVAL)->st_maglim=st_maglim;
	((MapParamsPtr)RETVAL)->gal_maglim=gal_maglim;
	((MapParamsPtr)RETVAL)->zoomk=zoomk;
	((MapParamsPtr)RETVAL)->SIZEX=SIZEX;
	((MapParamsPtr)RETVAL)->suppress_unkmag_gal=suppress_unkmag_gal;
	((MapParamsPtr)RETVAL)->SIZEY=3*SIZEX/4;
	((MapParamsPtr)RETVAL)->imzoom=(double)SIZEX/1280.0;
	OUTPUT:
	RETVAL

int 
Pread_rc3data(sout)
	SV * sout
	CODE:
	do {
	AV *out=(AV*)SvRV(sout);
	gal_rc3 *rc3list;
	long int n_objs,i;
	n_objs=23023;
	rc3list=read_rc3data();
	if (rc3list!=NULL){
		for (i=0;i<n_objs;i++){
			HV *gal=newHV();
			Gal2h(&(rc3list[i]),gal);
			av_push(out, newRV_noinc( (SV*)gal ));
		}
	}
	RETVAL=n_objs;
	free (rc3list);
	} while(0);
	OUTPUT:
	RETVAL
	sout

int
Pselect_rc3galaxies(obj, sout)
	void * obj
	SV * sout
	CODE:
	do {
	AV *out=(AV*)SvRV(sout);
	gal_rc3 *rc3list,*rc3sellist;
	long int i,nmatches;
	int status;

	rc3list=read_rc3data();
	
	rc3sellist=select_galaxies((MapParamsPtr)obj, rc3list, 23023,
		&nmatches,&status);
	RETVAL=status;
	free(rc3list);

	if (nmatches>0){
		for (i=0;i<nmatches;i++){
			HV *gal=newHV();
			Gal2h(&(rc3sellist[i]),gal);
			av_push(out, newRV_noinc( (SV*)gal ));
		}
	}
	/*for (i=0;i<n_objs;i++){
		SV **gal=av_fetch(in, i, 0);
	}*/
	free(rc3sellist);
	} while(0);
	OUTPUT:
	RETVAL
	sout

int
Pselect_tychostars(obj, sout)
	void * obj
	SV * sout
	CODE:
	do {
	AV *out=(AV*)SvRV(sout);
	FILE *ftyc;
	long int *t_ind, NMAX, n_objs, i;
	star *tycho_zone, *tycho_list;
	int status, u;
	
	t_ind=read_index_tycho();
	RETVAL=0;
	if (((ftyc=fopen(TYCHO_DATA,"rb"))==NULL)||(t_ind==NULL)){
		RETVAL=-1;
	} else {
	for(u=1;u<=864;u++){
		if (zone_check_tycho((MapParamsPtr)obj,u)==1){
			NMAX=read_zone_tycho_s((MapParamsPtr)obj,t_ind,ftyc,u,&tycho_zone);
			status=select_stars((MapParamsPtr)obj,tycho_zone,NMAX,&tycho_list,&n_objs);
			free(tycho_zone);

			if (n_objs>0){
				for (i=0;i<n_objs;i++){
					HV *tyc_star=newHV();
					Star2h(&(tycho_list[i]),tyc_star);
					av_push(out, newRV_noinc( (SV*)tyc_star ));
				}
			}
			free(tycho_list);
		}
	}
	free(t_ind);
	fclose(ftyc);
	}
	} while(0);
	OUTPUT:
	RETVAL
	sout

int
Pselect_a20stars(obj, sout)
	void * obj
	SV * sout
	CODE:
	do {
	AV *out=(AV*)SvRV(sout);
	FILE *ftest;
	long int NMAX, n_objs, k, **a20_ind;
	star *a20_zone, *a20_list;
	char **zone_fnames;
	int status, i, j;
	
	RETVAL=0;
	if ((ftest=fopen(A20_TEST,"rb"))==NULL){
		RETVAL=-1;
	} else {
	fclose(ftest);
	a20_ind=read_index_a20(&zone_fnames);
	for(i=0;i<96;i++)
	for(j=0;j<24;j++){
		if (zone_check_a20((MapParamsPtr)obj,i,j)==1){
			NMAX=read_zone_a20_s((MapParamsPtr)obj,a20_ind,zone_fnames,i,j,&a20_zone);
			status=select_stars((MapParamsPtr)obj,a20_zone,NMAX,&a20_list,&n_objs);
			free(a20_zone);

			if (n_objs>0){
				for (k=0;k<n_objs;k++){
					HV *a20_star=newHV();
					Star2h(&(a20_list[k]),a20_star);
					av_push(out, newRV_noinc( (SV*)a20_star ));
				}
			}
			free(a20_list);
		}
	}
	for (i=0;i<24;i++) free(zone_fnames[i]);
	free(zone_fnames);
	for (i=0;i<97;i++) free(a20_ind[i]);
	free(a20_ind);
	}
	} while(0);
	OUTPUT:
	RETVAL
	sout

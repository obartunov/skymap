#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "get_near.h"

int compare_gal_params(MapParamsPtr skymap_par,gal_rc3 rc3_0)
{
    int Value_f=1;
    double alp,del,smag;
    alp  = rc3_0.ra;
    del  = rc3_0.dec;
    smag = rc3_0.mag;
    //    if (fabs(del-delta)>(2*0.15625/(zoomk*imzoom))) Value_f=0;
    if (fabs(del-skymap_par->Dec_center)>(0.5*(rc3_0.Bsize/60)+(2*0.15625/(skymap_par->zoomk*skymap_par->imzoom)))) Value_f=0;
    if (del!=90)
    {
        if (fabs(alp-skymap_par->RA_center)>(0.0333*(rc3_0.Bsize/60)/cos(del*Pi/180)+(2*0.0390625/(skymap_par->zoomk*skymap_par->imzoom*cos(del*Pi/180))))) Value_f=0;
    }
    if (smag>skymap_par->gal_maglim) Value_f=0;
    if ((smag<0.1)&&(skymap_par->suppress_unkmag_gal==1)) Value_f=0;
    return Value_f;
}



gal_rc3 *select_galaxies(MapParamsPtr skymap_par, gal_rc3 *gal_list,
                         long int n_max, long int *nmatches, int *status)
{
    gal_rc3 gal;
    gal_rc3 *sel_gal_list;
    int i;

    *nmatches=0;
    *status=0;
    if (gal_list != NULL) {
        sel_gal_list=(gal_rc3 *)malloc(100.0*sizeof(gal_rc3));
        for (i=0;i<n_max;i++){
            memcpy((void *)&(gal),(void *)&(gal_list[i]),sizeof(gal_rc3));
            if (*nmatches<=80) {
                if (compare_gal_params(skymap_par,gal)==1) {
                    memcpy((void *)&(sel_gal_list[*nmatches]),(void *)&gal,sizeof(gal_rc3));
                    (*nmatches)++;
                }
            } else {
                *status=2;
                break;
            }
        }
    } else {
        *nmatches=0;
        sel_gal_list=NULL;
    }

    if (*nmatches==0) *status=1;
    return sel_gal_list;
}

int compare_stellar_params(MapParamsPtr skymap_par, star star_cur)
{
    int Value_f=1;
    //if (fabs(del-skymap_par->Dec_center)>(0.05208334*(3.0+(int)(fabs(skymap_par->st_maglim-0.1*smag)))/(skymap_par->zoomk*skymap_par->imzoom))) Value_f=0;
    if (fabs(star_cur.dec-skymap_par->Dec_center)>(0.05208334*3.0/(skymap_par->zoomk*skymap_par->imzoom))) Value_f=0;
    if (star_cur.dec!=90)
    {
        //if (fabs(alp-skymap_par->RA_center)>(0.00347223*(3.0+(int)(fabs(skymap_par->st_maglim-0.1*smag)))/(skymap_par->zoomk*skymap_par->imzoom*cos(del*Pi/180.0)))) Value_f=0;
        if (fabs(star_cur.ra-skymap_par->RA_center)>(0.00347223*3.0/(skymap_par->zoomk*skymap_par->imzoom*cos(star_cur.dec*Pi/180.0)))) Value_f=0;
    }
    if (fabs(star_cur.mag)>skymap_par->st_maglim+0.05) Value_f=0;
    return Value_f;
}

int select_stars(MapParamsPtr skymap_par,
                       star *cat_zone, long int n_max,
                       star **output_list, long int *nmatches)
{
    star cat_star;
    int i;

    int status=0;

    *nmatches=0;
    if (cat_zone != NULL) {
        (*output_list)=(star *)malloc(100.0*sizeof(star));
        for (i=0;i<n_max;i++){
            memcpy((void *)&(cat_star),(void *)&(cat_zone[i]),sizeof(star));
            if (*nmatches<=80) {
                if (compare_stellar_params(skymap_par,cat_star)==1) {
                    memcpy((void *)&((*output_list)[*nmatches]),(void *)&cat_star,sizeof(star));
                    (*nmatches)++;
                }
            } else {
                status=2;
                break;
            }
        }
    } else {
        *nmatches=0;
        (*output_list)=NULL;
    }

    if (*nmatches==0) status=1;
    return status;

}

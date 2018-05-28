#ifndef __GET_NEAR_H__
#define __GET_NEAR_H__

#include "skyc_readdata.h"

int compare_gal_params(MapParamsPtr skymap_par,gal_rc3 rc3_0);
gal_rc3 *select_galaxies(MapParamsPtr skymap_par, gal_rc3 *gal_list,
                         long int n_max, long int *nmatches, int *status);

int compare_stellar_params(MapParamsPtr skymap_par, star star_cur);
int select_stars(MapParamsPtr skymap_par,
                       star *cat_zone, long int n_max,
                       star **output_list, long int *nmatches);
#endif

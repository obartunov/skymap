#ifndef __SKYC_READDATA_H__
#define __SKYC_READDATA_H__

#include "skyc_struct.h"
/*function prototypes*/

/*read_a20.c*/
long int **read_index_a20(char ***zone_fnames);
long int read_zone_a20(MapParamsPtr skymap_par,
                       long int **a20_ind, char **zone_fnames,
                       int x, int y, double ** a20coord,
                       short int ** a20bmag, short int ** a20rmag);
int zone_check_a20(MapParamsPtr skymap_par,int i,int j);
long int read_zone_a20_s(MapParamsPtr skymap_par,
                         long int **a20_ind, char **zone_fnames,
                         int x, int y,star **a20_zone);

/*read_tycho.c*/
long int *read_index_tycho(void);
long int read_zone_tycho(MapParamsPtr skymap_par, 
                   long int *t_ind, FILE *ftyc, int j,
                   double ** tyccoord, short int ** tycmag,
                   double ** tycb_v, double ** tyc_pmra,
                   double ** tyc_pmdec, double ** tyc_par);
int zone_check_tycho(MapParamsPtr skymap_par, int j);
long int read_zone_tycho_s(MapParamsPtr skymap_par, long int *t_ind,
                         FILE *ftyc, int j, star **tycho_zone);
                         
//objects_read.c
plotable_obj * read_objstxt(char *filename, long int *objnum);
plotable_obj * get_objs_from_string(char *inp_string, long int *objnum);
char *get_filename(long int obj_id, char **obj_title);

//rc3_read.c
gal_rc3 * read_rc3data();

//coord.c
int CoordType(MapParamsPtr skymap_par);
starxy CompEquCoord(MapParamsPtr skymap_par, double alpha, double delta, int sm);
starxy CompMidCoord(MapParamsPtr skymap_par, double alpha, double delta, int sm);
starxy CompPolCoord(MapParamsPtr skymap_par, double alpha, double delta, int sm);
int CompCoordinates(MapParamsPtr skymap_par, double *radec, short int *mag, int **x, int **y, short int **r, long int obj_n, char comp_rad);
int comp_radius(int sm, double stmg);

#endif

#ifndef __SKYC_STRUCT_H__
#define __SKYC_STRUCT_H__

#include "gdfonts.h"
#include "skyc_def.h"

#ifdef USE_HL
#include "libpq-fe.h"
#endif

typedef struct
{
    double  ra, dec, mag, fi, Asize, Bsize;
    char p_name[30], name2[30], name3[30], name4[30];
    int     type_obj;
} gal_rc3;

typedef struct
{
    double  ra, dec, mag, b_v, pm_ra, pm_dec, par; /* tycho star */
    short int bmag, rmag; /* USNO A2.0 star */
} star;

typedef struct
{
    double ra;
    double dec;
    int color;
    int line_to; /* 1 means connection with previous object if it exists */
    char designation[255];
    int symtype;
    double minsymsize; /* minimal symbol size in pixels */
    double objsize; /* symbol diameter in arcmin */
} plotable_obj;

typedef struct
{
    long int        x;
    long int        y;
    int             sm;
} starxy;

typedef struct
{
    double  ra;
    double  dec;
} radec;

typedef struct
{
    double x;
    double y;
    double z;
} point3;

typedef struct
{
    gdFontPtr font;
    int white,
        black,
        red,
        green,
        darkgreen,
        blue,
        yellow;
} GifParams;

typedef GifParams * GifParamsPtr;

typedef struct
{
    double 	RA_center,
    Dec_center,
    zoomk,
    st_maglim,
    gal_maglim,
    st_mlpr_b,
    st_mlpr_f,
    RA_grid_cellsize,
    Dec_grid_cellsize,
    imzoom;
    int	SIZEX,
        SIZEY,
        DX,
        DY,
        pdf,
        display_grids,
        display_constbnd,
        print_starsmag,
        display_galaxies,
        suppress_unkmag_gal,
        print_galngc,
        print_galic,
        print_galugc,
        print_galpgc,
        print_galmag,
        mirrorx,
        mirrory,
        plot_add_data;
#ifdef USE_HL
    int display_hlobjects;
    PGconn *hl_conn;
#endif
    char *title,
        *plotlabels;
    GifParamsPtr gifpar;
} MapParams;

typedef MapParams * MapParamsPtr;

#endif

#ifndef __SKYC_H__
#define __SKYC_H__

#include "gd.h"
#include "cpdflib.h"
#include "skyc_readdata.h"
/*Function prototypes*/

/*draw_functions.c*/
void Draw_Ellipse(gdImagePtr im, double x, double y, double firad, double a, double b, int el_color);
void PDF_String(CPDFdoc *pdf, char *fontfam, char *fontenc,
               double fontsize, double x, double y, char *text);
void PDF_Line(CPDFdoc *pdf, double x1, double y1, double x2, double y2);
void Draw_Arc_pdf(CPDFdoc *pdf, double x, double y, double firad,
              double a, double b, int draw_direction, int moveto_arg);
void gdImageCircleBnd(gdImagePtr im, long int cx, long int cy, 
                      long int w, long int h,
                      long int x1, long int y1, long int x2, long int y2,
                      int color);
void gdImageLineBnd(gdImagePtr im, int x1, int y1, int x2, int y2,
                    int xb1, int yb1, int xb2, int yb2, int color);
void small_circle(gdImagePtr im,int x,int y,int d,int color);
void PlotCircle(MapParamsPtr skymap_par, void *map,int x,int y,int r,int color,int xbrd,int ybrd);
void plot_marker(gdImagePtr im,int x,int y,int mtype, int msize,int color);

/*coord.c*/
int CoordType(MapParamsPtr skymap_par);
starxy CompEquCoord(MapParamsPtr skymap_par, double alpha, double delta, int sm);
starxy CompMidCoord(MapParamsPtr skymap_par, double alpha, double delta, int sm);
starxy CompPolCoord(MapParamsPtr skymap_par, double alpha, double delta, int sm);
int CompCoordinates(MapParamsPtr skymap_par, double *radec, short int *mag, int **x, int **y, short int **r, long int obj_n, char comp_rad);

/*galaxies.c*/
void PlotGal(MapParamsPtr skymap_par,void *map,int x,int y,double fi,int Asiz,int Bsiz,int color);
void PlotGCl(MapParamsPtr skymap_par,void *map,int x,int y,int siz,int color);
void PlotOCl(MapParamsPtr skymap_par,void *map,int x,int y,int siz,int color);
void PlotPLN(MapParamsPtr skymap_par,void *map,int x,int y,int siz,int color);
void PlotDN(MapParamsPtr skymap_par,void *map,int x,int y,int siz,int color);
void CompAndPlotGalaxies(MapParamsPtr skymap_par, void *map, gal_rc3 *rc3, long int gal_numb);
void PlotCross(MapParamsPtr skymap_par,void *map,int x,int y,int siz,int color);

/*pol_init.c*/
void pol_init(MapParamsPtr skymap_par,void *map,int color,int draw_labels);

/*mid_init.c*/
void mid_init(MapParamsPtr skymap_par,void *map,int color,int draw_labels);

/*equ_init.c*/
void equ_init(MapParamsPtr skymap_par,void *map,int color,int draw_labels);

/*boundaries.c*/
double *read_boundaries();
void PlotConstBoundaries(MapParamsPtr skymap_par, void *map, double *conbnd);

/*prepare.c*/
gdImagePtr PrepareImageGif(MapParamsPtr skymap_par);
CPDFdoc *PrepareImagePDF(MapParamsPtr skymap_par);

/*misc.c*/
void print_gif_header();
void print_pdf_header(long length);
void soft_exit(MapParamsPtr skymap_par, void *map, char *msg);

/*otherobjs.c*/
void PlotObject(MapParamsPtr skymap_par,void *map,int x,int y,plotable_obj obj,int color);
void CompAndPlotObjects(MapParamsPtr skymap_par, void *map, plotable_obj *objlist, long int obj_numb);

/*add_obj_button.c*/
char *add_obj_button(char *alpha_s, char *delta_s, char *objname);

#ifdef USE_HL
/*hl_fetch.c*/
int hl_fetch(MapParamsPtr skymap_par, gal_rc3 **hl_gal);
int HL_connect(MapParamsPtr skymap_par);
void HL_disconnect(MapParamsPtr skymap_par);

#endif

#endif

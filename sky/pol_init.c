#include <stdlib.h>
#include <math.h>
#include "skyc.h"

void pol_init(MapParamsPtr skymap_par,void *map,int color,int draw_labels)
{
    double i,ycur;
    long int yc,yc0;
    int alh_p,alm_p;
    unsigned char *sss;
    double RA_center_print,als_p,x_bc,y_bc,alpha_diff,corn_const_hr,corn_consts_hr[4];
    char degchar;
    double xdir,ydir;
    long int ytop,ybottom,xleft,xright;

    gdImagePtr im;
    CPDFdoc *pdf;
    im=(gdImagePtr)map;
    pdf=(CPDFdoc *)map;

    sss=(unsigned char *)malloc(256);
    x_bc=0; y_bc=0;

    degchar = (skymap_par->pdf==1)? 161 : 7;
    xdir = (skymap_par->mirrorx==1)? -1.0 : 1.0;
    ydir = (skymap_par->mirrory==1)? -1.0 : 1.0;
    xleft = (skymap_par->mirrorx==1)? skymap_par->SIZEX-40 : 2;
    xright = (skymap_par->mirrorx==1)? 2: skymap_par->SIZEX-40;
    ytop = (skymap_par->mirrory==1)? skymap_par->SIZEY-20 : 2;
    ybottom = (skymap_par->mirrory==1)? 2: skymap_par->SIZEY-20;


    corn_const_hr=12.*atan(((double)skymap_par->SIZEX-84.)/((double)skymap_par->SIZEY-40.))/Pi;
    if (skymap_par->Dec_center>0)
    {
        yc = 480*skymap_par->imzoom-ydir*((long int)(17.0*skymap_par->imzoom*skymap_par->zoomk*(90-skymap_par->Dec_center)));
        yc0 = 480*skymap_par->imzoom-((long int)(17.0*skymap_par->imzoom*skymap_par->zoomk*(90-skymap_par->Dec_center)));
        corn_consts_hr[0]=-12.*atan2(20.-(double)yc0,(double)skymap_par->SIZEX/2.-42.)/Pi;
        corn_consts_hr[1]=-12.*atan2(20.-(double)yc0,42.-(double)skymap_par->SIZEX/2.)/Pi;
        corn_consts_hr[2]=-12.*atan2((double)skymap_par->SIZEY-20.-(double)yc0,42.-(double)skymap_par->SIZEX/2.)/Pi;
        corn_consts_hr[3]=-12.*atan2((double)skymap_par->SIZEY-20.-(double)yc0,(double)skymap_par->SIZEX/2.-42.)/Pi;

        if (corn_consts_hr[0]<0) corn_consts_hr[0]+=24.;
        if (corn_consts_hr[1]<0) corn_consts_hr[1]+=24.;
        if (corn_consts_hr[2]<0) corn_consts_hr[2]+=24.;
        if (corn_consts_hr[3]<0) corn_consts_hr[3]+=24.;
        if (yc0<20) corn_consts_hr[0]-=24.;
        for (i=0;i<=100/(skymap_par->Dec_grid_cellsize*skymap_par->zoomk);i++)
        {
            if ((skymap_par->Dec_grid_cellsize*17*i*skymap_par->imzoom*skymap_par->zoomk)<skymap_par->SIZEX*1.4){
                if (draw_labels==0)
                {
                    if (skymap_par->pdf!=1) {
                        gdImageCircleBnd(im,640*skymap_par->imzoom,yc,2*(long int)(skymap_par->Dec_grid_cellsize*17.0*i*skymap_par->imzoom*skymap_par->zoomk),
                                         2*(long int)(skymap_par->Dec_grid_cellsize*17.0*i*skymap_par->imzoom*skymap_par->zoomk),41,19,skymap_par->SIZEX-42,skymap_par->SIZEY-20,color);
                    } else {
                        cpdf_circle(pdf,640.0*skymap_par->imzoom,yc,(long int)(skymap_par->Dec_grid_cellsize*17.0*i*skymap_par->imzoom*skymap_par->zoomk));
                    }
                } else
                {
                    int deld,delm;
                    deld=(int)(90.-skymap_par->Dec_grid_cellsize*i);
                    delm=abs(rint(60*((90.-skymap_par->Dec_grid_cellsize*i)-(int)(90.-skymap_par->Dec_grid_cellsize*i))));
                    if ((rint(skymap_par->Dec_grid_cellsize)==skymap_par->Dec_grid_cellsize)&&(delm==0))
                        sprintf(sss,"+%02d%c",deld,degchar);
                    else
                        sprintf(sss,"+%02d%c%02d'",deld,degchar,delm);
                    if (skymap_par->pdf!=1) {
                        ycur=yc+ydir*(long int)(skymap_par->Dec_grid_cellsize*17.0*i*skymap_par->imzoom*skymap_par->zoomk);
                        if ((ycur>19)&&(ycur<skymap_par->SIZEY-20))
                            gdImageString(im,skymap_par->gifpar->font,640*skymap_par->imzoom+5,
                                          ycur,sss,color);
                        ycur=yc-ydir*(long int)(skymap_par->Dec_grid_cellsize*17.0*i*skymap_par->imzoom*skymap_par->zoomk);
                        if ((ycur>19)&&(ycur<skymap_par->SIZEY-20))
                            gdImageString(im,skymap_par->gifpar->font,640*skymap_par->imzoom+5,
                                          ycur,sss,color);
                    } else {
                        ycur=yc+ydir*(long int)(skymap_par->Dec_grid_cellsize*17.0*i*skymap_par->imzoom*skymap_par->zoomk);
                        if ((ycur>19)&&(ycur<skymap_par->SIZEY-20))
                            PDF_String(pdf,"Times-Roman","MacRomanEncoding",10.0,640*skymap_par->imzoom+5,
                                       ycur, sss);
                        ycur=yc-ydir*(long int)(skymap_par->Dec_grid_cellsize*17.0*i*skymap_par->imzoom*skymap_par->zoomk);
                        if ((ycur>19)&&(ycur<skymap_par->SIZEY-20))
                            PDF_String(pdf,"Times-Roman","MacRomanEncoding",10.0,640*skymap_par->imzoom+5,
                                       ycur, sss);
                    }
                }
            }
        }

        for (i=0;i<=360/skymap_par->RA_grid_cellsize;i++)
        {
            RA_center_print=(int)skymap_par->RA_center-skymap_par->RA_grid_cellsize*(double)i/15.;
            if (RA_center_print<0) RA_center_print+=24.;
            if (RA_center_print>=24) RA_center_print-=24.;
            alh_p=(int)RA_center_print;
            alm_p=abs((int)(60.*(RA_center_print-alh_p)));
            als_p=fabs((3600.*(RA_center_print-(double)alh_p-(double)alm_p/60.)));
            if (draw_labels==0)
            {
                if (skymap_par->pdf!=1) {
                    gdImageLineBnd(im,640*skymap_par->imzoom,yc,
                                   (long int)(640*skymap_par->imzoom+xdir*(2000*skymap_par->imzoom*sin((skymap_par->RA_center-RA_center_print)*Pi/12.))),
                                   (long int)(yc+ydir*(2000*skymap_par->imzoom*cos((skymap_par->RA_center-RA_center_print)*Pi/12.)))
                                   ,41,19,skymap_par->SIZEX-42,skymap_par->SIZEY-20,color);
                } else {
                    PDF_Line(pdf,640*skymap_par->imzoom,yc,
                             (long int)(640*skymap_par->imzoom+xdir*(2000*skymap_par->imzoom*sin((skymap_par->RA_center-RA_center_print)*Pi/12.))),
                             (long int)(yc+ydir*(2000*skymap_par->imzoom*cos((skymap_par->RA_center-RA_center_print)*Pi/12.))));
                }
            } else
            {
                als_p=0.0;
                if (((int)als_p==0)&&(alm_p==0)&&(rint((skymap_par->RA_grid_cellsize/15.0))==(skymap_par->RA_grid_cellsize/15.0)))
                    sprintf(sss,"%02dh",alh_p);
                else if (rint((skymap_par->RA_grid_cellsize/15.0))!=(skymap_par->RA_grid_cellsize/15.0)) {
                    if (((int)als_p==0)&&(rint(60.0*(skymap_par->RA_grid_cellsize/15.0))-60.0*(skymap_par->RA_grid_cellsize/15.0)<0.0001))
                        sprintf(sss,"%02dh%02dm",alh_p,alm_p);
                    else
                        sprintf(sss,"%02dh%02dm%02ds",alh_p,alm_p,(int)(als_p));
                }
                //sprintf(sss,"%02dh%02dm",alh_p,alm_p);
                alpha_diff=skymap_par->RA_center-RA_center_print-6.;
                if (alpha_diff>=corn_consts_hr[3]) alpha_diff-=24.;
                if (alpha_diff>=corn_consts_hr[3]) alpha_diff-=24.;
                if (alpha_diff<(corn_consts_hr[3]-24.)) alpha_diff+=24.;
                if (alpha_diff<(corn_consts_hr[3]-24.)) alpha_diff+=24.;
                if ((alpha_diff>=(corn_consts_hr[0]))&&(alpha_diff<corn_consts_hr[1]))
                {
                    x_bc=640.*skymap_par->imzoom+xdir*(((double)yc0-20.)/tan(alpha_diff*Pi/12.))-15.;
                    y_bc=ytop;
                    if (yc0<20) y_bc=-20.;
                }
                if ((alpha_diff>=corn_consts_hr[1])&&(alpha_diff<corn_consts_hr[2]))
                {
                    x_bc=xleft; if (skymap_par->pdf==1) x_bc-=16;
                    y_bc=(double)yc+ydir*(640.*skymap_par->imzoom-42.)*tan(alpha_diff*Pi/12.);
                }
                if ((alpha_diff>=corn_consts_hr[2])&&(alpha_diff<corn_consts_hr[3]))
                {
                    x_bc=640.*skymap_par->imzoom-xdir*(((double)skymap_par->SIZEY-20.-(double)yc0)/tan(alpha_diff*Pi/12.))-15.;
                    y_bc=ybottom;
                }
                if ((alpha_diff>=(corn_consts_hr[3]-24.))&&(alpha_diff<corn_consts_hr[0]))
                {
                    x_bc=xright;
                    y_bc=(double)yc-ydir*(640.*skymap_par->imzoom-42.)*tan(alpha_diff*Pi/12.);
                }
                if ((x_bc>-15)&&(x_bc<skymap_par->SIZEX)&&(y_bc>-1)&&(y_bc<skymap_par->SIZEY))
                {
                    if (skymap_par->pdf!=1) {
                        gdImageString(im,skymap_par->gifpar->font,x_bc,y_bc,sss,color);
                    } else {
                        PDF_String(pdf,"Times-Roman","MacRomanEncoding",10.0,
                                  x_bc,y_bc,sss);
                    }
                }
            }
        }
    } else if (skymap_par->Dec_center<0)
    {
        yc = 480*skymap_par->imzoom+ydir*(long int)(17.0*skymap_par->zoomk*skymap_par->imzoom*(90+skymap_par->Dec_center));
        yc0 = 480*skymap_par->imzoom+(long int)(17.0*skymap_par->zoomk*skymap_par->imzoom*(90+skymap_par->Dec_center));
        corn_consts_hr[3]=12.*atan2(20.-(double)yc0,(double)skymap_par->SIZEX/2.-42.)/Pi;
        corn_consts_hr[2]=12.*atan2(20.-(double)yc0,42.-(double)skymap_par->SIZEX/2.)/Pi;
        corn_consts_hr[1]=12.*atan2((double)skymap_par->SIZEY-20.-(double)yc0,42.-(double)skymap_par->SIZEX/2.)/Pi;
        corn_consts_hr[0]=12.*atan2((double)skymap_par->SIZEY-20.-(double)yc0,(double)skymap_par->SIZEX/2.-42.)/Pi;

        if (corn_consts_hr[0]<0) corn_consts_hr[0]+=24.;
        if (corn_consts_hr[1]<0) corn_consts_hr[1]+=24.;
        if (corn_consts_hr[2]<0) corn_consts_hr[2]+=24.;
        if (corn_consts_hr[3]<0) corn_consts_hr[3]+=24.;
        if (yc0>skymap_par->SIZEY-20.) corn_consts_hr[0]-=24.;

        for (i=0;i<=100/(skymap_par->Dec_grid_cellsize*skymap_par->zoomk);i++)
        {
            if ((skymap_par->Dec_grid_cellsize*17*i*skymap_par->imzoom*skymap_par->zoomk)<skymap_par->SIZEX*1.4){
                if (draw_labels==0)
                {
                    if (skymap_par->pdf!=1) {
                        gdImageCircleBnd(im,640*skymap_par->imzoom,yc,2*(long int)(skymap_par->Dec_grid_cellsize*skymap_par->imzoom*17.0*i*skymap_par->zoomk),
                                         2*(long int)(skymap_par->Dec_grid_cellsize*17.0*i*skymap_par->imzoom*skymap_par->zoomk),41,19,skymap_par->SIZEX-42,skymap_par->SIZEY-20,color);
                    }else {
                        cpdf_circle(pdf,640.0*skymap_par->imzoom,yc,(long int)(skymap_par->Dec_grid_cellsize*skymap_par->imzoom*17.0*i*skymap_par->zoomk));
                    }
                } else
                {
                    int deld,delm;
                    deld=(int)(90.-skymap_par->Dec_grid_cellsize*i);
                    delm=abs(rint(60*((90.-skymap_par->Dec_grid_cellsize*i)-(int)(90.-skymap_par->Dec_grid_cellsize*i))));
                    if ((rint(skymap_par->Dec_grid_cellsize)==skymap_par->Dec_grid_cellsize)&&(delm==0))
                        sprintf(sss,"-%02d%c",deld,degchar);
                    else
                        sprintf(sss,"-%02d%c%02d'",deld,degchar,delm);
                    if (skymap_par->pdf!=1) {
                        ycur=yc+ydir*(long int)(skymap_par->Dec_grid_cellsize*17.0*i*skymap_par->imzoom*skymap_par->zoomk);
                        if ((ycur>19)&&(ycur<skymap_par->SIZEY-20))
                            gdImageString(im,skymap_par->gifpar->font,640*skymap_par->imzoom+5,
                                          ycur,sss,color);
                        ycur=yc-ydir*(long int)(skymap_par->Dec_grid_cellsize*17.0*i*skymap_par->imzoom*skymap_par->zoomk);
                        if ((ycur>19)&&(ycur<skymap_par->SIZEY-20))
                            gdImageString(im,skymap_par->gifpar->font,640*skymap_par->imzoom+5,
                                          ycur,sss,color);
                    }else {
                        ycur=yc+ydir*(long int)(skymap_par->Dec_grid_cellsize*17.0*i*skymap_par->imzoom*skymap_par->zoomk);
                        if ((ycur>19)&&(ycur<skymap_par->SIZEY-20))
                            PDF_String(pdf,"Times-Roman","MacRomanEncoding",10.0,640*skymap_par->imzoom+5,
                                       ycur,sss);
                        ycur=yc-ydir*(long int)(skymap_par->Dec_grid_cellsize*17.0*i*skymap_par->imzoom*skymap_par->zoomk);
                        if ((ycur>19)&&(ycur<skymap_par->SIZEY-20))
                        PDF_String(pdf,"Times-Roman","MacRomanEncoding",10.0,640*skymap_par->imzoom+5,
                                      ycur,sss);
                    }
                }
            }
        }
        for (i=0;i<=360/skymap_par->RA_grid_cellsize;i++)
        {
            RA_center_print=(int)skymap_par->RA_center-skymap_par->RA_grid_cellsize*(double)i/15.;
            if (RA_center_print<0) RA_center_print+=24.;
            if (RA_center_print>=24) RA_center_print-=24.;
            alh_p=(int)RA_center_print;
            alm_p=abs((int)(60.*(RA_center_print-alh_p)));
            als_p=fabs((3600.*(RA_center_print-(double)alh_p-(double)alm_p/60.)));
            if (draw_labels==0)
            {
                if (skymap_par->pdf!=1) {
                    gdImageLineBnd(im,640*skymap_par->imzoom,yc,
                                   (long int)(640*skymap_par->imzoom+xdir*(2000*skymap_par->imzoom*sin((skymap_par->RA_center-RA_center_print)*Pi/12.))),
                                   (long int)(yc-ydir*(2000*skymap_par->imzoom*cos((skymap_par->RA_center-RA_center_print)*Pi/12.))),
                                   41,19,skymap_par->SIZEX-42,skymap_par->SIZEY-20,color);
                }else {
                    PDF_Line(pdf,640*skymap_par->imzoom,yc,
                             (long int)(640*skymap_par->imzoom+xdir*(2000*skymap_par->imzoom*sin((skymap_par->RA_center-RA_center_print)*Pi/12.))),
                             (long int)(yc-ydir*(2000*skymap_par->imzoom*cos((skymap_par->RA_center-RA_center_print)*Pi/12.))));
                }
            } else
            {
                als_p=0.0;
                if (((int)als_p==0)&&(alm_p==0)&&(rint((skymap_par->RA_grid_cellsize/15.0))==(skymap_par->RA_grid_cellsize/15.0)))
                    sprintf(sss,"%02dh",alh_p);
                else if (rint((skymap_par->RA_grid_cellsize/15.0))!=(skymap_par->RA_grid_cellsize/15.0)) {
                    if (((int)als_p==0)&&(rint(60.0*(skymap_par->RA_grid_cellsize/15.0))-60.0*(skymap_par->RA_grid_cellsize/15.0)<0.0001))
                        sprintf(sss,"%02dh%02dm",alh_p,alm_p);
                    else
                        sprintf(sss,"%02dh%02dm%02ds",alh_p,alm_p,(int)(als_p));
                }
                //sprintf(sss,"%02dh%02dm",alh_p,alm_p);
                alpha_diff=skymap_par->RA_center-RA_center_print-6.;
                if (alpha_diff>=corn_consts_hr[3]) alpha_diff-=24.;
                if (alpha_diff>=corn_consts_hr[3]) alpha_diff-=24.;
                if (alpha_diff<(corn_consts_hr[3]-24.)) alpha_diff+=24.;
                if (alpha_diff<(corn_consts_hr[3]-24.)) alpha_diff+=24.;
                if ((alpha_diff>=(corn_consts_hr[0]))&&(alpha_diff<corn_consts_hr[1]))
                {
                    x_bc=640.*skymap_par->imzoom+xdir*(((double)skymap_par->SIZEY-20.-(double)yc0)/tan(alpha_diff*Pi/12.))-15.;
                    y_bc=ybottom;
                    if (yc>skymap_par->SIZEY-20.) y_bc=skymap_par->SIZEY+20;
                }
                if ((alpha_diff>=corn_consts_hr[1])&&(alpha_diff<corn_consts_hr[2]))
                {
                    x_bc=xleft; if (skymap_par->pdf==1) x_bc-=16;
                    y_bc=(double)yc-ydir*(640.*skymap_par->imzoom-42.)*tan(alpha_diff*Pi/12.);
                }
                if ((alpha_diff>=corn_consts_hr[2])&&(alpha_diff<corn_consts_hr[3]))
                {
                    x_bc=640.*skymap_par->imzoom-xdir*(((double)yc0-20.)/tan(alpha_diff*Pi/12.))-15.;
                    y_bc=ytop;
                }
                if ((alpha_diff>=(corn_consts_hr[3]-24.))&&(alpha_diff<corn_consts_hr[0]))
                {
                    x_bc=xright;
                    y_bc=(double)yc+ydir*(640.*skymap_par->imzoom-42.)*tan(alpha_diff*Pi/12.);
                }
                if ((x_bc>-16)&&(x_bc<skymap_par->SIZEX)&&(y_bc>0)&&(y_bc<skymap_par->SIZEY)) {
                    if (skymap_par->pdf!=1) {
                        gdImageString(im,skymap_par->gifpar->font,x_bc,y_bc,sss,color);
                    } else {
                        PDF_String(pdf,"Times-Roman","MacRomanEncoding",10.0,x_bc,y_bc,sss);
                    }
                }
            }
        }
    }
}

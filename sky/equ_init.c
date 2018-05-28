#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "skyc.h"

void equ_init(MapParamsPtr skymap_par,void *map,int color,int draw_labels)
{
    double i;
    long int xcur,ycur;
    int alh_p,alm_p;
    double xdir,ydir;
    unsigned char *sss;
    double RA_center_print,als_p;
    int check_p=0;
    
    char degchar;

    gdImagePtr im;
    CPDFdoc *pdf;
    im=(gdImagePtr)map;
    pdf=(CPDFdoc *)map;

    degchar = (skymap_par->pdf==1)? 161 : 7;
    sss=(unsigned char *)malloc(256);

    xdir = (skymap_par->mirrorx==1)? -1.0 : 1.0;
    ydir = (skymap_par->mirrory==1)? -1.0 : 1.0;

    for (i=0;i<=100/skymap_par->Dec_grid_cellsize;i++)
    {
        ycur=480*skymap_par->imzoom+ydir*((long int)(17.0*skymap_par->imzoom*skymap_par->zoomk*skymap_par->Dec_center)-(long int)(17.0*skymap_par->imzoom*i*skymap_par->Dec_grid_cellsize*skymap_par->zoomk));
        if ((ycur>=19)&&(ycur<=960*skymap_par->imzoom-20)) {
            if (draw_labels==0)
            {
                if (skymap_par->pdf!=1) {
                    //                gdImageLine(im,0,ycur,1280*skymap_par->imzoom,ycur,color);
                    gdImageLine(im,41,ycur,1280*skymap_par->imzoom-42,ycur,color);
                }else {
                    PDF_Line(pdf,41,ycur,1280*skymap_par->imzoom-42,ycur);
                }
            } else
            {
                int deld,delm;
                deld=(int)(0.0000001+skymap_par->Dec_grid_cellsize*i);
                delm=abs(rint(60.0*((0.0000001+skymap_par->Dec_grid_cellsize*i)-(int)(0.0000001+skymap_par->Dec_grid_cellsize*i))));
                if ((rint(skymap_par->Dec_grid_cellsize)==skymap_par->Dec_grid_cellsize)&&(delm==0))
                        sprintf(sss,"%+02d%c",deld,degchar);
                    else
                        sprintf(sss,"%+02d%c%02d'",deld,degchar,delm);

                if (skymap_par->pdf!=1) {
                    gdImageString(im,skymap_par->gifpar->font,42-6*strlen(sss),ycur,sss,color);
                    gdImageString(im,skymap_par->gifpar->font,skymap_par->SIZEX-41,ycur,sss,color);
                }else{
                    PDF_String(pdf,"Times-Roman","MacRomanEncoding",10.0,24.0-8.0*strlen(sss),ycur,sss);
                    PDF_String(pdf,"Times-Roman","MacRomanEncoding",10.0,skymap_par->SIZEX-42,ycur,sss);
                }
                check_p=1;
            }
        }
        
        ycur=480*skymap_par->imzoom+ydir*((long int)(17.0*skymap_par->imzoom*skymap_par->zoomk*skymap_par->Dec_center)-(long int)(-17.0*skymap_par->imzoom*i*skymap_par->Dec_grid_cellsize*skymap_par->zoomk));
        if ((ycur>=19)&&(ycur<=960*skymap_par->imzoom-20)) {
            if (draw_labels==0)
            {
                if (skymap_par->pdf!=1) {
                    gdImageLine(im,41,ycur,1280*skymap_par->imzoom-42,ycur,color);
                } else {
                    PDF_Line(pdf,41,ycur,1280*skymap_par->imzoom-42,ycur);
                }
            } else
            {
                int deld,delm;
                deld=(int)(0.0000001+skymap_par->Dec_grid_cellsize*i);
                delm=abs(rint(60*((0.0000001+skymap_par->Dec_grid_cellsize*i)-(int)(0.0000001+skymap_par->Dec_grid_cellsize*i))));
                if ((rint(skymap_par->Dec_grid_cellsize)==skymap_par->Dec_grid_cellsize)&&(delm==0))
                        sprintf(sss,"-%02d%c",deld,degchar);
                    else
                        sprintf(sss,"-%02d%c%02d'",deld,degchar,delm);
                if ((i!=0)||(check_p==0)){
                    if (skymap_par->pdf!=1) {
                        gdImageString(im,skymap_par->gifpar->font,42-6*strlen(sss),ycur,sss,color);
                        gdImageString(im,skymap_par->gifpar->font,skymap_par->SIZEX-42,ycur,sss,color);
                    }else{
                        PDF_String(pdf,"Times-Roman","MacRomanEncoding",10.0,24.0-8.0*strlen(sss),ycur,sss);
                        PDF_String(pdf,"Times-Roman","MacRomanEncoding",10.0,skymap_par->SIZEX-42,ycur,sss);
                    }
                }
            }
        }
        check_p=1;
    }
    for (i=0;i<=10.0*10.0/skymap_par->RA_grid_cellsize;i++)
    {
        RA_center_print=(int)skymap_par->RA_center+skymap_par->RA_grid_cellsize*i/15.;

        xcur=640*skymap_par->imzoom+xdir*((long int)(cos(Pi*skymap_par->Dec_center/180.)*255.0*skymap_par->imzoom*skymap_par->zoomk*(skymap_par->RA_center-RA_center_print)));
        if ((xcur>=41)&&(xcur<=1280*skymap_par->imzoom-42)){
            if (draw_labels==0)
            {
                if (skymap_par->pdf!=1) {
                    gdImageLine(im,xcur,19,xcur,960*skymap_par->imzoom-20,color);
                } else{
                    PDF_Line(pdf,xcur,19,xcur,960*skymap_par->imzoom-20);
                }
                } else
            {
                if (RA_center_print<0) RA_center_print+=24.;
                if (RA_center_print>=24) RA_center_print-=24.;

                alh_p=(int)(RA_center_print+0.00000001);
                alm_p=abs((int)(60.*(RA_center_print+0.00000001-alh_p)));
                als_p=fabs((3600.*(RA_center_print+0.00000001-(double)alh_p-(double)alm_p/60.)));
                if (((int)als_p==0)&&(alm_p==0)&&(rint((skymap_par->RA_grid_cellsize/15.0))==(skymap_par->RA_grid_cellsize/15.0)))
                    sprintf(sss,"%02dh",alh_p);
                else if (rint((skymap_par->RA_grid_cellsize/15.0))!=(skymap_par->RA_grid_cellsize/15.0)) {
                    if (((int)als_p==0)&&(rint(60.0*(skymap_par->RA_grid_cellsize/15.0))-60.0*(skymap_par->RA_grid_cellsize/15.0)<0.0001))
                        sprintf(sss,"%02dh%02dm",alh_p,alm_p);
                    else
                        sprintf(sss,"%02dh%02dm%02ds",alh_p,alm_p,(int)(als_p));
                }
                if ((xcur>41)&&(xcur<skymap_par->SIZEX-41))
                {
                    if (skymap_par->pdf!=1) {
                        gdImageString(im,skymap_par->gifpar->font,xcur-3*strlen(sss),5,sss,color);
                        gdImageString(im,skymap_par->gifpar->font,xcur-3*strlen(sss),skymap_par->SIZEY-20,sss,color);
                    }else{
                        PDF_String(pdf,"Times-Roman","MacRomanEncoding",10.0,xcur-4.0*strlen(sss),0,sss);
                        PDF_String(pdf,"Times-Roman","MacRomanEncoding",10.0,xcur-4.0*strlen(sss),skymap_par->SIZEY-20,sss);
                    }
                }
            }
        }

        RA_center_print=(int)skymap_par->RA_center-skymap_par->RA_grid_cellsize*i/15.;
        xcur=640*skymap_par->imzoom+xdir*((long int)(cos(Pi*skymap_par->Dec_center/180.)*255.0*skymap_par->imzoom*skymap_par->zoomk*(skymap_par->RA_center-RA_center_print)));
        if ((xcur>=41)&&(xcur<=1280*skymap_par->imzoom-42)){
            if (draw_labels==0)
            {
                if (skymap_par->pdf!=1) {
                    gdImageLine(im,xcur,19,xcur,960*skymap_par->imzoom-20,color);
                } else {
                    PDF_Line(pdf,xcur,19,xcur,960*skymap_par->imzoom-20);
                }
            } else
            {
                if (RA_center_print<0) RA_center_print+=24.;
                if (RA_center_print>=24) RA_center_print-=24.;

                //alh_p=(int)RA_center_print;
                //alm_p=abs(rint(6000.*(RA_center_print-alh_p))/100);
                //als_p=fabs((3600.*(RA_center_print-(double)alh_p-(double)alm_p/60.)));
                //sprintf(sss,"%02dh%02dm%02ds",alh_p,alm_p,(int)rint(als_p));

                alh_p=(int)(RA_center_print+0.00000001);
                alm_p=abs((int)(60.*(RA_center_print+0.00000001-alh_p)));
                als_p=fabs((3600.*(RA_center_print+0.00000001-(double)alh_p-(double)alm_p/60.)));
                if (((int)als_p==0)&&(alm_p==0)&&(rint((skymap_par->RA_grid_cellsize/15.0))==(skymap_par->RA_grid_cellsize/15.0)))
                    sprintf(sss,"%02dh",alh_p);
                else if (rint((skymap_par->RA_grid_cellsize/15.0))!=(skymap_par->RA_grid_cellsize/15.0)) {
                    if (((int)als_p==0)&&(rint(60.0*(skymap_par->RA_grid_cellsize/15.0))-60.0*(skymap_par->RA_grid_cellsize/15.0)<0.0001))
                        sprintf(sss,"%02dh%02dm",alh_p,alm_p);
                    else
                        sprintf(sss,"%02dh%02dm%02ds",alh_p,alm_p,(int)(als_p));
                }
                if ((xcur>41)&&(xcur<skymap_par->SIZEX-41))
                {
                    if (skymap_par->pdf!=1) {
                        gdImageString(im,skymap_par->gifpar->font,xcur-3*strlen(sss),5,sss,color);
                        gdImageString(im,skymap_par->gifpar->font,xcur-3*strlen(sss),skymap_par->SIZEY-20,sss,color);
                    } else {
                        PDF_String(pdf,"Times-Roman","MacRomanEncoding",10.0,xcur-4.0*strlen(sss),0,sss);
                        PDF_String(pdf,"Times-Roman","MacRomanEncoding",10.0,xcur-4.0*strlen(sss),skymap_par->SIZEY-20,sss);
                    }
                }
            }
        }
    }
}


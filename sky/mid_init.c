#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "skyc.h"

void mid_init(MapParamsPtr skymap_par,void *map,int color,int draw_labels)
{
    double i;
    long int yc, rmain, x_cb=0, y_cb=0;
    long int rcur;
    int albeg,alend,alh_p,alm_p;
    unsigned char *sss;
    double RA_center_print,als_p,RA_center_discret;
    double dlti;
    double xdir,ydir;
    long int ytop,ybottom;
    //long int xleft,xright;

    char degchar;

    gdImagePtr im;
    CPDFdoc *pdf;
    im=(gdImagePtr)map;
    pdf=(CPDFdoc *)map;

    degchar = (skymap_par->pdf==1)? 161 : 7;
    sss=(unsigned char *)malloc(256);
    
    xdir = (skymap_par->mirrorx==1)? -1.0 : 1.0;
    ydir = (skymap_par->mirrory==1)? -1.0 : 1.0;
    //xleft = (skymap_par->mirrorx==1)? 2 : skymap_par->SIZEX-40;
    //xright = (skymap_par->mirrorx==1)? 2 : skymap_par->SIZEX-40;
    //ytop = (skymap_par->mirrory==1)? skymap_par->SIZEY-20 : 2;
    //ybottom = (skymap_par->mirrory==1)? 2 : skymap_par->SIZEY-20;
    ytop = 2;
    ybottom = skymap_par->SIZEY-20;

    if (skymap_par->Dec_center>0)
    {
        rmain = (long int)((90-skymap_par->Dec_center)*17.0*skymap_par->imzoom*skymap_par->zoomk/sin(Pi*skymap_par->Dec_center/180));
        yc = 480*skymap_par->imzoom - ydir*rmain;
        dlti = (skymap_par->Dec_grid_cellsize*(long int)(skymap_par->Dec_center/skymap_par->Dec_grid_cellsize)-skymap_par->Dec_center);
        albeg=89-(int)(180*atan(640*skymap_par->imzoom/abs(yc))/Pi);
        alend=91+(int)(180*atan(640*skymap_par->imzoom/abs(yc))/Pi);
        for (i=0;i<=100/(skymap_par->Dec_grid_cellsize);i++)
        {
            rcur = (long int)(rmain+(skymap_par->Dec_grid_cellsize*i-dlti)*17.0*skymap_par->imzoom*skymap_par->zoomk);
            //fprintf(stderr,"yc=%02d rcur=%02d\n",(int)yc,(int)rcur);
            if ((rcur>=0)&&((yc+ydir*rcur)>=0)&&((yc+ydir*rcur)<=skymap_par->SIZEY*1.4)) {
                //fprintf(stderr,"after if yc=%02d rcur=%02d\n",(int)yc,(int)rcur);

                if (draw_labels==0)
                {
                    //fprintf(stderr,"yc=%02d rcur=%02d\n",(int)yc,(int)rcur);
                    if (skymap_par->pdf!=1) {
                        gdImageCircleBnd(im,640*skymap_par->imzoom,yc,2*rcur,2*rcur,41,19,skymap_par->SIZEX-42,skymap_par->SIZEY-20,color);
                    } else {
                        cpdf_circle(pdf,640.0*skymap_par->imzoom,(double)yc,(double)rcur);
                    }
                } else
                {
                    int deld,delm;
                    deld=(int)(0.0000001+skymap_par->Dec_grid_cellsize*((long int)(skymap_par->Dec_center/skymap_par->Dec_grid_cellsize)-i));
                    delm=abs((int)(60*(0.0000001+skymap_par->Dec_grid_cellsize*((long int)(skymap_par->Dec_center/skymap_par->Dec_grid_cellsize)-i)-
                                          (int)(0.0000001+skymap_par->Dec_grid_cellsize*((long int)(skymap_par->Dec_center/skymap_par->Dec_grid_cellsize)-i)))));
                    if ((rint(skymap_par->Dec_grid_cellsize)==skymap_par->Dec_grid_cellsize)&&(delm==0))
                        sprintf(sss,"%+02d%c",deld,degchar);
                    else
                        sprintf(sss,"%+02d%c%02d'",deld,degchar,delm);
                    y_cb=yc+ydir*(sqrt((double)rcur*(double)rcur-
                                 (double)(42-640*skymap_par->imzoom)*(double)(42-640*skymap_par->imzoom)));
                    if ((y_cb>20)&&(y_cb<(skymap_par->SIZEY-19))){
                        if (skymap_par->pdf!=1) {
                            gdImageString(im,skymap_par->gifpar->font,42-6*strlen(sss),y_cb-5,sss,color);
                        } else {
                            PDF_String(pdf,"Times-Roman","MacRomanEncoding",10.0,24.0-8.0*strlen(sss),y_cb-5,sss);
                        }
                    }
                    y_cb=yc+ydir*(sqrt((double)rcur*(double)rcur-
                                 (double)(skymap_par->SIZEX/2-42)*(double)(skymap_par->SIZEX/2-42)));
                    if ((y_cb>20)&&(y_cb<(skymap_par->SIZEY-19))){
                        if (skymap_par->pdf!=1) {
                            gdImageString(im,skymap_par->gifpar->font,skymap_par->SIZEX-40,y_cb-5,sss,color);
                        } else {
                            PDF_String(pdf,"Times-Roman","MacRomanEncoding",10.0,skymap_par->SIZEX-40,y_cb-5,sss);
                        }
                    }

                }
            }
            rcur = (long int)(rmain+(-skymap_par->Dec_grid_cellsize*i-dlti)*17.0*skymap_par->imzoom*skymap_par->zoomk);
            if ((rcur>=0)&&((yc+ydir*rcur)>=0)&&((yc+ydir*rcur)<=skymap_par->SIZEY*1.4)) {
                if (draw_labels==0)
                {
                    if (skymap_par->pdf!=1) {
                        gdImageCircleBnd(im,640*skymap_par->imzoom,yc,2*rcur,2*rcur,41,19,skymap_par->SIZEX-42,skymap_par->SIZEY-20,color);
                    } else {
                        cpdf_circle(pdf,640.0*skymap_par->imzoom,(double)yc,(double)rcur);
                    }
                } else
                {
                    int deld,delm;
                    deld=(int)(0.0000001+skymap_par->Dec_grid_cellsize*((long int)(skymap_par->Dec_center/skymap_par->Dec_grid_cellsize)+i));
                    delm=abs((int)(60*(0.0000001+skymap_par->Dec_grid_cellsize*((long int)(skymap_par->Dec_center/skymap_par->Dec_grid_cellsize)+i)-
                                       (int)(0.0000001+skymap_par->Dec_grid_cellsize*((long int)(skymap_par->Dec_center/skymap_par->Dec_grid_cellsize)+i)))));
                    if ((rint(skymap_par->Dec_grid_cellsize)==skymap_par->Dec_grid_cellsize)&&(delm==0))
                        sprintf(sss,"%+02d%c",deld,degchar);
                    else
                        sprintf(sss,"%+02d%c%02d'",deld,degchar,delm);
                    y_cb=yc+ydir*sqrt((double)rcur*(double)rcur-
                                 (double)(42-640*skymap_par->imzoom)*(double)(42-640*skymap_par->imzoom));
                    if ((y_cb>20)&&(y_cb<(skymap_par->SIZEY-19))) {
                        if (skymap_par->pdf!=1) {
                            gdImageString(im,skymap_par->gifpar->font,42-6*strlen(sss),y_cb-5,sss,color);
                        } else {
                            PDF_String(pdf,"Times-Roman","MacRomanEncoding",10.0,24.0-8.0*strlen(sss),y_cb-5,sss);
                        }
                    }

                    y_cb=yc+ydir*sqrt((double)rcur*(double)rcur-
                                 (double)(skymap_par->SIZEX/2-42)*(double)(skymap_par->SIZEX/2-42));
                    if ((y_cb>20)&&(y_cb<(skymap_par->SIZEY-19))){
                        if (skymap_par->pdf!=1) {
                            gdImageString(im,skymap_par->gifpar->font,skymap_par->SIZEX-40,y_cb-5,sss,color);
                        } else {
                            PDF_String(pdf,"Times-Roman","MacRomanEncoding",10.0,skymap_par->SIZEX-40,y_cb-5,sss);
                        }
                    }
                }
            }
        }
        for (i=0;i<=90/skymap_par->RA_grid_cellsize;i++)
        {
            if (fabs(sin(Pi*skymap_par->Dec_center/180)*(skymap_par->RA_center-(int)skymap_par->RA_center+skymap_par->RA_grid_cellsize*i/15))<=
                (1.0+180.0*atan(640*skymap_par->imzoom/abs(yc))/Pi)){
                RA_center_print=(int)skymap_par->RA_center-skymap_par->RA_grid_cellsize*(double)i/15.;
                if (draw_labels==0)
                {
                    if (skymap_par->pdf!=1) {
                        gdImageLineBnd(im,640*skymap_par->imzoom,yc,(long int)(640*skymap_par->imzoom+xdir*((rmain+2000*skymap_par->imzoom)*
                                                                               sin(sin(Pi*skymap_par->Dec_center/180)*(skymap_par->RA_center-RA_center_print)*Pi/12.))),
                                       (long int)(yc+ydir*((rmain+2000*skymap_par->imzoom)*cos(sin(Pi*skymap_par->Dec_center/180)*
                                                                                         (skymap_par->RA_center-RA_center_print)*Pi/12.))),
                                       41,19,skymap_par->SIZEX-42,skymap_par->SIZEY-20,color);
                    } else {
                        PDF_Line(pdf,640*skymap_par->imzoom,yc,(long int)(640*skymap_par->imzoom+xdir*((rmain+2000*skymap_par->imzoom)*
                                                                          sin(sin(Pi*skymap_par->Dec_center/180)*(skymap_par->RA_center-RA_center_print)*Pi/12.))),
                                 (long int)(yc+ydir*((rmain+2000*skymap_par->imzoom)*cos(sin(Pi*skymap_par->Dec_center/180)*
                                                                                   (skymap_par->RA_center-RA_center_print)*Pi/12.))));
                    }
                } else
                {
                    RA_center_discret=RA_center_print;
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
                    x_cb=640*skymap_par->imzoom+xdir*ydir*
                        ((rmain*sin(sin(Pi*skymap_par->Dec_center/180)*(skymap_par->RA_center-RA_center_discret)*Pi/12))*
                        (20-yc)/
                        (rmain*cos(sin(Pi*skymap_par->Dec_center/180)*(skymap_par->RA_center-RA_center_discret)*Pi/12.)));
                    if ((x_cb<(skymap_par->SIZEX-40))&&(x_cb>40)) {
                        if (skymap_par->pdf!=1) {
                            gdImageString(im,skymap_par->gifpar->font,x_cb-3*strlen(sss),ytop,sss,color);
                        }else {
                            PDF_String(pdf,"Times-Roman","MacRomanEncoding",10.0,x_cb-4.0*strlen(sss),ytop,sss);
                        }
                    }

                    x_cb=640*skymap_par->imzoom+xdir*ydir*
                        ((rmain*sin(sin(Pi*skymap_par->Dec_center/180)*(skymap_par->RA_center-RA_center_discret)*Pi/12))*
                        (skymap_par->SIZEY-20-yc)/
                        (rmain*cos(sin(Pi*skymap_par->Dec_center/180)*(skymap_par->RA_center-RA_center_discret)*Pi/12.)));
                    if ((x_cb<(skymap_par->SIZEX-40))&&(x_cb>40)){
                        if (skymap_par->pdf!=1) {
                            gdImageString(im,skymap_par->gifpar->font,x_cb-3*strlen(sss),ybottom,sss,color);
                        }else {
                            PDF_String(pdf,"Times-Roman","MacRomanEncoding",10.0,x_cb-4.0*strlen(sss),ybottom,sss);
                        }
                    }
                }
            }
            if (fabs(sin(Pi*skymap_par->Dec_center/180)*(skymap_par->RA_center-(int)skymap_par->RA_center-skymap_par->RA_grid_cellsize*i/15))<=
                (1.0+180.0*atan(640*skymap_par->imzoom/abs(yc))/Pi)){
                RA_center_print=(int)skymap_par->RA_center+skymap_par->RA_grid_cellsize*i/15.;
                if (draw_labels==0)
                {
                    if (skymap_par->pdf!=1) {
                        gdImageLineBnd(im,640*skymap_par->imzoom,yc,(long int)(640*skymap_par->imzoom+xdir*((rmain+2000*skymap_par->imzoom)*
                                                                               sin(sin(Pi*skymap_par->Dec_center/180)*(skymap_par->RA_center-RA_center_print)*Pi/12.))),
                                       (long int)(yc+ydir*((rmain+2000*skymap_par->imzoom)*cos(sin(Pi*skymap_par->Dec_center/180)*
                                                                                         (skymap_par->RA_center-RA_center_print)*Pi/12.))),
                                       41,19,skymap_par->SIZEX-42,skymap_par->SIZEY-20,color);
                    }else {
                        PDF_Line(pdf,640*skymap_par->imzoom,yc,(long int)(640*skymap_par->imzoom+xdir*((rmain+2000*skymap_par->imzoom)*
                                                                               sin(sin(Pi*skymap_par->Dec_center/180)*(skymap_par->RA_center-RA_center_print)*Pi/12.))),
                                       (long int)(yc+ydir*((rmain+2000*skymap_par->imzoom)*cos(sin(Pi*skymap_par->Dec_center/180)*
                                                                                         (skymap_par->RA_center-RA_center_print)*Pi/12.))));
                    }
                } else
                {
                    RA_center_discret=RA_center_print;
                    if (RA_center_print<0) RA_center_print+=24.;
                    if (RA_center_print>=24) RA_center_print-=24.;
                    alh_p=(int)(RA_center_print+0.00000001);
                    alm_p=abs(rint(6000.*(RA_center_print+0.00000001-alh_p))/100);
                    als_p=fabs((3600.*(RA_center_print+0.00000001-(double)alh_p-(double)alm_p/60.)));
                    if (((int)als_p==0)&&(alm_p==0)&&(rint((skymap_par->RA_grid_cellsize/15.0))==(skymap_par->RA_grid_cellsize/15.0)))
                        sprintf(sss,"%02dh",alh_p);
                    else if (rint((skymap_par->RA_grid_cellsize/15.0))!=(skymap_par->RA_grid_cellsize/15.0)) {
                        if (((int)als_p==0)&&(rint(60.0*(skymap_par->RA_grid_cellsize/15.0))-60.0*(skymap_par->RA_grid_cellsize/15.0)<0.0001))
                            sprintf(sss,"%02dh%02dm",alh_p,alm_p);
                        else
                            sprintf(sss,"%02dh%02dm%02ds",alh_p,alm_p,(int)(als_p));
                    }
//                    if ((int)als_p==0)
//                        sprintf(sss,"%02dh%02dm",alh_p,alm_p);
//                    else
//                        sprintf(sss,"%02dh%02dm%02ds",alh_p,alm_p,(int)(als_p));
                    x_cb=640*skymap_par->imzoom+xdir*ydir*
                        ((rmain*sin(sin(Pi*skymap_par->Dec_center/180)*(skymap_par->RA_center-RA_center_discret)*Pi/12))*
                        (20-yc)/
                        (rmain*cos(sin(Pi*skymap_par->Dec_center/180)*(skymap_par->RA_center-RA_center_discret)*Pi/12.)));
                    if ((x_cb<(skymap_par->SIZEX-40))&&(x_cb>40)){
                        if (skymap_par->pdf!=1) {
                            gdImageString(im,skymap_par->gifpar->font,x_cb-3*strlen(sss),ytop,sss,color);
                        } else {
                            PDF_String(pdf,"Times-Roman","MacRomanEncoding",10.0,x_cb-4.0*strlen(sss),ytop,sss);
                        }
                    }
                    x_cb=640*skymap_par->imzoom+xdir*ydir*
                        ((rmain*sin(sin(Pi*skymap_par->Dec_center/180)*(skymap_par->RA_center-RA_center_discret)*Pi/12))*
                        (skymap_par->SIZEY-20-yc)/
                        (rmain*cos(sin(Pi*skymap_par->Dec_center/180)*(skymap_par->RA_center-RA_center_discret)*Pi/12.)));
                    if ((x_cb<(skymap_par->SIZEX-40))&&(x_cb>40)) {
                        if (skymap_par->pdf!=1) {
                            gdImageString(im,skymap_par->gifpar->font,x_cb-3*strlen(sss),ybottom,sss,color);
                        }else{
                            PDF_String(pdf,"Times-Roman","MacRomanEncoding",10.0,x_cb-4.0*strlen(sss),ybottom,sss);
                        }
                    }
                }
            }
        }
    } else if (skymap_par->Dec_center<0)
    {
        rmain = abs((long int)((90+skymap_par->Dec_center)*17.0*skymap_par->imzoom*skymap_par->zoomk/sin(Pi*skymap_par->Dec_center/180)));
        yc = 480*skymap_par->imzoom+ydir*rmain;
        dlti = -skymap_par->Dec_grid_cellsize*(long int)(skymap_par->Dec_center/skymap_par->Dec_grid_cellsize)+skymap_par->Dec_center;
        albeg=269-(int)(180*atan(640*skymap_par->imzoom/abs(yc-960*skymap_par->imzoom))/Pi);
        alend=271+(int)(180*atan(640*skymap_par->imzoom/abs(yc-960*skymap_par->imzoom))/Pi);
        for (i=0;i<=100/(skymap_par->Dec_grid_cellsize);i++)
        {
            rcur = (long int)(rmain+(skymap_par->Dec_grid_cellsize*i-dlti)*skymap_par->imzoom*17.0*skymap_par->zoomk);
            if ((rcur>=0)&&((yc-ydir*rcur)>=-skymap_par->SIZEY*0.4)&&((yc-ydir*rcur)<=skymap_par->SIZEY)){
                if (draw_labels==0)
                {
                    if (skymap_par->pdf!=1) {
                        gdImageCircleBnd(im,640*skymap_par->imzoom,yc,2*rcur,2*rcur,41,19,skymap_par->SIZEX-42,skymap_par->SIZEY-20,color);
                    } else {
                        cpdf_circle(pdf,640*skymap_par->imzoom,yc,rcur);
                    }
                } else
                {
                    int deld,delm;
                    deld=(int)(-0.0000001+skymap_par->Dec_grid_cellsize*((long int)(skymap_par->Dec_center/skymap_par->Dec_grid_cellsize)+i));
                    delm=abs((int)(60*(0.0000001-skymap_par->Dec_grid_cellsize*((long int)(skymap_par->Dec_center/skymap_par->Dec_grid_cellsize)+i)-
                                          (int)(0.0000001-skymap_par->Dec_grid_cellsize*((long int)(skymap_par->Dec_center/skymap_par->Dec_grid_cellsize)+i)))));
                    if ((rint(skymap_par->Dec_grid_cellsize)==skymap_par->Dec_grid_cellsize)&&(delm==0))
                        sprintf(sss,"%+02d%c",deld,degchar);
                    else
                        sprintf(sss,"%+02d%c%02d'",deld,degchar,delm);
                    y_cb=yc-ydir*(sqrt((double)rcur*(double)rcur-
                                 (double)(42-640*skymap_par->imzoom)*(double)(42-640*skymap_par->imzoom)));
                    if ((y_cb>20)&&(y_cb<(skymap_par->SIZEY-19))){
                        if (skymap_par->pdf!=1) {
                            gdImageString(im,skymap_par->gifpar->font,42-6*strlen(sss),y_cb-5,sss,color);
                        } else {
                            PDF_String(pdf,"Times-Roman","MacRomanEncoding",10.0,24.0-8.0*strlen(sss),y_cb-5,sss);
                        }
                    }

                    y_cb=yc-ydir*(sqrt((double)rcur*(double)rcur-
                                 (double)(skymap_par->SIZEX/2-42)*(double)(skymap_par->SIZEX/2-42)));
                    if ((y_cb>20)&&(y_cb<(skymap_par->SIZEY-19))) {
                        if (skymap_par->pdf!=1) {
                            gdImageString(im,skymap_par->gifpar->font,skymap_par->SIZEX-40,y_cb-5,sss,color);
                        } else {
                            PDF_String(pdf,"Times-Roman","MacRomanEncoding",10.0,skymap_par->SIZEX-40,y_cb-5,sss);
                        }
                    }
                }
            }
            rcur = (long int)(rmain+(-skymap_par->Dec_grid_cellsize*i-dlti)*skymap_par->imzoom*17.0*skymap_par->zoomk);
            if ((rcur>=0)&&((yc-ydir*rcur)>=-skymap_par->SIZEY*0.4)&&((yc-ydir*rcur)<=skymap_par->SIZEY)){
                if (draw_labels==0)
                {
                    if (skymap_par->pdf!=1) {
                        gdImageCircleBnd(im,640*skymap_par->imzoom,yc,2*rcur,2*rcur,41,19,skymap_par->SIZEX-42,skymap_par->SIZEY-20,color);
                    }else{
                        cpdf_circle(pdf,640.0*skymap_par->imzoom,yc,rcur);
                    }
                } else
                {
                    int deld,delm;
                    deld=(int)(-0.0000001+skymap_par->Dec_grid_cellsize*((long int)(skymap_par->Dec_center/skymap_par->Dec_grid_cellsize)-i));
                    delm=abs((int)(60*(0.0000001-skymap_par->Dec_grid_cellsize*((long int)(skymap_par->Dec_center/skymap_par->Dec_grid_cellsize)-i)-
                                          (int)(0.0000001-skymap_par->Dec_grid_cellsize*((long int)(skymap_par->Dec_center/skymap_par->Dec_grid_cellsize)-i)))));
                    if ((rint(skymap_par->Dec_grid_cellsize)==skymap_par->Dec_grid_cellsize)&&(delm==0))
                        sprintf(sss,"%+02d%c",deld,degchar);
                    else
                        sprintf(sss,"%+02d%c%02d'",deld,degchar,delm);
                    y_cb=yc-ydir*(sqrt((double)rcur*(double)rcur-
                                 (double)(42-640*skymap_par->imzoom)*(double)(42-640*skymap_par->imzoom)));
                    if ((y_cb>20)&&(y_cb<(skymap_par->SIZEY-19))){
                        if (skymap_par->pdf!=1) {
                            gdImageString(im,skymap_par->gifpar->font,42-6*strlen(sss),y_cb-5,sss,color);
                        }else {
                            PDF_String(pdf,"Times-Roman","MacRomanEncoding",10.0,24.0-8.0*strlen(sss),y_cb-5,sss);
                        }
                    }
                    y_cb=yc-ydir*(sqrt((double)rcur*(double)rcur-
                                 (double)(skymap_par->SIZEX/2-42)*(double)(skymap_par->SIZEX/2-42)));
                    if ((y_cb>20)&&(y_cb<(skymap_par->SIZEY-19))){
                        if (skymap_par->pdf!=1) {
                            gdImageString(im,skymap_par->gifpar->font,skymap_par->SIZEX-40,y_cb-5,sss,color);
                        }else{
                            PDF_String(pdf,"Times-Roman","MacRomanEncoding",10.0,skymap_par->SIZEX-40,y_cb-5,sss);
                        }
                    }
                }
            }
        }
        for (i=0;i<=90/skymap_par->RA_grid_cellsize;i++)
        {
            if (fabs(sin(-Pi*skymap_par->Dec_center/180)*(skymap_par->RA_center-(int)skymap_par->RA_center-skymap_par->RA_grid_cellsize*i/15))<=
                (1.0+180.0*atan(640*skymap_par->imzoom/abs(yc-960*skymap_par->imzoom))/Pi)){
                RA_center_print=(int)skymap_par->RA_center+skymap_par->RA_grid_cellsize*i/15.;
                if (draw_labels==0)
                {
                    if (skymap_par->pdf!=1) {
                    gdImageLineBnd(im,640*skymap_par->imzoom,yc,
                                (long int)(640*skymap_par->imzoom+xdir*((rmain+2000*skymap_par->imzoom)*
                                           sin(sin(-Pi*skymap_par->Dec_center/180)*(skymap_par->RA_center-RA_center_print)*Pi/12.))),
                                (long int)(yc-ydir*((rmain+2000*skymap_par->imzoom)*cos(sin(-Pi*skymap_par->Dec_center/180)*
                                                                                  (skymap_par->RA_center-RA_center_print)*Pi/12.))),
                                   41,19,skymap_par->SIZEX-42,skymap_par->SIZEY-20,color);
                    }else{
                        PDF_Line(pdf,640*skymap_par->imzoom,yc,
                                (long int)(640*skymap_par->imzoom+xdir*((rmain+2000*skymap_par->imzoom)*
                                           sin(sin(-Pi*skymap_par->Dec_center/180)*(skymap_par->RA_center-RA_center_print)*Pi/12.))),
                                (long int)(yc-ydir*((rmain+2000*skymap_par->imzoom)*cos(sin(-Pi*skymap_par->Dec_center/180)*
                                                                                  (skymap_par->RA_center-RA_center_print)*Pi/12.))));
                    }
                } else
                {
                    RA_center_discret=RA_center_print;
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
                    x_cb=640*skymap_par->imzoom+xdir*ydir*
                        ((rmain*sin(sin(-Pi*skymap_par->Dec_center/180)*(skymap_par->RA_center-RA_center_discret)*Pi/12))*
                        (20-yc)/
                        (-rmain*cos(sin(-Pi*skymap_par->Dec_center/180)*(skymap_par->RA_center-RA_center_discret)*Pi/12.)));
                    if ((x_cb<(skymap_par->SIZEX-40))&&(x_cb>40)) {
                        if (skymap_par->pdf!=1) {

                            gdImageString(im,skymap_par->gifpar->font,x_cb-3*strlen(sss),2,sss,color);
                        }else{
                            PDF_String(pdf,"Times-Roman","MacRomanEncoding",10.0,x_cb-4.0*strlen(sss),2,sss);
                        }
                    }
                    x_cb=640*skymap_par->imzoom+xdir*ydir*
                        ((rmain*sin(sin(-Pi*skymap_par->Dec_center/180)*(skymap_par->RA_center-RA_center_discret)*Pi/12))*
                        (skymap_par->SIZEY-20-yc)/
                        (-rmain*cos(sin(-Pi*skymap_par->Dec_center/180)*(skymap_par->RA_center-RA_center_discret)*Pi/12.)));
                    if ((x_cb<(skymap_par->SIZEX-40))&&(x_cb>40)){
                        if (skymap_par->pdf!=1) {
                            gdImageString(im,skymap_par->gifpar->font,x_cb-3*strlen(sss),skymap_par->SIZEY-20,sss,color);
                        }else {
                            PDF_String(pdf,"Times-Roman","MacRomanEncoding",10.0,x_cb-4.0*strlen(sss),skymap_par->SIZEY-20,sss);
                        }
                    }
                }
            }
            if (fabs(sin(-Pi*skymap_par->Dec_center/180)*(skymap_par->RA_center-(int)skymap_par->RA_center-skymap_par->RA_grid_cellsize*i/15))<=
                (1.0+180.0*atan(640*skymap_par->imzoom/abs(yc-960*skymap_par->imzoom))/Pi)){
                RA_center_print=(int)skymap_par->RA_center-skymap_par->RA_grid_cellsize*i/15.;
                if (draw_labels==0)
                {
                    if (skymap_par->pdf!=1) {
                        gdImageLineBnd(im,640*skymap_par->imzoom,yc,
                                       (long int)(640*skymap_par->imzoom+xdir*((rmain+2000*skymap_par->imzoom)*
                                                  sin(sin(-Pi*skymap_par->Dec_center/180)*(skymap_par->RA_center-RA_center_print)*Pi/12.))),
                                       (long int)(yc-ydir*((rmain+2000*skymap_par->imzoom)*cos(sin(-Pi*skymap_par->Dec_center/180)*
                                                                                         (skymap_par->RA_center-RA_center_print)*Pi/12.))),
                                       41,19,skymap_par->SIZEX-42,skymap_par->SIZEY-20,color);
                    }else {
                        PDF_Line(pdf,640*skymap_par->imzoom,yc,
                                       (long int)(640*skymap_par->imzoom+xdir*((rmain+2000*skymap_par->imzoom)*
                                                  sin(sin(-Pi*skymap_par->Dec_center/180)*(skymap_par->RA_center-RA_center_print)*Pi/12.))),
                                       (long int)(yc-ydir*((rmain+2000*skymap_par->imzoom)*cos(sin(-Pi*skymap_par->Dec_center/180)*
                                                                                         (skymap_par->RA_center-RA_center_print)*Pi/12.))));
                    }
                } else
                {
                    RA_center_discret=RA_center_print;
                    if (RA_center_print<0) RA_center_print+=24.;
                    if (RA_center_print>=24) RA_center_print-=24.;
                    alh_p=(int)(RA_center_print+0.00000001);
                    alm_p=abs(rint(6000.*(RA_center_print+0.00000001-alh_p))/100);
                    als_p=fabs((3600.*(RA_center_print+0.00000001-(double)alh_p-(double)alm_p/60.)));
                    if (((int)als_p==0)&&(alm_p==0)&&(rint((skymap_par->RA_grid_cellsize/15.0))==(skymap_par->RA_grid_cellsize/15.0)))
                        sprintf(sss,"%02dh",alh_p);
                    else if (rint((skymap_par->RA_grid_cellsize/15.0))!=(skymap_par->RA_grid_cellsize/15.0)) {
                        if (((int)als_p==0)&&(rint(60.0*(skymap_par->RA_grid_cellsize/15.0))-60.0*(skymap_par->RA_grid_cellsize/15.0)<0.0001))
                            sprintf(sss,"%02dh%02dm",alh_p,alm_p);
                        else
                            sprintf(sss,"%02dh%02dm%02ds",alh_p,alm_p,(int)(als_p));
                    }
                    x_cb=640*skymap_par->imzoom+xdir*ydir*
                        ((rmain*sin(sin(-Pi*skymap_par->Dec_center/180)*(skymap_par->RA_center-RA_center_discret)*Pi/12))*
                        (20-yc)/
                        (-rmain*cos(sin(-Pi*skymap_par->Dec_center/180)*(skymap_par->RA_center-RA_center_discret)*Pi/12.)));
                    if ((x_cb<(skymap_par->SIZEX-40))&&(x_cb>40)){
                        if (skymap_par->pdf!=1) {
                            gdImageString(im,skymap_par->gifpar->font,x_cb-3*strlen(sss),2,sss,color);
                        }else{
                            PDF_String(pdf,"Times-Roman","MacRomanEncoding",10.0,x_cb-4.0*strlen(sss),2,sss);
                        }
                    }
                    x_cb=640*skymap_par->imzoom+xdir*ydir*
                        ((rmain*sin(sin(-Pi*skymap_par->Dec_center/180)*(skymap_par->RA_center-RA_center_discret)*Pi/12))*
                        (skymap_par->SIZEY-20-yc)/
                        (-rmain*cos(sin(-Pi*skymap_par->Dec_center/180)*(skymap_par->RA_center-RA_center_discret)*Pi/12.)));
                    if ((x_cb<(skymap_par->SIZEX-40))&&(x_cb>40)){
                        if (skymap_par->pdf!=1) {
                            gdImageString(im,skymap_par->gifpar->font,x_cb-3*strlen(sss),skymap_par->SIZEY-20,sss,color);
                        }else{
                            PDF_String(pdf,"Times-Roman","MacRomanEncoding",10.0,x_cb-4.0*strlen(sss),skymap_par->SIZEY-20,sss);
                        }
                    }
                }
            }
        }
    }
}

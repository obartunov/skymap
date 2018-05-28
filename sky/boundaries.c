#include <stdlib.h>
#include <math.h>
#include "skyc.h"

double *read_boundaries()
{
    FILE *fcb;
    long int i;
    int cbrd;
    unsigned char cbrec[4];

    double *conbnd=(double *)malloc(26092*sizeof(double));
    //double conbnd[26092]; //without last-1,-1

    if((fcb=fopen(CON_BND,"rb"))==NULL) return NULL;
    for(i=1;i<13046;i++)
    {
        cbrd=fread(cbrec,sizeof(unsigned char),4,fcb);
        conbnd[2*(i-1)+0]=24.000000*((double)cbrec[0]+256*(double)cbrec[1])/65536.00000;
        if ((cbrec[2]+256*cbrec[3]) > 32767)
          conbnd[2*(i-1)+1] = 90.0*((cbrec[2]/32400.0+256.0/32400.0*cbrec[3])-65536.0/32400.0);
        else conbnd[2*(i-1)+1] = 90.0*((cbrec[2]/32400.0+256.0/32400.0*cbrec[3]));
    }
    fclose(fcb);
    return conbnd;
}

void PlotConstBoundaries(MapParamsPtr skymap_par, void *map, double *conbnd)
{
    int *xcoords,*ycoords,d,xx,yy,xx1,yy1,coln;
    short int *radii;
    double aa,dd,aa1,dd1;
    gdImagePtr im;
    CPDFdoc *pdf;
    im=(gdImagePtr)map;
    pdf=(CPDFdoc *)map;

    coln=1;
    CompCoordinates(skymap_par,conbnd,NULL,&xcoords,&ycoords,&radii,CBND,0);
    if (skymap_par->pdf==1) cpdf_newpath(pdf);
    for(d=1;d<CBND;d++)
    {
        xx=(int)(xcoords[d-1]);
        yy=(int)(ycoords[d-1]);
        xx1=(int)(xcoords[d]);
        yy1=(int)(ycoords[d]);

        aa=conbnd[2*(d-1)];
        dd=conbnd[2*(d-1)+1];
        aa1=conbnd[2*d];
        dd1=conbnd[2*d+1];
        if (((aa1*aa1+dd1*dd1)<0.05)||((aa*aa+dd*dd)<0.05))
        {
            xx1=(int)(xcoords[d+coln]);
            yy1=(int)(ycoords[d+coln]);
            coln=d+1;
            d+=2;
        } else
        {
            if ((xx1>-skymap_par->SIZEX*0.3)&&(xx1<(skymap_par->SIZEX*1.3))&&(yy1>-skymap_par->SIZEY*0.4)&&(yy1<(skymap_par->SIZEY*1.4))&&
                (xx>-skymap_par->SIZEX*0.3)&&(xx<(skymap_par->SIZEX*1.3))&&(yy>-skymap_par->SIZEY*0.4)&&(yy<(skymap_par->SIZEY*1.4))) {
                if (skymap_par->pdf!=1) {
                    gdImageLineBnd(im,xx1,yy1,xx,yy,41,19,skymap_par->SIZEX-42,skymap_par->SIZEY-20,skymap_par->gifpar->red);
                } else {
                    PDF_Line(pdf,xx1,yy1,xx,yy);
                }
            }
        }
    }
    if (skymap_par->pdf==1) {
        cpdf_moveto(pdf,0.0,0.0);
        cpdf_stroke(pdf);
    }
}


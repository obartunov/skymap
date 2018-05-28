#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "gd.h"
#include "stmagf.h"

#define Pi 3.1415926535897932

int	SIZEX;
int	SIZEY;
int	DX;
int	DY;
double	zoomk;
int	dgrids;
int     colstars;
int	dcbnd;
double  year0;
double	dlta,dlti;
double	imzoom;
double	xcenter=180,
	ycenter=0,
	radius=180;
double  agrid=15.0;
double  dgrid=10.0;
int	const_bnd;
int     const_lines;
double	xconst[13046],
	yconst[13046],
	xtek,ytek,mtek;
double	conbnd[26092]; //without last-1,-1
int	number,rr;
int     dfigs;
int     dcnam;
int	white, black, red, green, darkgreen, blue, yellow;
int     spectr[7];
int     blue_gr[60];
int     other_colors[181];
int     hrz_map;
gdFontPtr fontl, fonts, fontmb, fontg;
FILE    *ste;

double  ut;
int     day,month,year;
double  longi,lat,azimc,heightc;
double  A11,A12,A21,A22,A23,A31,A32,A33;

double  y00;

starxy ProjectPt(point3 pointc)
{
    starxy ssss1;
    ssss1.x = 30 + 640*imzoom*(1 + (-y00/(-y00+pointc.y))*pointc.x);
    ssss1.y = 30 + 640*imzoom*(1 - (-y00/(-y00+pointc.y))*pointc.z);
    ssss1.sm= 0;
    return ssss1;
}

point3 RotAz(point3 pointc)
{
    point3 pnt0;
    pnt0.x = pointc.x*A11 + pointc.y*A12;
    pnt0.y = pointc.x*A21 + pointc.y*A22 + pointc.z*A23;
    pnt0.z = pointc.x*A31 + pointc.y*A32 + pointc.z*A33;
    return pnt0;
}

void prepare_image(gdImagePtr im)
{
    int h,H;
    int i;
    int az,AZ;
    point3 pnt0,pnt1;
    starxy s1;

        for (i=0;i<60;i++)
        {
            blue_gr[59-i]=gdImageColorAllocate(im, 0, 0, (int)i*2);
        }
        AZ=3600*imzoom;
        for(az=0;az<=AZ;az++)
        {
            H=200*imzoom;
            for (h=0;h<=H;h++)
            {
                pnt0.x= cos(Pi*h/(imzoom*2000.0))*sin(Pi*(az/(double)AZ));
                pnt0.z= sin(Pi*h/(imzoom*2000.0));
                pnt0.y= cos(Pi*h/(imzoom*2000.0))*cos(Pi*(az/(double)AZ));
                pnt1  = RotAz(pnt0);
                if (pnt1.y>0)
                {
                s1=ProjectPt(pnt1);
                gdImageSetPixel(im,s1.x,s1.y,blue_gr[(int)60.0*h/H]);
                }
                pnt0.x= cos(Pi*h/(imzoom*2000.0))*sin(Pi*(-az/(double)AZ));
                pnt0.z= sin(Pi*h/(imzoom*2000.0));
                pnt0.y= cos(Pi*h/(imzoom*2000.0))*cos(Pi*(-az/(double)AZ));
                pnt1  = RotAz(pnt0);
                if (pnt1.y>0)
                {
                    s1=ProjectPt(pnt1);
                    gdImageSetPixel(im,s1.x,s1.y,blue_gr[(int)60.0*h/H]);
                }
            }
        }
}


void allocate_colors(gdImagePtr im)
{
    int i;
    black	    = gdImageColorAllocate(im, 0, 0, 0);
}


int main(int argc, char **argv, char **envp)
{
    gdImagePtr im;
    
    int	u;
    int	d;
    int	xx,yy,xx1,yy1,rr,coln;
    int xcsi,ycsi;
    double
        aa,dd,aa1,dd1;
    point3 pntl;
    
    hrz_map=1;
    y00=-1.0;
    dcnam=1;
    ste =fopen("/dev/stderr","wt");

    if (argc>=2) heightc=atof(argv[1]); else exit(0);
    if (argc>=3) SIZEX=atoi(argv[2]); else exit(0);

    fprintf (ste,"A=%lf h=%lf SIZEX=%d\n",azimc,heightc,SIZEX);

    A11= cos(Pi*azimc/180);
    A12=-sin(Pi*azimc/180);
    //    A13= 0;
    A21= cos(Pi*heightc/180)*sin(Pi*azimc/180);
    A22= cos(Pi*heightc/180)*cos(Pi*azimc/180);
    A23= sin(Pi*heightc/180);
    A31=-sin(Pi*heightc/180)*sin(Pi*azimc/180);
    A32=-sin(Pi*heightc/180)*cos(Pi*azimc/180);
    A33= cos(Pi*heightc/180);

    imzoom=(double)SIZEX/1280.0;
    pntl.x=sin(Pi*azimc/180.0);
    pntl.y=cos(Pi*azimc/180.0);
    pntl.z=0;
    SIZEY=ProjectPt(RotAz(pntl)).y+60;
    im = gdImageCreate(SIZEX+60, SIZEY);
    allocate_colors(im);

    gdImageColorExact(im, 0,0,0);


    prepare_image(im);

    fclose(ste);
//    print_gif_header();
    gdImageGif(im,stdout);
    gdImageDestroy(im);
    return 1;
}

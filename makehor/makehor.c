#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "gd.h"
#include "gdfonts.h"
#include "gdfontt.h"
#include "gdfontl.h"
#include "gdfontg.h"
#include "gdfontmb.h"
#include "stmagf.h"

#define Pi 3.1415926535897932
#define CBND 13046
#define BS_COUNT 9094

FILE	*fbs;
FILE    *ste;

double	alss;
double	delss;
double	stmag;

char    *hor_map_gif;
int	NMAX;
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
double	*bscoord;
unsigned char	*bsmag;
unsigned char	*bsspec;
int	number,rr;
int     dfigs;
int     dcnam;
int	white, black, red, green, darkgreen, blue, yellow;
int     spectr[7];
int     blue_gr[60];
int     other_colors[181];
int     hrz_map;
gdFontPtr fontl, fonts, fontmb, fontg;

double  ut;
int     day,month,year;
double  longi,lat,azimc,heightc;
double  A11,A12,A21,A22,A23,A31,A32,A33;

double  y00;

double S_time;

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

double comp_s_time()
{
    double s_ut0, s_t, s_day;
    long int nday;
    s_day=(year0-1)/year0;
    //osennee ravnodenstvie 23.09.99 v 11:31 UT  ///15.5 UT///
    if (month>9)
    {
        nday=365*(year-99)+(int)(year/4)-24;
        nday+=7;
        if (month>=10) nday+=day;
        if (month>=11) nday+=31;
        if (month>=12) nday+=30;
    }
    if (month<=9)
    {
        nday=365*(year-100)+(int)(year/4)-24;
        nday+=92;
        nday+=7;
        if (month>= 1) nday+=day;
        if (month>= 2) nday+=31;
        if (month>= 3) nday+=28;
        if (month>= 4) nday+=31;
        if (month>= 5) nday+=30;
        if (month>= 6) nday+=31;
        if (month>= 7) nday+=30;
        if (month>= 8) nday+=31;
        if (month>= 9) nday+=31;
    }
    s_ut0=(double)(nday-0.4798611111)*(s_day/15.0);//-0.4798611111;
    s_ut0=s_ut0-(double)24*(int)(s_ut0/24.0)+0.000285;//+0.000284666666667
    if (s_ut0<0) s_ut0+=24.0;
    s_t=s_ut0-longi/15.0+1.00274543*ut;
    s_t=s_t-(double)24*(int)(s_t/24.0);
    if (s_t<0) s_t+=24.0;
//    fprintf(ste,"ut=%lf date=%d/%d/%d\n",ut,day,month,year);
//    fprintf(ste,"longi=%lf nday=%d s_ut0=%lf s_time=%lf\n",longi,nday,s_ut0,s_t);
    return s_t;
}

void draw_horizont(gdImagePtr im)
{
    starxy s1,s11,s12;
    int i,I;
    point3 pnt0,pnt1;
    point3 pnt01,pnt11;
    point3 pnt02,pnt12;
    char *sss1;
    
    sss1=(char *)malloc(sizeof(char)*5);
    I=3600*imzoom;
    for(i=0;i<I;i++)
    {
        pnt0.x= sin(Pi*(i/(double)I));
        pnt0.z= 0;
        pnt0.y= cos(Pi*(i/(double)I));
        pnt1=RotAz(pnt0);
        s1=ProjectPt(pnt1);
        if (pnt1.y>0)
            gdImageSetPixel(im,s1.x,s1.y,white);
        pnt0.x= sin(Pi*(-i/(double)I));
        pnt0.z= 0;
        pnt0.y= cos(Pi*(-i/(double)I));
        pnt1=RotAz(pnt0);
        s1=ProjectPt(pnt1);
        if (pnt1.y>0)
            gdImageSetPixel(im,s1.x,s1.y,white);
    }
    for(i=0;i<I;i++)
    {
        pnt0.x= cos(Pi*(i/(double)I));
        pnt0.y= 0;
        pnt0.z= sin(Pi*(i/(double)I));
        pnt1=pnt0;
        s1=ProjectPt(pnt1);
        gdImageSetPixel(im,s1.x,s1.y,white);
    }

    /***** Southern Point *****/
    pnt0.x= 0;
    pnt0.y= 1;
    pnt0.z= 0;
    pnt1=RotAz(pnt0);
    s1=ProjectPt(pnt1);

    pnt01.x= 0;
    pnt01.y= 1;
    pnt01.z= -(1.0/60.0)/imzoom;
    pnt11=RotAz(pnt01);
    s11=ProjectPt(pnt11);

    if (pnt1.y>=-0.001)
    {
        pnt02.x= 0;
        pnt02.y= 1;
        pnt02.z= -(2.0/60.0)/imzoom;
        pnt12=RotAz(pnt02);
        s12=ProjectPt(pnt12);

        sprintf(sss1,"S");
        gdImageLine(im,s1.x,s1.y,s11.x,s11.y,white);
        gdImageString(im, fonts, s12.x, s12.y, sss1, white);
    }
    
    /***** Nothern Point *****/
    pnt0.x= 0;
    pnt0.y= -1;
    pnt0.z= 0;
    pnt1=RotAz(pnt0);
    s1=ProjectPt(pnt1);

    pnt01.x= 0;
    pnt01.y= -1;
    pnt01.z= -(1.0/60.0)/imzoom;
    pnt11=RotAz(pnt01);
    s11=ProjectPt(pnt11);

    if (pnt1.y>=-0.001)
    {
        pnt02.x= 0;
        pnt02.y= -1;
        pnt02.z= -(2.0/60.0)/imzoom;
        pnt12=RotAz(pnt02);
        s12=ProjectPt(pnt12);

        sprintf(sss1,"N");
        gdImageLine(im,s1.x,s1.y,s11.x,s11.y,white);
        gdImageString(im, fonts, s12.x, s12.y, sss1, white);
    }
    
    /***** Eastern Point *****/
    pnt0.x= -1;
    pnt0.y= 0;
    pnt0.z= 0;
    pnt1=RotAz(pnt0);
    s1=ProjectPt(pnt1);

    pnt01.x= -1;
    pnt01.y= 0;
    pnt01.z= -(1.0/60.0)/imzoom;
    pnt11=RotAz(pnt01);
    s11=ProjectPt(pnt11);

    if (pnt1.y>=-0.001)
    {
        pnt02.x= -1;
        pnt02.y= 0;
        pnt02.z= -(2.0/60.0)/imzoom;
        pnt12=RotAz(pnt02);
        s12=ProjectPt(pnt12);

        sprintf(sss1,"E");
        gdImageLine(im,s1.x,s1.y,s11.x,s11.y,white);
        gdImageString(im, fonts, s12.x, s12.y, sss1, white);
    }
    
    /***** Western Point *****/
    pnt0.x= 1;
    pnt0.y= 0;
    pnt0.z= 0;
    pnt1=RotAz(pnt0);
    s1=ProjectPt(pnt1);

    pnt01.x= 1;
    pnt01.y= 0;
    pnt01.z= -(1.0/60.0)/imzoom;
    pnt11=RotAz(pnt01);
    s11=ProjectPt(pnt11);

    if (pnt1.y>=-0.001)
    {
        pnt02.x= 1;
        pnt02.y= 0;
        pnt02.z= -(2.0/60.0)/imzoom;
        pnt12=RotAz(pnt02);
        s12=ProjectPt(pnt12);

        sprintf(sss1,"W");
        gdImageLine(im,s1.x,s1.y,s11.x,s11.y,white);
        gdImageString(im, fonts, s12.x, s12.y, sss1, white);
    }
    
}

void allocate_colors(gdImagePtr im)
{
    int i;
    black	    = gdImageColorAllocate(im, 0, 0, 0);
//    white	    = gdImageColorAllocate(im, 255, 255, 255);
//    yellow          = gdImageColorAllocate(im, 255, 255, 0);
//    red	            = gdImageColorAllocate(im, 255, 0, 0);
//    green	    = gdImageColorAllocate(im, 0, 255, 0);
//    blue	    = gdImageColorAllocate(im, 0, 0, 255);
//    darkgreen       = gdImageColorAllocate(im, 0, 128, 0);
//    spectr[0]       = gdImageColorAllocate(im, 0, 255, 255);
//    spectr[1]       = gdImageColorAllocate(im, 120, 255, 255);
//    spectr[2]       = gdImageColorAllocate(im, 255, 255, 255);
//    spectr[3]       = gdImageColorAllocate(im, 255, 255, 128);
//    spectr[4]       = gdImageColorAllocate(im, 255, 255, 0);
//    spectr[5]       = gdImageColorAllocate(im, 255, 128, 0);
//    spectr[6]       = gdImageColorAllocate(im, 255, 64, 64);
}

void draw_hrzn_map(gdImagePtr im)
{
    FILE *fgif;
    gdImagePtr him;
    int trnsc;
    int h,H;
    int az,AZ;
    int h1,az1;
    int colpix;
    int maxcol;
    int colcur;
    int newcols=0;
    int kkk;
    int *rh, *gh, *bh;
    point3 pnt0,pnt1;
    starxy s1;

    if (hrz_map==1)
    {
        fgif = fopen(hor_map_gif, "rb");
        him = gdImageCreateFromGif(fgif);
        fclose(fgif);
        trnsc = gdImageGetTransparent(him);
        maxcol = gdImageColorsTotal(him);
        if ((maxcol<0)||(maxcol>256)) exit(1);
        rh=(int *)malloc(sizeof(int)*(maxcol-1));
        gh=(int *)malloc(sizeof(int)*(maxcol-1));
        bh=(int *)malloc(sizeof(int)*(maxcol-1));
        for (kkk=0; kkk<maxcol; kkk++)
        {
            rh[kkk]=gdImageRed(him, kkk);
            gh[kkk]=gdImageGreen(him, kkk);
            bh[kkk]=gdImageBlue(him, kkk);
        }

        newcols=maxcol;
        if (newcols>180) newcols=180;
        for(kkk=0;kkk<newcols;kkk++)
        {
            other_colors[kkk] = gdImageColorAllocate(im,rh[kkk],gh[kkk],bh[kkk]);
        }
//        fprintf(ste,"Transp color index=%d\n",trnsc);
        H = him->sy*10.0*imzoom;
        AZ=him->sx*1.1*imzoom;
        for(az=0;az<AZ;az++)
        {
            for (h=0;h<H;h++)
            {
                colpix=gdImageGetPixel(him,(int)(az/(imzoom*1.1)-1.0),(int)((H-h)/(imzoom*10.0)-1.0));

                if (colpix>179)
                {
                    colcur=179;
                } else colcur=colpix;


                if (colpix!=trnsc)
                {
                    pnt0.x= cos(Pi*h/(H*1.1*18.0))*sin(2*Pi*(az/(double)AZ));
                    pnt0.z= sin(Pi*h/(H*1.1*18.0));
                    pnt0.y= cos(Pi*h/(H*1.1*18.0))*cos(2*Pi*(az/(double)AZ));
                    pnt1  = RotAz(pnt0);
                    if (pnt1.y>0)
                    {
                        s1=ProjectPt(pnt1);
                        gdImageSetPixel(im,s1.x,s1.y,other_colors[colcur]);
                    }
                }
            }
        }
        gdImageDestroy(him);
    }
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
    
    fonts=gdFontSmall;
    fontl=gdFontLarge;
    fontmb=gdFontMediumBold;
    fontg=gdFontGiant;
    
    colstars=1;
    hrz_map=1;
    S_time=5.3;
    y00=-1.0;
    dcnam=1;
    //ste =fopen("/dev/stderr","wt");
    hor_map_gif=(char *)malloc(4096*sizeof(char));

    if (argc>=2) hor_map_gif=argv[1]; else exit(0);
//    fprintf(ste, "%s\n",hor_map_gif);
    if (argc>=3) azimc=atof(argv[2]); else exit(0);
    if (argc>=4) heightc=atof(argv[3]); else exit(0);
    if (argc>=5) SIZEX=atoi(argv[4]); else exit(0);

    fprintf (stderr,"A=%lf h=%lf SIZEX=%d\n",azimc,heightc,SIZEX);
//    d=0;
//    do
//    {
//        d++;
//    }
//    while(strstr(envp[d],"QUERY_STRING")==NULL);
//    sscanf(envp[d],"QUERY_STRING=azimuth=%lf&height=%lf&xs=%d",
//           &azimc,&heightc,&SIZEX);

//    if (year<70) year+=100;
//    year0=365.24219879-0.0000000614*year;
//    S_time=comp_s_time();

    A11= cos(Pi*azimc/180);
    A12=-sin(Pi*azimc/180);
    //    A13= 0;
    A21= cos(Pi*heightc/180)*sin(Pi*azimc/180);
    A22= cos(Pi*heightc/180)*cos(Pi*azimc/180);
    A23= sin(Pi*heightc/180);
    A31=-sin(Pi*heightc/180)*sin(Pi*azimc/180);
    A32=-sin(Pi*heightc/180)*cos(Pi*azimc/180);
    A33= cos(Pi*heightc/180);

    if (stmag>7.5) stmag=7.5;
    imzoom=(double)SIZEX/1280.0;
    pntl.x=sin(Pi*azimc/180.0);
    pntl.y=cos(Pi*azimc/180.0);
    pntl.z=0;
    SIZEY=ProjectPt(RotAz(pntl)).y+60;
    im = gdImageCreate(SIZEX+60, SIZEY);
    allocate_colors(im);

//    gdImageColorExact(im, 0,0,0);


//    im1=gdImageCreate(SIZEX+60, SIZEY);
//    im = gdImageCreate(SIZEX+60, SIZEY);
//    allocate_colors(im);

    gdImageColorTransparent(im, black);
//    gdImageColorExact(im, 0,0,0);

    draw_hrzn_map(im);
//    draw_horizont(im);
//    fclose(ste);

//    print_gif_header();
    gdImageGif(im,stdout);
    gdImageDestroy(im);
    return 1;
}

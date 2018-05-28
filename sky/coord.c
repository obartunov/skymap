#include <stdlib.h>
#include <math.h>
#include "skyc.h"


int CoordType(MapParamsPtr skymap_par)
{
    int chs=1;
    if ((fabs(skymap_par->Dec_center)<2.*skymap_par->zoomk)||(fabs(skymap_par->Dec_center)<=20)) chs=0;
    if ((90.0-fabs(skymap_par->Dec_center))<=(30.000/skymap_par->zoomk)) chs=2;
    return chs;
}

starxy	CompEquCoord(MapParamsPtr skymap_par,
                     double alpha,	double delta,
                     int sm)
{
    starxy stareq;
    double xe, ye, yc;
    double mcur;

    yc = 480*skymap_par->imzoom + (long int)(17.000*(double)skymap_par->zoomk*skymap_par->imzoom*skymap_par->Dec_center);
    if ((skymap_par->RA_center<=3)&&(alpha>=20)) alpha = alpha - 24;
    if ((skymap_par->RA_center>=22)&&(alpha<=3)) alpha = alpha + 24;
    xe = 640*skymap_par->imzoom - (long int)(cos(Pi*skymap_par->Dec_center/180.)*skymap_par->zoomk*skymap_par->imzoom*256.0000*(alpha-skymap_par->RA_center));
    ye = yc - (skymap_par->zoomk*skymap_par->imzoom*17.000*delta);
    mcur = comp_radius(sm,skymap_par->st_maglim);
    stareq.x = xe;
    stareq.y = ye;
    if (skymap_par->mirrorx==1) stareq.x=-stareq.x + skymap_par->SIZEX;
    if (skymap_par->mirrory==1) stareq.y=-stareq.y + skymap_par->SIZEY;
    stareq.sm = mcur;
    return stareq;
}

starxy	CompMidCoord(MapParamsPtr skymap_par,
                     double alpha,	double delta,
                     int sm)
{
    starxy	starmid;
    long int xm, ym, yc;
    long double rmain;
    double xcur,ycur,mcur;

    xm=0; ym=0; yc=0;
    if ((delta<(skymap_par->Dec_center+50/skymap_par->zoomk))||(delta>(skymap_par->Dec_center-50/skymap_par->zoomk))||(0.1*sm<skymap_par->st_maglim))
    {
        if ((skymap_par->RA_center<=6)&&(alpha>=18)) alpha = alpha - 24;
        if ((skymap_par->RA_center>=19)&&(alpha<=6)) alpha = alpha + 24;
        if (skymap_par->Dec_center>0)
        {
            rmain = abs((90-skymap_par->Dec_center)*skymap_par->imzoom*17.0000*skymap_par->zoomk/sin(Pi*skymap_par->Dec_center/180));
            yc = 480*skymap_par->imzoom - rmain;
            xm = 640*skymap_par->imzoom + (rmain+17.0000*skymap_par->imzoom*(double)skymap_par->zoomk*(skymap_par->Dec_center-delta))*sin((skymap_par->RA_center-alpha)*sin(Pi*skymap_par->Dec_center/180)*15.0000*Pi/180);
            ym =  yc + (rmain+17.0000*skymap_par->imzoom*(double)skymap_par->zoomk*(skymap_par->Dec_center-delta))*cos((skymap_par->RA_center-alpha)*sin(Pi*skymap_par->Dec_center/180)*15.0000*Pi/180);
        }
        else if (skymap_par->Dec_center<0)
        {
            rmain = abs((90+skymap_par->Dec_center)*skymap_par->imzoom*17.0000*skymap_par->zoomk/sin(Pi*skymap_par->Dec_center/180));
            yc = 480*skymap_par->imzoom + rmain;
            xm = 640*skymap_par->imzoom + (rmain+17.0000*skymap_par->imzoom*(double)skymap_par->zoomk*(delta-skymap_par->Dec_center))*sin((skymap_par->RA_center-alpha)*sin((-Pi)*skymap_par->Dec_center/180)*15.0*Pi/180);
            ym =  yc - (rmain+17.0000*skymap_par->imzoom*(double)skymap_par->zoomk*(delta-skymap_par->Dec_center))*cos((skymap_par->RA_center-alpha)*sin((-Pi)*skymap_par->Dec_center/180)*15.0*Pi/180);
        }
        xcur = xm;
        ycur = ym;
        mcur = comp_radius(sm,skymap_par->st_maglim);
        starmid.x=xcur;
        starmid.y=ycur;
        if (skymap_par->mirrorx==1) starmid.x=-starmid.x + skymap_par->SIZEX;
        if (skymap_par->mirrory==1) starmid.y=-starmid.y + skymap_par->SIZEY;
        starmid.sm=mcur;
    } else
    {
        starmid.x=-50;
        starmid.y=-50;
    }
    return(starmid);
}

starxy  CompPolCoord(MapParamsPtr skymap_par,
                     double alpha,	double delta,
                     int sm)
{
    starxy starpol;
    long int xp, yp;
    int yc;
    double xcur,ycur,mcur;

    if (skymap_par->Dec_center>0)
    {
        yc = 480*skymap_par->imzoom - 17.000*skymap_par->zoomk*skymap_par->imzoom*(90-skymap_par->Dec_center);
        xp = 640*skymap_par->imzoom - (double)skymap_par->zoomk*skymap_par->imzoom*17.000*(90.000-delta)*sin(Pi*(alpha-skymap_par->RA_center)/12.000);
        yp = yc + (double)skymap_par->zoomk*skymap_par->imzoom*17.000*(90.000-delta)*cos(Pi*(alpha-skymap_par->RA_center)/12.000);
    }
    else
    {
        yc = 480*skymap_par->imzoom + 17.000*skymap_par->zoomk*skymap_par->imzoom*(90+skymap_par->Dec_center);
        xp = 640*skymap_par->imzoom - (double)skymap_par->zoomk*skymap_par->imzoom*17.000*(90.000+delta)*sin(Pi*(alpha-skymap_par->RA_center)/12.000);
        yp = yc - (double)skymap_par->zoomk*skymap_par->imzoom*17.000*(90.000+delta)*cos(Pi*(alpha-skymap_par->RA_center)/12.000);
    }
    xcur = xp;
    ycur = yp;
    mcur = comp_radius(sm,skymap_par->st_maglim);
    starpol.x=xcur;
    starpol.y=ycur;
    if (skymap_par->mirrorx==1) starpol.x=-starpol.x + skymap_par->SIZEX;
    if (skymap_par->mirrory==1) starpol.y=-starpol.y + skymap_par->SIZEY;
    starpol.sm=mcur;
    return(starpol);
}

int CompCoordinates(MapParamsPtr skymap_par, double *radec, short int *mag, int **x, int **y, short int **r, long int obj_n, char comp_rad)
{
    int chs;
    starxy star_cur;
    long int i;
    int m_cur;
    //int *x0,*y0;
    //short int *r0;

    (*x)=(int *)malloc(obj_n*sizeof(int));
    (*y)=(int *)malloc(obj_n*sizeof(int));
    (*r)=(short int *)malloc(obj_n*sizeof(short int));
    chs=1;
    if ((fabs(skymap_par->Dec_center)<2.*skymap_par->zoomk)||(fabs(skymap_par->Dec_center)<=20))
        chs=0;
    if ((90.0-fabs(skymap_par->Dec_center))<=(30.000/skymap_par->zoomk))
        chs=2;

    switch(chs){
    case 0 :
            for(i=1;i<=obj_n;i++)
            {
                m_cur = (comp_rad==1)? abs((int)mag[i-1]) : 255;
                star_cur=CompEquCoord(skymap_par, radec[(i-1)*2], radec[(i-1)*2+1], m_cur);
                (*x)[i-1]=star_cur.x;
                (*y)[i-1]=star_cur.y;
                (*r)[i-1]= (comp_rad==1)? star_cur.sm : -1;
//                fprintf(stderr,"Equ: ra=%lf, dec=%lf, mag=%lf,  x=%d, y=%d, r=%d\n",radec[(i-1)*2], radec[(i-1)*2+1],mag[i-1]/10.0,x0[i-1],y0[i-1],r0[i-1]);
            }
        break;
    case 1 :
            for(i=1;i<=obj_n;i++)
            {
                m_cur = (comp_rad==1)? abs((int)mag[i-1]) : 255;
                star_cur=CompMidCoord(skymap_par, radec[(i-1)*2], radec[(i-1)*2+1], m_cur);
                (*x)[i-1]=star_cur.x;
                (*y)[i-1]=star_cur.y;
                (*r)[i-1]= (comp_rad==1)? star_cur.sm : -1;
//                fprintf(stderr,"Mid: ra=%lf, dec=%lf, mag=%lf,  x=%d, y=%d, r=%d\n",radec[(i-1)*2], radec[(i-1)*2+1],mag[i-1]/10.0,x0[i-1],y0[i-1],r0[i-1]);
            }
        break;
    case 2 :
            for(i=1;i<=obj_n;i++)
            {
                m_cur = (comp_rad==1)? abs((int)mag[i-1]) : 255;
                star_cur=CompPolCoord(skymap_par, radec[(i-1)*2], radec[(i-1)*2+1], m_cur);
                (*x)[i-1]=star_cur.x;
                (*y)[i-1]=star_cur.y;
                (*r)[i-1]= (comp_rad==1)? star_cur.sm : -1;
                //fprintf(stderr,"Pol: x=%d, y=%d, r=%d\n",x0[i-1],y0[i-1],r0[i-1]);
            }
        break;
    }
    return chs;
}

int comp_radius(int sm, double stmag)
{
    int rad_of_star;
    //    rad_of_star = 1 + (int)(1.8*(0+stmag-0.1*(double)sm));
    rad_of_star = 1 + (int)(2.0*(0.5+(double)(int)stmag-0.1*(sm+1)));
    rad_of_star = 1 + (2.0*(int)(0.05+(double)(int)stmag-0.1*(double)sm));
    return(rad_of_star);
}

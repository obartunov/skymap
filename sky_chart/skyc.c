#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <string.h>
#include <cgic.h>

#include "skyc.h"
#include "gd.h"
#include "gdfonts.h"
#include "gdfontt.h"
#include "gdfontl.h"
#include "gdfontg.h"
#include "gdfontmb.h"
#include "constel.h"
#include "cpdflib.h"
#include "stmagf.h"
#include "P_.h"
#include "astro.h"
#include "cpdflib.h"
#include "milkyway.h"

#define Pi 3.1415926535897932
#define CBND 13046
#define BS_COUNT 9094

FILE	*fbs;
FILE    *ste;

double	alss;
double	delss;
double	stmag;

int	pdf_output;
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
int     mw_gr[60];
int     other_colors[255];
int     hrz_map;
int     disp_planets;
int     drawmw;
int     lang;
//double  planets_positions[6][10];
double  planets_ra[10];
double  planets_dec[10];
double  planets_sizes[10];
double  sun_ecl_coords[2];
double  moon_ecl_coords[2];
gdFontPtr fontl, fonts, fontmb, fontg;

double  ut;
int     day,month,year;
double  longi,lati,azimc,heightc;
double  A11,A12,A21,A22,A23,A31,A32,A33;

//double  y00;

double  S_time, mjd2000;

int     theme_n;
int     chosen_const;
double  mark_ra, mark_dec;
int     mark_size = -1;
//char   *theme_n;
char	*query_string_s;

struct timeval timev;
struct timezone timez;
double begin_time, readdata_time, grad_time,
       bounds_time, figs_time, stars_time,
       planets_time, horiz_time, finish_time, process_time ;
int    Dashed[6];

int checkconst=1;
int chconstnum=0;
int chconstarr[89];
/*int *chconstarr[7]={82, 83, 33, 16, 18, 13, 11};*/

void draw_horizont(gdImagePtr im);

       
void print_pdf_header(long length)
{
    printf("Content: sky_map.pdf%c",10);
    printf("Content-disposition: inline; filename=\"sky_map.pdf\"%c",10);
    printf("Content-type: application/pdf%c", 10);
    printf("Content-Length: %ld%c%c", length, 10, 10);
    //printf("Content-type: application/pdf\n");
    //printf("Pragma: no-cachep\n\n");
}
       
void print_gif_header()
{
    printf("Content-type: image/gif\n");
    printf("Pragma: no-cache\n\n");
}

void print_png_header()
{
    printf("Content-type: image/png\n");
    printf("Pragma: no-cache\n\n");
}

void print_jpeg_header()
{
    printf("Content-type: image/jpeg\n");
    printf("Pragma: no-cache\n\n");
}

void get_the_time(double *whichtime)
{
    gettimeofday(&timev,&timez);
    *whichtime=(double)timev.tv_sec+(double)timev.tv_usec/1000000.0;
}

void dummy_gif(int i)
{
    gdImagePtr dummy_im;
    int ccc1,ccc;
    
    dummy_im=gdImageCreate(640,480);
    ccc1=gdImageColorAllocate(dummy_im,0,0,0);
    gdImageColorExact(dummy_im, 0,0,0);
    ccc=gdImageColorAllocate(dummy_im,255,255,255);
    if (i==0) gdImageString(dummy_im,fontg,100,100,"OOPS !!! Data file is corrupted !",ccc);
    if (i==1) gdImageString(dummy_im,fontg,100,100,"OOPS !!! Data file not found !",ccc);
    if (i==2) { gdImageString(dummy_im,fontg,100,100,"OOPS !!! Theme not found !",ccc);
        gdImageString(dummy_im,fontg,100,150,query_string_s,ccc);
    }
    if (i==3) gdImageString(dummy_im,fontg,100,100,"OOPS !!! Gradient not found !",ccc);
    if (i==4) gdImageString(dummy_im,fontg,100,100,"OOPS !!! Horizont not found !",ccc);
    if (i==5) gdImageString(dummy_im,fontg,100,100,"OOPS !!! BAD ENVIRONMENT (no QUERY_STRING )!!!",ccc);
//    if (i==6) gdImageString(dummy_im,fontg,100,100,"OOPS !!! BAD ENVIRONMENT (bad QUERY_STRING )!!!",ccc);
    if (i==6) { gdImageString(dummy_im,fontg,100,100,"OOPS !!! BAD ENVIRONMENT (bad QUERY_STRING )!!!",ccc);
    gdImageString(dummy_im,fontg,100,150,query_string_s,ccc);
    }
    
    print_gif_header();
    gdImageGif(dummy_im,stdout);
    gdImageDestroy(dummy_im);
    exit(-1);
}

void ecl_eq_my(double l_i, double b_i, double *ra_o, double *dec_o)
{
    *ra_o = atan((sin(l_i)*cos(Pi*23.432911/180.)-(tan(b_i)*sin(Pi*23.432911/180.)))/cos(l_i));
    *dec_o = asin(sin(b_i)*cos(Pi*23.432911/180.)+(cos(b_i)*sin(Pi*23.432911/180.)*sin(l_i)));
    if (cos(l_i)<0) *ra_o += Pi;
    if (*ra_o<0) *ra_o+=2.0*Pi;
}

void compute_planets_pos()
{
    int i;
    int err_val;
    double mod_jd;

    double lsn, rsn, bsn;
    double tmpl, tmpb;
    double tmpa, tmpd;

    double xtmp,ytmp,ztmp,rtmp,ltmp,btmp,xtmps,ytmps,ztmps;

    double lpd0,psi0,rp0,rho0,lam,bet,dia,mag, hp;

    cal_mjd(month, day, year+1900, &mod_jd);
//    mod_jd+=(ut-12)/24.0; // a modified-JD is corrected by mjd.c
    mod_jd+=ut/24.0;
//    fprintf(ste,"MJD=%lf\n",mod_jd);
    
    sunpos(mod_jd, &lsn, &rsn, &bsn);
////    sphcart(lsn, bsn, rsn, xtmps, ytmps, ztmps);
//    ecl_eq(mod_jd, lsn, bsn, &tmpa, &tmpd);
    sun_ecl_coords[0]=lsn;
    sun_ecl_coords[1]=bsn;

//    tmpa = atan((sin(lsn)*cos(Pi*23.432911/180.)-(tan(bsn)*sin(Pi*23.432911/180.)))/cos(lsn));
//    tmpd = asin(sin(bsn)*cos(Pi*23.432911/180.)+(cos(bsn)*sin(Pi*23.432911/180.)*sin(lsn)));
//    if (cos(lsn)<0) tmpa += Pi;
//    if (tmpa<0) tmpa+=2.0*Pi;

    ecl_eq_my(lsn, bsn, &tmpa, &tmpd);
    planets_ra[SUN]=tmpa;
    planets_dec[SUN]=tmpd;
    planets_sizes[SUN]=206265.0*(696000.0/(149598000.0*rsn));
//    fprintf(ste,"n=SUN l =%lf b  =%lf\n",180.0*lsn/Pi,180.0*bsn/Pi);
//    fprintf(ste,"n=SUN  ra=%lf dec=%lf\n",12.0*tmpa/Pi,180.0*tmpd/Pi);

    for (i=0;i<=7;i++)
    {
        plans(mod_jd, i, &lpd0, &psi0, &rp0, &rho0, &lam, &bet, &dia, &mag);
        
//        tmpa = atan((sin(lam)*cos(Pi*23.432911/180.)-(tan(bet)*sin(Pi*23.432911/180.)))/cos(lam));
//        tmpd = asin(sin(bet)*cos(Pi*23.432911/180.)+(cos(bet)*sin(Pi*23.432911/180.)*sin(lam)));
//        if (cos(lam)<0) tmpa += Pi;
//        if (tmpa<0) tmpa+=2.0*Pi;

        //eq_ecl(mod_jd, lsn, bsn, &tmpa, &tmpd);
        ecl_eq_my(lam, bet, &tmpa, &tmpd);
        planets_ra[i]=tmpa;
        planets_dec[i]=tmpd;
        planets_sizes[i]=dia/rho0;
//        fprintf(ste,"n=%d l =%lf b  =%lf\n",i,180.0*lam/Pi,180.0*bet/Pi);
//        fprintf(ste,"n=%d ra=%lf dec=%lf\n",i,12.0*tmpa/Pi,180.0*tmpd/Pi);
    }

    moon(mod_jd,&lam,&bet,&hp,&lpd0,&psi0);
    moon_ecl_coords[0]=lam;
    moon_ecl_coords[1]=bet;

//    tmpa = atan((sin(lam)*cos(Pi*23.432911/180.)-(tan(bet)*sin(Pi*23.432911/180.)))/cos(lam));
//    tmpd = asin(sin(bet)*cos(Pi*23.432911/180.)+(cos(bet)*sin(Pi*23.432911/180.)*sin(lam)));
//    if (cos(lam)<0) tmpa += Pi;
//    if (tmpa<0) tmpa+=2.0*Pi;

    ecl_eq_my(lam, bet, &tmpa, &tmpd);
    planets_ra[MOON]=tmpa;
    planets_dec[MOON]=tmpd;
    planets_sizes[MOON]=1800.0;
//    fprintf(ste,"n=MOON ra=%lf dec=%lf\n",12.0*tmpa/Pi,180.0*tmpd/Pi);    
}
    
void Draw_Ellipse(gdImagePtr im, double x, double y, double firad, double a, double b, int el_color)
{
    double k;
    double f;
    //  f=(double)fi*Pi/180.0;
    f=firad+Pi/2;
    a=a/2;
    b=b/2;
    for (k=0;k<6.29;k=k+0.01)
      {
        gdImageLine(im,
                    x+(b*sin(k-0.01)*sin(f)-a*cos(k-0.01)*cos(f)),
                    y+(b*sin(k-0.01)*cos(f)+a*cos(k-0.01)*sin(f)),
                    x+(b*sin(k)*sin(f)-a*cos(k)*cos(f)),
                    y+(b*sin(k)*cos(f)+a*cos(k)*sin(f)),
                    el_color);
      }
}

void Draw_Arc_pdf(CPDFdoc *pdf, double x, double y, double firad,
              double a, double b, int draw_direction, int moveto_arg)
{
    double k;
    double f;
    double x_c_arc[7];
    double y_c_arc[7];
    double x_c_arc_rot[7];
    double y_c_arc_rot[7];
    int i,j;
    
    a*=2;
    b*=2;
    x_c_arc[0] = 0;
    x_c_arc[1] = -(5./18.)*b;
    x_c_arc[2] = -b/2.;
    x_c_arc[3] = -b/2.;
    x_c_arc[4] = -b/2.;
    x_c_arc[5] = -(5./18.)*b;
    x_c_arc[6] = 0;

    y_c_arc[0] = -a/2.;
    y_c_arc[1] = -a/2.;
    y_c_arc[2] = -(5./18.)*a;
    y_c_arc[3] = 0;
    y_c_arc[4] = (5./18.)*a;
    y_c_arc[5] = a/2.;
    y_c_arc[6] = a/2.;
    for (i=0;i<7;i++)
    {
        x_c_arc_rot[i]=x + cos(firad)*x_c_arc[i]+sin(firad)*y_c_arc[i];
        y_c_arc_rot[i]=y - sin(firad)*x_c_arc[i]+cos(firad)*y_c_arc[i];
    }

    if (moveto_arg > 0)
        cpdf_moveto(pdf,0,0);
    if (draw_direction>=0)
    {
        if (moveto_arg > 0)
            cpdf_moveto(pdf, x_c_arc_rot[0], y_c_arc_rot[0]);
        cpdf_curveto(pdf,
                     x_c_arc_rot[1], y_c_arc_rot[1],
                     x_c_arc_rot[2], y_c_arc_rot[2],
                     x_c_arc_rot[3], y_c_arc_rot[3]);
        cpdf_curveto(pdf,
                     x_c_arc_rot[4], y_c_arc_rot[4],
                     x_c_arc_rot[5], y_c_arc_rot[5],
                     x_c_arc_rot[6], y_c_arc_rot[6]);
        //if (moveto_arg > 0)
        //    cpdf_moveto(pdf, x_c_arc_rot[0], y_c_arc_rot[0]);
    } else
    {
        if (moveto_arg > 0)
            cpdf_moveto(pdf, x_c_arc_rot[6], y_c_arc_rot[6]);
        cpdf_curveto(pdf,
                     x_c_arc_rot[5], y_c_arc_rot[5],
                     x_c_arc_rot[4], y_c_arc_rot[4],
                     x_c_arc_rot[3], y_c_arc_rot[3]);
        cpdf_curveto(pdf,
                     x_c_arc_rot[2], y_c_arc_rot[2],
                     x_c_arc_rot[1], y_c_arc_rot[1],
                     x_c_arc_rot[0], y_c_arc_rot[0]);
        //if (moveto_arg > 0)
        //    cpdf_moveto(pdf, x_c_arc_rot[6], y_c_arc_rot[6]);
    }
}

void Draw_Arc(gdImagePtr im, double x, double y, double firad,
              double a, double b, double beg_rad, double end_rad, int el_color)
{
    double k;
    double f;
    //  f=(double)fi*Pi/180.0;
    f=firad+Pi/2;
    a=a/2;
    b=b/2;
    for (k=beg_rad;k<end_rad;k=k+0.01)
      {
        gdImageLine(im,
                    x+(b*sin(k-0.01)*sin(f)-a*cos(k-0.01)*cos(f)),
                    y+(b*sin(k-0.01)*cos(f)+a*cos(k-0.01)*sin(f)),
                    x+(b*sin(k)*sin(f)-a*cos(k)*cos(f)),
                    y+(b*sin(k)*cos(f)+a*cos(k)*sin(f)),
                    el_color);
      }
}

void read_data()
{
    FILE *fcb;
    int d,x1,x2,y1,y2,tmp;
    long int i;
    int cbrd;
    unsigned char cbrec[4];
    
    //fprintf(stderr,"Begin of reading constellations\n");
//    if ((fcb=fopen("dat/conbnd.bin","rb"))==NULL) dummy_gif(1);
    if ((fcb=fopen("/astrodata/sky_chart/dat/conbnd.bin","rb"))==NULL) dummy_gif(1);
//    for(i=1;i<13046;i++)
    for(i=0;i<13046;i++)
    {
        if ((cbrd=fread(cbrec,sizeof(unsigned char),4,fcb))!=4) dummy_gif(0);
        conbnd[2*(i-1)+0]=24.000000*((double)cbrec[0]+256*(double)cbrec[1])/65536.00000;
        if ((cbrec[2]+256*cbrec[3]) > 32767)
          conbnd[2*(i-1)+1] = 90.0*((cbrec[2]/32400.0+256.0/32400.0*cbrec[3])-65536.0/32400.0);
        else conbnd[2*(i-1)+1] = 90.0*((cbrec[2]/32400.0+256.0/32400.0*cbrec[3]));
    }
    fclose(fcb);
    //fprintf(stderr,"Finish of reading constellations\n");
}



void read_bs(void)
{

  int d,x1,x2,y1,y2,tmp;
  long int i;
  int tcrd;
  unsigned char bsrec[6];
  NMAX=BS_COUNT;
  bscoord=(double *)malloc(2*sizeof(double)*NMAX);
  bsmag=(char *)malloc(sizeof(char)*NMAX);
  bsspec=(char *)malloc(sizeof(char)*NMAX);
//  fseek(ftyc,16*t_ind[j-1],0);

  //fprintf(stderr,"Begin of reading stars\n");
  
  for(i=1;i<=BS_COUNT;i++)
  
  {
      if ((tcrd=fread(bsrec,sizeof(unsigned char),6,fbs))!=6) dummy_gif(0);
      bscoord[2*(i-1)+0]=24.000000*((long int)bsrec[0]+256*(long int)bsrec[1])/65536.00000;

      if ((bsrec[2]+256*bsrec[3]) > 32768.0)
          bscoord[2*(i-1)+1] = (bsrec[2]+256*bsrec[3]-65536.00000)/360.00000;
      else    bscoord[2*(i-1)+1] = (bsrec[2]+256*bsrec[3])/360.00000;
      bsmag[i-1]=(char)(bsrec[4]);
      if (bsmag[i-1]>128) bsmag[i-1]=0;
      bsspec[i-1]=(char)(bsrec[5]);
//      fprintf(ste,"%lf %lf %lf %d\n",bscoord[2*(i-1)],bscoord[2*(i-1)+1],
//             bsmag[i-1]/10.0,bsspec[i-1]);
      //      if (bsspec[i-1]>=32.768) bsspec[i-1]=bsspec[i-1]-65.536;
  }
  //fprintf(stderr,"End of reading stars\n");
}

starxy ProjectPt(point3 pointc)
{
    starxy ssss1;
    ssss1.x = 30 + 640*imzoom*(1 + (-y00/(-y00+pointc.y))*pointc.x);
    ssss1.y = 30 + 640*imzoom*(1 - (-y00/(-y00+pointc.y))*pointc.z);
    if(fabs(pointc.x) < 1e-10) ssss1.x=30+640*imzoom;
    if(fabs(pointc.z) < 1e-10) ssss1.y=30+640*imzoom;
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

starxy CompPointCoord(double azimc,double heic,double t,double dec,int sm,
                      double stmg,double cut_val)
{
    starxy stareq;
    double x1, y1, z1;
    double x2, y2, z2;
    point3 p0001,p0002;

//    //formulae for y0=-inf;
//    //x1= sin(Pi*t/12)*cos(Pi*dec/180);
//    //y1= cos(Pi*t/12)*sin(Pi*lat/180)*cos(Pi*dec/180)-sin(Pi*dec/180)*cos(Pi*lat/180);
//    //z1= cos(Pi*t/12)*cos(Pi*lat/180)*cos(Pi*dec/180)+sin(Pi*dec/180)*sin(Pi*lat/180);

    x1= sin(Pi*t/12)*cos(Pi*dec/180);
    y1= cos(Pi*t/12)*sin(Pi*lati/180)*cos(Pi*dec/180)-sin(Pi*dec/180)*cos(Pi*lati/180);
    z1= cos(Pi*t/12)*cos(Pi*lati/180)*cos(Pi*dec/180)+sin(Pi*dec/180)*sin(Pi*lati/180);

    p0001.x  = x1;
    p0001.y  = y1;
    p0001.z  = z1;
    p0002 = RotAz(p0001);

    mtek = comp_radius(sm,stmg);
    y2=p0002.y;
    p0001=p0002;
    stareq = ProjectPt(p0001);
    stareq.sm = mtek;

    if ((y2<cut_val)||(z1<cut_val)) stareq.sm=-100;
    if(checkconst==1 && chconstnum>0) {
        int ll,res=1;
        for(ll=0; ll<chconstnum; ll++){
            res*=cns_check(Pi*(S_time - t)/12., Pi*dec/180.,
                           mjd2000, 0.2*Pi/180., chconstarr[ll]);
        }
        if(res !=0 ) stareq.sm=-100;
        /*        if(cns_pick(Pi*(S_time - t)/12., Pi*dec/180.,
         2000.0)!=Vir) stareq.sm=-100;*/
    }
    return stareq;
}

void CompCoord_pdf(CPDFdoc *pdf,float zoomk,double alpc, double delc)
{
    int j;
    long int i;
    long int g;
    double Astek,Bstek;
    double fitek;
    double racs[89];
    double deccs[89];
    double ra[50];
    double dec[50];
    double ssm[89];
    int dcodes[50];
    int k,k01;
    int nn1;
    starxy startek,startek1;
    if (((const_lines==1)||(dcnam==1))&&(const_bnd!=1))
    {
        cpdf_closepath(pdf);
        cpdf_stroke(pdf);
        for(i=0;i<=88;i++)
        {
            k=cns_figure(i,ra,dec,dcodes);
            k01=0;
            racs[i]=0;
            deccs[i]=0;
            ssm[i]=0;
            if (k!=-1)
            {
                cpdf_setrgbcolor(pdf,1.0,0.0,0.0);
                //cpdf_setlinewidth(pdf,0.5);
                cpdf_moveto(pdf,0.,0.);
                for(g=0;g<(k-1);g++)
                {
                    startek =CompPointCoord(azimc, heightc, S_time-ra[g], dec[g], 0, stmag, 0.00);
                    startek1=CompPointCoord(azimc, heightc, S_time-ra[g+1], dec[g+1], 0, stmag, 0.00);
                    if ((startek.sm>0)&&(startek1.sm>0))
                    {
                        ssm[i]+=startek.sm;
                        racs[i]+=startek.x;
                        deccs[i]+=startek.y;
                        k01++;
                        if ((dcodes[g+1]==1)&&(const_lines==1))
                        {
                            cpdf_moveto(pdf,startek.x,startek.y);
                            cpdf_lineto(pdf,startek1.x,startek1.y);
                        }
                        if ((dcodes[g+1]==2)&&(const_lines==1))
                        {
                            //cpdf_closepath(pdf);
                            //cpdf_stroke(pdf);
                            //cpdf_setrgbcolor(pdf,0.0,0.5,0.0);
                            //cpdf_moveto(pdf,0.,0.);
                            
                            //cpdf_setdash(pdf,"[4 4] 0");
                            cpdf_moveto(pdf,startek.x,startek.y);
                            cpdf_lineto(pdf,startek1.x,startek1.y);
                            //cpdf_nodash(pdf);

                            //cpdf_moveto(pdf,0.,0.);
                            //cpdf_closepath(pdf);
                            //cpdf_stroke(pdf);
                        }
                    }
                }
                cpdf_moveto(pdf,0.,0.);
                cpdf_closepath(pdf);
                cpdf_stroke(pdf);
            }
            racs[i]/=k01;
            deccs[i]/=k01;
            if ((i==Eri)&&(k01>24))
                deccs[i]*=0.9;
            if (dcnam==1)
            {
                if (ssm[i]>0)
                {
                    nn1=(int)strlen(cns_name(i)+5);
                    cpdf_setrgbcolor(pdf,0.0,0.0,1.0);
                    cpdf_beginText(pdf,0);
                    cpdf_setFont(pdf,"Arial-Bold","MacRomanEncoding",10.0);
                    cpdf_text(pdf,(racs[i]-2.5*nn1)-15.,deccs[i]+20.,0.0,cns_name(i)+5);
                    cpdf_endText(pdf);
                }
            }
        }
    } else if (const_bnd==1)
    {
        for(i=1;i<=CBND;i++)
        {
            startek=CompPointCoord(azimc, heightc, S_time-conbnd[(i-1)*2], conbnd[(i-1)*2+1], 0, stmag, 0.00);
            xconst[i-1]=startek.x;
            yconst[i-1]=startek.y;
            if (startek.sm<0)
            {
                xconst[i-1]=-10000;
                yconst[i-1]=-10000;
            }
        }
    }
    else
    {
        if(colstars==1) {
            cpdf_setgrayFill(pdf,1.0);
            cpdf_setgrayStroke(pdf,0.0);
        } else {
            cpdf_setgrayFill(pdf,0.0);
            cpdf_setgrayStroke(pdf,1.0);
        }
        for(i=1;i<=NMAX;i++)
        {
            startek=CompPointCoord(azimc, heightc, S_time-bscoord[(i-1)*2], bscoord[(i-1)*2+1], bsmag[i-1], stmag, -0.01);
            //            sprintf(stmaglab,"%d",bsmag[i-1]);
            //            fprintf(ste,"%lf %lf %lf %d  %d %d %d\n",bscoord[2*(i-1)],bscoord[2*(i-1)+1],bsmag[i-1]/10.0,startek.x,startek.y,startek.sm);
            //            if (bsmag[i-1]<stmag)
            if (startek.sm>=0)
            {
                if(colstars==1) {
                    double rs = (double)( spectr[bsspec[i-1]/10] >> 16)/255.0;
                    double gs = (double)((spectr[bsspec[i-1]/10] >> 8 ) % 256)/255.0;
                    double bs = (double)( spectr[bsspec[i-1]/10] % 256)/255.0;
                    cpdf_setrgbcolor(pdf,rs,gs,bs);
                    cpdf_setrgbcolorFill(pdf,rs,gs,bs);
                    cpdf_setrgbcolorStroke(pdf,0.,0.,0.);
                }
                print_circle_pdf(pdf,(float)startek.x,(float)startek.y,
                                 (float)startek.sm*1280.0/SIZEX);
            }
        }
        cpdf_setrgbcolor(pdf,0.,0.,0.);
    }
}

void CompPlotMW_pdf(CPDFdoc *pdf,float zoomk,double alpc, double delc)
{
    int i;
    starxy startek,startek1;
    int col1,col2;

    col1=0;
    for(i=1;i<=NMWAY;i++)
    {
        startek=CompPointCoord(azimc, heightc, S_time-mw_data[i-1].ra,
                               mw_data[i-1].dec, 0.50, stmag, 0.00);
        if(colstars==1) {
            col2=(int)(sqrt(mw_data[i-1].intens)*16.0-2.0);
            //col2=mw_data[i-1].intens;
            if(col2 > 240) col2=240;
            if(col2 < 0) col2=0;
            col2*=(255.0/240.0);
        }
        else {
            col2=mw_data[i-1].intens;
        }
        if ((startek.sm>=0)&&(col2>1))
        {
            if(col2!=col1){
                if(colstars==1) {
                    cpdf_setrgbcolorFill(pdf,col2/512.0,col2/395.0,col2/318.0);
                    cpdf_setrgbcolorStroke(pdf,col2/512.0,col2/395.0,col2/318.0);
//                    fprintf(stderr,"%5.2f %5.2f %5.2f\n",col2/512.0,col2/395.0,col2/318.0);
                } else {
                    cpdf_setgrayFill(pdf,1.0-col2/384.0);
                    cpdf_setgrayStroke(pdf,1.0-col2/384.0);
                }
                col1=col2;
            }
//            print_circle_pdf(pdf,(float)startek.x,(float)startek.y,3.0*mwpt_rad);
            print_circle_pdf(pdf,(float)startek.x,(float)startek.y,2.0);
        }
    }
    cpdf_setrgbcolor(pdf,0.,0.,0.);
}

void CompPlotMW(gdImagePtr im, float zoomk, double alpc, double delc)
{
    int i;
    starxy startek,startek1,*stararr;
    int col;
    int cim;
    int *carr;

    stararr=(starxy *)malloc(sizeof(starxy)*NMWAY);
    carr=(int *)malloc(sizeof(int)*NMWAY);
    for(i=1;i<=NMWAY;i++)
    {
        stararr[i-1]=CompPointCoord(azimc, heightc, S_time-mw_data[i-1].ra,
                               mw_data[i-1].dec, 0.50, stmag, 0.00);
        startek=stararr[i-1];
        if (startek.sm>=0)
        {
            carr[i-1]=gdImageGetPixel(im, startek.x,startek.y);
        }
    }

    for(i=1;i<=NMWAY;i++)
    {
        startek=stararr[i-1];
        if (startek.sm>=0)
        {
            int rmw,gmw,bmw;
            cim=carr[i-1];
            col=(int)(sqrt(mw_data[i-1].intens)*4.0-2);
            //col=(int)((mw_data[i-1].intens+4.0)/4.25);
            //col+=cim;
            if(col>127) col=127;
            if(col<0) col=0;
            rmw=col;
            gmw=col;
            bmw=2*col;
            bmw+=gdTrueColorGetBlue(cim);
            //if(im->blue[col] > im->blue[cim])
            print_circle(im,startek.x,startek.y,imzoom*5+1,
                         gdTrueColor(rmw,gmw,bmw),SIZEX+60);
        }
    }
}

void CompCoord(gdImagePtr im,float zoomk,double alpc, double delc)
{
    int j;
    long int i;
    long int g;
    double Astek,Bstek;
    double fitek;
    double racs[89];
    double deccs[89];
    double ra[50];
    double dec[50];
    double ssm[89];
    int dcodes[50];
    int k,k01;
    int nn1;
    starxy startek,startek1;
    double cut_val;
    if (((const_lines==1)||(dcnam==1))&&(const_bnd!=1))
    {
        for(i=0;i<=88;i++)
        {
            k=cns_figure(i,ra,dec,dcodes);

            //for constellation painting
            k01=0;
            racs[i]=0;
            deccs[i]=0;
            ssm[i]=0;

            if (k!=-1)
            {
                for(g=0;g<(k-1);g++)
                {
                    startek =CompPointCoord(azimc, heightc, S_time-ra[g], dec[g], 0, stmag, 0.00);
                    startek1=CompPointCoord(azimc, heightc, S_time-ra[g+1], dec[g+1], 0, stmag, 0.00);
                    if ((startek.sm>0)&&(startek1.sm>0))
                    {
                        ssm[i]+=startek.sm;
                        racs[i]+=startek.x;
                        deccs[i]+=startek.y;
                        k01++;
                    }
                }
            }
            racs[i]/=k01;
            deccs[i]/=k01;
            if ((i==Eri)&&(k01>24))
                deccs[i]*=0.9;
            if ((i==chosen_const)||((chosen_const==Se1)&&(i==Se2)))
            {
                gdImageFill(im, racs[i], deccs[i],blue_gr[20]);
                draw_horizont(im);
                gdImageFillToBorder(im, 1, 1, white, black);
            }
            //end of new block
            
            k01=0;
            racs[i]=0;
            deccs[i]=0;
            ssm[i]=0;

            if (k!=-1)
            {
                for(g=0;g<(k-1);g++)
                {
                    startek =CompPointCoord(azimc, heightc, S_time-ra[g], dec[g], 0, stmag, 0.00);
                    startek1=CompPointCoord(azimc, heightc, S_time-ra[g+1], dec[g+1], 0, stmag, 0.00);
                    if ((startek.sm>0)&&(startek1.sm>0))
                    {
                        ssm[i]+=startek.sm;
                        racs[i]+=startek.x;
                        deccs[i]+=startek.y;
                        k01++;
                        if ((dcodes[g+1]==1)&&(const_lines==1))
                        {
                            gdImageLine(im,startek.x,startek.y,startek1.x,startek1.y,darkgreen);
                        }
                        if ((dcodes[g+1]==2)&&(const_lines==1))
                        {
                            gdImageLine(im,startek.x,startek.y,startek1.x,startek1.y,gdStyled);
                        }
                    }
                }
            }
            racs[i]/=k01;
            deccs[i]/=k01;
            if ((i==Eri)&&(k01>24))
                deccs[i]*=0.9;
            if (dcnam==1)
            {
                if (ssm[i]>0)
                {
                    if(lang==1) {
                        nn1=(int)strlen(cns_name_ru(i)+5);
                        gdImageString(im,fontmb,racs[i]-2.5*nn1,deccs[i],cns_name_ru(i)+5,yellow);
                    } else if(lang==0) {
                        nn1=(int)strlen(cns_name(i)+5);
                        gdImageString(im,fontmb,racs[i]-2.5*nn1,deccs[i],cns_name(i)+5,yellow);
                    }
                }
            }

        }
    } else
    if (const_bnd==1)
    {
        //cut_val = (chosen_const>=0) ? -0.1 : 0;
        if (chosen_const >=0) cut_val=-0.04; else cut_val=0;
        for(i=1;i<=CBND;i++)
        {
            startek=CompPointCoord(azimc, heightc, S_time-conbnd[(i-1)*2], conbnd[(i-1)*2+1], 0, stmag, cut_val);
            xconst[i-1]=startek.x;
            yconst[i-1]=startek.y;
            if (startek.sm<0)
            {
                xconst[i-1]=-10000;
                yconst[i-1]=-10000;
            }
        }
    }
    else
    {
        for(i=1;i<=NMAX;i++)
        {
            startek=CompPointCoord(azimc, heightc, S_time-bscoord[(i-1)*2], bscoord[(i-1)*2+1], bsmag[i-1], stmag, 0.00);
//            sprintf(stmaglab,"%d",bsmag[i-1]);
//            fprintf(ste,"%lf %lf %lf %d  %d %d %d\n",bscoord[2*(i-1)],bscoord[2*(i-1)+1],bsmag[i-1]/10.0,startek.x,startek.y,startek.sm);
//            if (bsmag[i-1]<stmag)
            if (startek.sm>=0)
            {
                if (colstars==1)
                {
                    print_circle(im,startek.x,startek.y,startek.sm,spectr[bsspec[i-1]/10],SIZEX+60);
//                    print_circle(im,startek.x,startek.y,0,spectr[bsspec[i-1]/10],SIZEX+60);
                } else
                {
                    print_circle(im,startek.x,startek.y,startek.sm,white,SIZEX+60);
                }
//                    if ((printsl==1)&&((slbr-0.05)<=(double)(bsmag[i-1]/10.0))&&((slf+0.05)>=(bsmag[i-1]/10)))
//                        gdImageString(im,font,startek.x+startek.sm,startek.y-5,stmaglab,yellow);
            }
        }
    }
}

void draw_moon_pdf(CPDFdoc *pdf, double x_moon, double y_moon, double ph_a, double pos_a)
{
    int i;
    if (ph_a>=0) pos_a+=Pi;
    cpdf_moveto(pdf,0,0);
    cpdf_closepath(pdf);
    cpdf_stroke(pdf);
    
    if(colstars==1) {
        cpdf_setgrayFill(pdf,1.0);
        cpdf_setgrayStroke(pdf,0.0);
    } else {
        cpdf_setgrayFill(pdf,0.0);
        cpdf_setgrayStroke(pdf,0.0);
    }

    Draw_Arc_pdf(pdf,x_moon,y_moon,pos_a,23.,23.,1,1);
    Draw_Arc_pdf(pdf,x_moon,y_moon,pos_a,23.,23.*cos(ph_a),-1,0);

    cpdf_moveto(pdf,0.,0.);
    cpdf_closepath(pdf);
    cpdf_fill(pdf);
}

void draw_moon(gdImagePtr im, int x_moon, int y_moon, double ph_a, double pos_a)
{
    int i;
    print_circle(im,x_moon,y_moon,24,black,SIZEX+60);
    if (ph_a>0) pos_a+=Pi;
    //pos_a=-pos_a;
    //gdImageArc(im,x_moon,y_moon,24,24,90-180.0*pos_a/Pi,270-180.0*pos_a/Pi,white);
    Draw_Arc(im,x_moon,y_moon,pos_a,24,24,Pi,2*Pi,white);
    if (abs(ph_a)<=Pi/2.0)
    {
        for (i=1;i<=24;i++)
        {
            Draw_Arc(im,x_moon,y_moon,pos_a,i,i,Pi,2*Pi,white);
            Draw_Arc(im,x_moon,y_moon,pos_a,i-1,i*cos(ph_a),Pi,2*Pi,black);
        }
    } else
    {
        for (i=1;i<=24;i++)
        {
            Draw_Arc(im,x_moon,y_moon,pos_a,i,i,Pi,2*Pi,white);
            Draw_Arc(im,x_moon,y_moon,pos_a,i,i*cos(ph_a),Pi,2*Pi,white);
        }
    }
}

void draw_planets_pdf(CPDFdoc *pdf)
{
    int i;
    double phase_angle, pos_angle, pos_angle0;
    double delta_lam, delta_bet;
    double ra_1, ra_2, dec_1, dec_2;
    starxy startek, startek1, startek_1, startek_2;

    if(colstars==1) {
        cpdf_setgrayFill(pdf,1.0);
        cpdf_setgrayStroke(pdf,0.0);
    } else {
        cpdf_setgrayFill(pdf,0.0);
        cpdf_setgrayStroke(pdf,1.0);
    }

    startek=CompPointCoord(azimc, heightc, S_time-(planets_ra[URANUS]*12.0/Pi), planets_dec[URANUS]*180.0/Pi, 0.0, stmag, 0.00);
    if (startek.sm>=0)
    {
        print_circle_pdf(pdf,startek.x,startek.y,6);
    }

    startek=CompPointCoord(azimc, heightc, S_time-(planets_ra[NEPTUNE]*12.0/Pi), planets_dec[NEPTUNE]*180.0/Pi, 0.0, stmag, 0.00);
    if (startek.sm>=0)
    {
        print_circle_pdf(pdf,startek.x,startek.y,6);
    }

    startek=CompPointCoord(azimc, heightc, S_time-(planets_ra[PLUTO]*12.0/Pi), planets_dec[PLUTO]*180.0/Pi, 0.0, stmag, 0.00);
    if (startek.sm>=0)
    {
        print_circle_pdf(pdf,startek.x,startek.y,5);
    }

    startek=CompPointCoord(azimc, heightc, S_time-(planets_ra[MERCURY]*12.0/Pi), planets_dec[MERCURY]*180.0/Pi, -1.0, stmag, 0.00);
    if (startek.sm>=0)
    {
        print_circle_pdf(pdf,startek.x,startek.y,startek.sm);
    }

    startek=CompPointCoord(azimc, heightc, S_time-(planets_ra[VENUS]*12.0/Pi), planets_dec[VENUS]*180.0/Pi, -4.0, stmag, 0.00);
    if (startek.sm>=0)
    {
        print_circle_pdf(pdf,startek.x,startek.y,startek.sm);
    }

    startek=CompPointCoord(azimc, heightc, S_time-(planets_ra[MARS]*12.0/Pi), planets_dec[MARS]*180.0/Pi, -1.0, stmag, 0.00);
    if (startek.sm>=0)
    {
        print_circle_pdf(pdf,startek.x,startek.y,startek.sm);
    }

    startek=CompPointCoord(azimc, heightc, S_time-(planets_ra[JUPITER]*12.0/Pi), planets_dec[JUPITER]*180.0/Pi, -2.5, stmag, 0.00);
    if (startek.sm>=0)
    {
        print_circle_pdf(pdf,startek.x,startek.y,startek.sm);
    }

    startek=CompPointCoord(azimc, heightc, S_time-(planets_ra[SATURN]*12.0/Pi), planets_dec[SATURN]*180.0/Pi, 1.0, stmag, 0.00);
    if (startek.sm>=0)
    {
        print_circle_pdf(pdf,startek.x,startek.y,startek.sm);
    }

    startek=CompPointCoord(azimc, heightc, S_time-(planets_ra[SUN]*12.0/Pi), planets_dec[SUN]*180.0/Pi, 0.0, stmag, 0.00);
    startek1=startek;
    if (startek.sm>=0)
    {
        print_circle_pdf(pdf,startek.x,startek.y,24);
        cpdf_beginText(pdf,0);
        cpdf_setFont(pdf,"Times-Italic","MacRomanEncoding",10.0);
        cpdf_text(pdf,startek.x+16,startek.y-5,0.0,pl_names[SUN]);
        cpdf_endText(pdf);
    }

    for (i=0;i<=7;i++)
    {
        startek=CompPointCoord(azimc, heightc, S_time-(planets_ra[i]*12.0/Pi), planets_dec[i]*180.0/Pi, 0.0, stmag, 0.00);
            if (startek.sm>=0)
            {
                cpdf_beginText(pdf,0);
                cpdf_setFont(pdf,"Times-Italic","MacRomanEncoding",10.0);
                cpdf_text(pdf,startek.x+10,startek.y-5,0.0,pl_names[i]);
                cpdf_endText(pdf);
            }
    }
    /*MOON*/

    startek=CompPointCoord(azimc, heightc, S_time-(planets_ra[MOON]*12.0/Pi), planets_dec[MOON]*180.0/Pi, 0.0, stmag, 0.00);
    if (startek.sm>=0)
    {
        delta_lam=(sun_ecl_coords[0]-moon_ecl_coords[0]);
        delta_bet=(sun_ecl_coords[1]-moon_ecl_coords[1]);
        if (delta_lam>Pi) delta_lam-=2*Pi;
        if (delta_lam<-Pi) delta_lam+=2*Pi;
        phase_angle=delta_lam;
        if (sqrt(pow(delta_lam,2)+pow(delta_bet,2))<0.3)
        {
            if (startek.y==startek1.y) pos_angle=Pi/2.0; else
                pos_angle=Pi-atan((startek.x-startek1.x)/(startek.y-startek1.y));
        }
        else
        {
            ecl_eq_my(moon_ecl_coords[0], moon_ecl_coords[1]+0.15, &ra_1, &dec_1);
            ecl_eq_my(moon_ecl_coords[0], moon_ecl_coords[1]-0.15, &ra_2, &dec_2);
            startek_1=CompPointCoord(azimc, heightc, S_time-(ra_1*12.0/Pi), dec_1*180.0/Pi, 0.0, stmag, 0.00);
            startek_2=CompPointCoord(azimc, heightc, S_time-(ra_2*12.0/Pi), dec_2*180.0/Pi, 0.0, stmag, 0.00);
            if (startek_1.y==startek_2.y) pos_angle=Pi/2.0; else
                pos_angle=atan2((startek_1.x-startek_2.x),(startek_1.y-startek_2.y));
        }
        draw_moon_pdf(pdf,startek.x,startek.y,phase_angle,pos_angle);
        cpdf_beginText(pdf,0);
        cpdf_setFont(pdf,"Times-Italic","MacRomanEncoding",10.0);
        cpdf_text(pdf,startek.x+16,startek.y-5,0.0,pl_names[MOON]);
        cpdf_endText(pdf);
    }
    
}

void draw_planets(gdImagePtr im)
{
    int i;
    double phase_angle, pos_angle, pos_angle0;
    double delta_lam, delta_bet;
    double ra_1, ra_2, dec_1, dec_2;
    starxy startek, startek1, startek_1, startek_2;

    startek=CompPointCoord(azimc, heightc, S_time-(planets_ra[URANUS]*12.0/Pi), planets_dec[URANUS]*180.0/Pi, 0.0, stmag, 0.00);
    if (startek.sm>=0)
    {
        print_circle(im,startek.x,startek.y,6,spectr[0],SIZEX+60);
    }

    startek=CompPointCoord(azimc, heightc, S_time-(planets_ra[NEPTUNE]*12.0/Pi), planets_dec[NEPTUNE]*180.0/Pi, 0.0, stmag, 0.00);
    if (startek.sm>=0)
    {
        print_circle(im,startek.x,startek.y,6,blue,SIZEX+60);
    }

    startek=CompPointCoord(azimc, heightc, S_time-(planets_ra[PLUTO]*12.0/Pi), planets_dec[PLUTO]*180.0/Pi, 0.0, stmag, 0.00);
    if (startek.sm>=0)
    {
        print_circle(im,startek.x,startek.y,5,darkgreen,SIZEX+60);
    }

    startek=CompPointCoord(azimc, heightc, S_time-(planets_ra[MERCURY]*12.0/Pi), planets_dec[MERCURY]*180.0/Pi, -1.0, stmag, 0.00);
    if (startek.sm>=0)
    {
        print_circle(im,startek.x,startek.y,startek.sm,spectr[2],SIZEX+60);
    }

    startek=CompPointCoord(azimc, heightc, S_time-(planets_ra[VENUS]*12.0/Pi), planets_dec[VENUS]*180.0/Pi, -4.0, stmag, 0.00);
    if (startek.sm>=0)
    {
        print_circle(im,startek.x,startek.y,startek.sm,spectr[3],SIZEX+60);
    }

    startek=CompPointCoord(azimc, heightc, S_time-(planets_ra[MARS]*12.0/Pi), planets_dec[MARS]*180.0/Pi, -1.0, stmag, 0.00);
    if (startek.sm>=0)
    {
        print_circle(im,startek.x,startek.y,startek.sm,spectr[6],SIZEX+60);
    }

    startek=CompPointCoord(azimc, heightc, S_time-(planets_ra[JUPITER]*12.0/Pi), planets_dec[JUPITER]*180.0/Pi, -2.5, stmag, 0.00);
    if (startek.sm>=0)
    {
        print_circle(im,startek.x,startek.y,startek.sm,spectr[4],SIZEX+60);
    }

    startek=CompPointCoord(azimc, heightc, S_time-(planets_ra[SATURN]*12.0/Pi), planets_dec[SATURN]*180.0/Pi, 1.0, stmag, 0.00);
    if (startek.sm>=0)
    {
        print_circle(im,startek.x,startek.y,startek.sm,spectr[3],SIZEX+60);
    }

    startek=CompPointCoord(azimc, heightc, S_time-(planets_ra[SUN]*12.0/Pi), planets_dec[SUN]*180.0/Pi, 0.0, stmag, 0.00);
    startek1=startek;
    if (startek.sm>=0)
    {
        print_circle(im,startek.x,startek.y,24,spectr[4],SIZEX+60);
        if(lang==1)
            gdImageString(im,fontmb,startek.x+16,startek.y-5,pl_names_ru[SUN],white);
        else if(lang==0)
            gdImageString(im,fontmb,startek.x+16,startek.y-5,pl_names[SUN],white);
    }

    for (i=0;i<=7;i++)
    {
            startek=CompPointCoord(azimc, heightc, S_time-(planets_ra[i]*12.0/Pi), planets_dec[i]*180.0/Pi, 0.0, stmag, 0.00);
            if (startek.sm>=0)
            {
                if(lang==1)
                    gdImageString(im,fontmb,startek.x+10,startek.y-5,pl_names_ru[i],green);
                else if(lang==0)
                    gdImageString(im,fontmb,startek.x+10,startek.y-5,pl_names[i],green);
            }

    }
    /*MOON*/
    startek=CompPointCoord(azimc, heightc, S_time-(planets_ra[MOON]*12.0/Pi), planets_dec[MOON]*180.0/Pi, 0.0, stmag, 0.00);
    if (startek.sm>=0)
    {
        delta_lam=(sun_ecl_coords[0]-moon_ecl_coords[0]);
        delta_bet=(sun_ecl_coords[1]-moon_ecl_coords[1]);
        if (delta_lam>Pi) delta_lam-=2*Pi;
        if (delta_lam<-Pi) delta_lam+=2*Pi;
//        phase_angle=Pi-sqrt(pow(delta_lam,2)+pow(delta_bet,2));
        phase_angle=delta_lam;
        if (sqrt(pow(delta_lam,2)+pow(delta_bet,2))<0.3)
        {
            if (startek.y==startek1.y) pos_angle=Pi/2.0; else
                pos_angle=Pi-atan((startek.x-startek1.x)/(startek.y-startek1.y));
        }
//            pos_angle=Pi+atan((startek.y-startek1.y)/(startek.x-startek1.x));
        else
        {
//            pos_angle0=0.41015*cos(moon_ecl_coords[0])+Pi-atan(delta_bet/delta_lam);
//            ecl_eq_my(moon_ecl_coords[0], moon_ecl_coords[1]+0.06, &ra_1, &dec_1);
//            ecl_eq_my(moon_ecl_coords[0], moon_ecl_coords[1]-0.06, &ra_2, &dec_2);
            ecl_eq_my(moon_ecl_coords[0], moon_ecl_coords[1]+0.15, &ra_1, &dec_1);
            ecl_eq_my(moon_ecl_coords[0], moon_ecl_coords[1]-0.15, &ra_2, &dec_2);
            startek_1=CompPointCoord(azimc, heightc, S_time-(ra_1*12.0/Pi), dec_1*180.0/Pi, 0.0, stmag, 0.00);
            startek_2=CompPointCoord(azimc, heightc, S_time-(ra_2*12.0/Pi), dec_2*180.0/Pi, 0.0, stmag, 0.00);
            if (startek_1.y==startek_2.y) pos_angle=Pi/2.0; else
                pos_angle=atan2((startek_1.x-startek_2.x),(startek_1.y-startek_2.y));
        }
//        if (phase_angle<-Pi) phase_angle+=2.0*Pi;
//        if (phase_angle>Pi) phase_angle-=2.0*Pi;
        draw_moon(im,startek.x,startek.y,phase_angle,pos_angle);
        if(lang==1)
            gdImageString(im,fontmb,startek.x+16,startek.y-5,pl_names_ru[MOON],white);
        else if(lang==0)
            gdImageString(im,fontmb,startek.x+16,startek.y-5,pl_names[MOON],white);
//        print_circle(im,startek.x,startek.y,24,spectr[4],SIZEX+60);
    }
    
}

void canvas_init_pdf(CPDFdoc *pdf,double alpc,double delc,double zoomk,int color)
{
    double i,j;
    long int yc, rmain;
    starxy s1,s2,s3;

    if (dgrids==1)
    {
        cpdf_setrgbcolor(pdf, 0.0, 0.5, 1.0);
        for (i=0;i<24;i++)
        {
            s2=CompPointCoord(azimc,heightc,S_time-i,-80,0, stmag, -0.01);
            s3=s2;
            for (j=-80*imzoom;j<=80*imzoom;j++)
            {
                s1=CompPointCoord(azimc,heightc,S_time-i,j/(imzoom),0, stmag, -0.01);
                if ((s1.sm>0)&&(s2.sm>0))
                {
                    cpdf_moveto(pdf,s2.x,s2.y);
                    cpdf_lineto(pdf,s1.x,s1.y);
                }
                s2=s1;
            }
            if(s3.sm>0)
                cpdf_moveto(pdf,s3.x,s3.y);
            cpdf_closepath(pdf);
            cpdf_stroke(pdf);
        }
        for (j=-8;j<=8;j++)
        {
            s2=CompPointCoord(azimc,heightc,S_time,j*10,0,stmag, -0.01);
            s3=s2;
            for (i=0;i<=24.0*10.0*imzoom*cos(Pi*j/18.0)+1;i++)
            {
                s1=CompPointCoord(azimc,heightc,S_time-i/(10.0*imzoom*cos(Pi*j/18.0)),j*10,0,stmag, -0.01);
                if ((s1.sm>0)&&(s2.sm>0))
                {
                    cpdf_moveto(pdf,s2.x,s2.y);
                    cpdf_lineto(pdf,s1.x,s1.y);
                }
                s2=s1;
                /*cpdf_circle(pdf,s1.x/100.,s1.y/100.,0.002);*/
            }
            if(s3.sm>0)
                cpdf_moveto(pdf,s3.x,s3.y);
            cpdf_closepath(pdf);
            cpdf_stroke(pdf);
        }
    }
}

void canvas_init(gdImagePtr im,double alpc,double delc,double zoomk,int color)
{
     double i,j;
     long int yc, rmain;
     starxy s1,s2;
     char *string1;

     string1=(char*)malloc(5*sizeof(char));
     //str1=(char *)malloc(10);     
     if (dgrids==1)
     {
         for (i=0;i<24;i++)
         {
	     sprintf(string1,"%02dh",(int)i);
	     s1=CompPointCoord(azimc,heightc,S_time-i+0.5/(imzoom*8.),-0.4/imzoom,0, stmag, 0.00);
             if (s1.sm > 0)
                 gdImageString(im,fontmb,s1.x,s1.y,string1,white);

	     s1=CompPointCoord(azimc,heightc,S_time-i+0.5/(imzoom*8.),60.-0.4/imzoom,0, stmag, 0.00);
	     if (s1.sm > 0)
	     	gdImageString(im,fontmb,s1.x,s1.y,string1,white);

	     s1=CompPointCoord(azimc,heightc,S_time-i+0.5/(imzoom*8.),-60.-0.4/imzoom,0, stmag, 0.00);
	     if (s1.sm > 0)
	     	gdImageString(im,fontmb,s1.x,s1.y,string1,white);

             for (j=-80*2.5*imzoom;j<=80*2.5*imzoom;j++)
             {
                 s1=CompPointCoord(azimc,heightc,S_time-i,j/(2.5*imzoom),0, stmag, 0.00);
                 //fprintf(ste,"s1.x=%lf s1.y=%lf\n",s1.x/1.,s1.y/1.);
                 if (s1.sm>0)
                     gdImageSetPixel(im, s1.x, s1.y, green);
             }
         }
         for (j=-8;j<=8;j++)
         {
	     sprintf(string1,"%+02d",(int)j*10);
	     s1=CompPointCoord(azimc,heightc,0,j*10,0,stmag, 0.00);
	     if (s1.sm > 0)
	     	gdImageString(im,fontmb,s1.x,s1.y,string1,white);
             for (i=0;i<24.0*30.0*imzoom*cos(Pi*j/18.0);i++)
             {
                 s1=CompPointCoord(azimc,heightc,S_time-i/(30.0*imzoom*cos(Pi*j/18.0)),j*10,0,stmag, 0.00);
                 //fprintf(ste,"s1.x=%lf s1.y=%lf\n",s1.x/1.,s1.y/1.);
                 if (s1.sm>0)
                     gdImageSetPixel(im, s1.x, s1.y, green);
             }
         }
     }
 }

double comp_s_time()
{
    double s_ut0, s_t, s_day;
    long int nday;
    double mjd0,mjday;
    s_day=(year0-1)/year0;
    cal_mjd(1, 1.0, 2000, &mjd2000);
    //osennee ravnodenstvie 23.09.99 v 11:31 UT  ///15.5 UT///
    cal_mjd(9, 23, 1999, &mjd0);
    cal_mjd(month, day, year+1900, &mjday);
    nday=(long int)(mjday-mjd0);
    /*
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
    } */
//    s_ut0=(double)(nday-0.4798611111)*(s_day/15.0);//-0.4798611111;
    s_ut0=0.000+((double)(nday))*(1.0-s_day);//-0.4798611111;
    s_ut0=24.0*(s_ut0-(int)(s_ut0));
    s_ut0=s_ut0-(double)24*(int)(s_ut0/24.0)+0.000285;//+0.000284666666667
    if (s_ut0<0) s_ut0+=24.0;
    s_t=s_ut0-longi/15.0+1.00274543*ut;
    s_t=s_t-(double)24*(int)(s_t/24.0);
    if (s_t<0) s_t+=24.0;
//    fprintf(ste,"ut=%lf date=%d/%d/%d\n",ut,day,month,year);
//    fprintf(ste,"longi=%lf nday=%d s_ut0=%lf s_time=%lf\n",longi,nday,s_ut0,s_t);
    return s_t;
}

void draw_horizont_pdf(CPDFdoc *pdf)
{
    starxy s1,s2,s11,s12;
    int i,I;
    point3 pnt0,pnt1;
    point3 pnt01,pnt11;
    point3 pnt02,pnt12;
    char *sss1;

    sss1=(char *)malloc(sizeof(char)*5);
    I=3600*imzoom;

    pnt0.x = -1.;
    pnt0.z = 0.;
    pnt0.y = 0.;
    pnt1=RotAz(pnt0);
    s1=ProjectPt(pnt1);
    if(colstars==1){
        cpdf_setgrayStroke(pdf,1.);
        cpdf_setrgbcolor(pdf,1.,1.,1.);
    } else {
        cpdf_setgrayStroke(pdf,0.);
    }
    //cpdf_arc(pdf,670,670,640,0.,PI,1);
    cpdf_moveto(pdf,s1.x,s1.y);
    s2=s1;
    for(i=-I/1.;i<=I/1.;i++)
    {
        pnt0.x= sin(Pi*(i/(double)I));
        pnt0.z= 0;
        pnt0.y= cos(Pi*(i/(double)I));
        pnt1=RotAz(pnt0);
        s1=ProjectPt(pnt1);
        if ((i % 500)==0)
        {
            cpdf_moveto(pdf,s2.x,s2.y);
            cpdf_closepath(pdf);
            cpdf_stroke(pdf);
            cpdf_moveto(pdf,s1.x,s1.y);
            s2=s1;
        }
        if (pnt1.y>0) {
            cpdf_lineto(pdf,s1.x,s1.y);
            /*fprintf(stderr,"i,s1.xy=%d %lf %lf\n",i,s1.x,s1.y);*/
        }
    }
    for(i=0;i<=I;i++)
    {
        pnt0.x= cos(Pi*(i/(double)I));
        pnt0.y= 0;
        pnt0.z= sin(Pi*(i/(double)I));
        pnt1=pnt0;
        s1=ProjectPt(pnt1);
        if ((i % 500)==0)
        {
            cpdf_moveto(pdf,s2.x,s2.y);
            cpdf_closepath(pdf);
            cpdf_stroke(pdf);
            cpdf_moveto(pdf,s1.x,s1.y);
            s2=s1;
        }
        cpdf_lineto(pdf,s1.x,s1.y);
        /*fprintf(stderr,"2i,s1.xy=%d %lf %lf\n",i,s1.x,s1.y);*/
    }
    cpdf_closepath(pdf);
    cpdf_stroke(pdf);


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
        cpdf_moveto(pdf,s1.x,s1.y);
        cpdf_lineto(pdf,s11.x,s11.y);
        cpdf_closepath(pdf);
        cpdf_stroke(pdf);

        cpdf_beginText(pdf,0);
        cpdf_setFont(pdf,"Times-Bold","MacRomanEncoding",14.0);
        cpdf_text(pdf,s12.x-10,s12.y+26,0.0,sss1);
        cpdf_endText(pdf);
        
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
        cpdf_moveto(pdf,s1.x,s1.y);
        cpdf_lineto(pdf,s11.x,s11.y);
        cpdf_closepath(pdf);
        cpdf_stroke(pdf);

        cpdf_beginText(pdf,0);
        cpdf_setFont(pdf,"Times-Bold","MacRomanEncoding",14.0);
        cpdf_text(pdf,s12.x-10,s12.y+26,0.0,sss1);
        cpdf_endText(pdf);
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
        cpdf_moveto(pdf,s1.x,s1.y);
        cpdf_lineto(pdf,s11.x,s11.y);
        cpdf_closepath(pdf);
        cpdf_stroke(pdf);

        cpdf_beginText(pdf,0);
        cpdf_setFont(pdf,"Times-Bold","MacRomanEncoding",14.0);
        cpdf_text(pdf,s12.x-10,s12.y+26,0.0,sss1);
        cpdf_endText(pdf);
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
        cpdf_moveto(pdf,s1.x,s1.y);
        cpdf_lineto(pdf,s11.x,s11.y);
        cpdf_closepath(pdf);
        cpdf_stroke(pdf);

        cpdf_beginText(pdf,0);
        cpdf_setFont(pdf,"Times-Bold","MacRomanEncoding",14.0);
        cpdf_text(pdf,s12.x-10,s12.y+26,0.0,sss1);
        cpdf_endText(pdf);
    }
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

void draw_times(gdImagePtr im)
{
    char *sss1;

    sss1=(char *)malloc(sizeof(char)*5);

    sprintf(sss1,"Data : %lf sec.",readdata_time-begin_time);
    gdImageString(im, fonts, 18, 75, sss1, white);

    sprintf(sss1,"Prep : %lf sec.",grad_time-readdata_time);
    gdImageString(im, fonts, 18, 85, sss1, white);

    sprintf(sss1,"Bound: %lf sec.",bounds_time-grad_time);
    gdImageString(im, fonts, 18, 95, sss1, white);

    sprintf(sss1,"Figs : %lf sec.",figs_time-bounds_time);
    gdImageString(im, fonts, 18,105, sss1, white);

    sprintf(sss1,"Stars: %lf sec.",stars_time-bounds_time);
    gdImageString(im, fonts, 18,115, sss1, white);

    sprintf(sss1,"Plan.: %lf sec.",planets_time-stars_time);
    gdImageString(im, fonts, 18,125, sss1, white);

    sprintf(sss1,"Horiz: %lf sec.",horiz_time-planets_time);
    gdImageString(im, fonts, 18,135, sss1, white);
    get_the_time(&finish_time);
    process_time=finish_time-begin_time;

    sprintf(sss1,"Total: %lf sec.",process_time);
    gdImageString(im, fonts, 18, 60, sss1, white);
}

void allocate_colors(gdImagePtr im)
{
    black	    = gdTrueColor( 0, 0, 0);
    white	    = gdTrueColor( 255, 255, 255);
    yellow          = gdTrueColor( 255, 255, 0);
    red	            = gdTrueColor( 255, 0, 0);
    green	    = gdTrueColor( 0, 255, 0);
    blue	    = gdTrueColor( 0, 0, 255);
    darkgreen       = gdTrueColor( 0, 128, 0);
    spectr[0]       = gdTrueColor( 0, 255, 255);
    spectr[1]       = gdTrueColor( 120, 255, 255);
    spectr[2]       = gdTrueColor( 255, 255, 255);
    spectr[3]       = gdTrueColor( 255, 255, 128);
    spectr[4]       = gdTrueColor( 255, 255, 0);
    spectr[5]       = gdTrueColor( 255, 128, 0);
    spectr[6]       = gdTrueColor( 255, 64, 64);
}

void allocate_colors_pdf(void)
{
    black	    = 0 * 65536 + 0 * 256 + 0;
    white	    = 255 * 65536 + 255 * 256 + 255;
    yellow          = 255 * 65536 + 255 * 256 + 0;
    red	            = 255 * 65536 + 0 * 256 + 0;
    green	    = 0 * 65536 + 255 * 256 + 0;
    blue	    = 0 * 65536 + 0 * 256 + 255;
    darkgreen       = 0 * 65536 + 128 * 256 + 0;
    spectr[0]       = 0 * 65536 + 255 * 256 + 255;
    spectr[1]       = 120 * 65536 + 255 * 256 + 255;
    spectr[2]       = 255 * 65536 + 255 * 256 + 255;
    spectr[3]       = 255 * 65536 + 255 * 256 + 128;
    spectr[4]       = 255 * 65536 + 255 * 256 + 0;
    spectr[5]       = 255 * 65536 + 128 * 256 + 0;
    spectr[6]       = 255 * 65536 + 64 * 256 + 64;
}

void prepare_image_pdf(CPDFdoc *pdf)
{
    CPDFplotDomain *myDomain, *oldDomain;
    cpdf_enableCompression(pdf, YES);
    cpdf_init(pdf);
    cpdf_setDefaultDomainUnit(pdf,0.12);
    cpdf_pageInit(pdf, 1, PORTRAIT, A4, A4);

    /*myDomain = cpdf_createPlotDomain(0.5*inch,3.5*inch,7.0*inch,7.0*inch,
                             0.,1340.,1370.,0.,LINEAR,LINEAR,0); */
    myDomain = cpdf_createPlotDomain(0.5*inch,3.5*inch,7.0*inch,7.0*inch,
                                     0.,SIZEX+60.,SIZEX+60.0,0.,
                                     LINEAR,LINEAR,0);
    oldDomain = cpdf_setPlotDomain(pdf, myDomain);
    cpdf_setgrayStroke(pdf,0.0);

    cpdf_setgrayFill(pdf, 0.0);    /* Black */
    cpdf_setlinewidth(pdf,0.25);
    //cpdf_beginText(pdf, 0);
    //cpdf_setFont(pdf, "Times-Italic", "MacRomanEncoding", 16.0);
    //cpdf_text(pdf, 1.6, 1.0, 0.0, "SKY_CHART IMAGE");
    //cpdf_endText(pdf);

    /////////not necessary to do//////////////////
    if(colstars==1) {
        cpdf_moveto(pdf,0,0);
        cpdf_closepath(pdf);
        cpdf_stroke(pdf);

        cpdf_setgrayFill(pdf,0.);
        cpdf_setgrayStroke(pdf,0.);
        cpdf_moveto(pdf,-50,-100);
        cpdf_lineto(pdf,SIZEX*1.0+100,-100);
        cpdf_lineto(pdf,SIZEX*1.0+100,SIZEX*1.0+100);
        cpdf_lineto(pdf,-50,SIZEX*1.0+100);
        cpdf_lineto(pdf,-50,-100);
        cpdf_closepath(pdf);
        cpdf_fill(pdf);
    }

}

void prepare_image(gdImagePtr im)
{
    int h,H;
    FILE *fgrad;
    char *fgn;
    int i;
    int az,AZ;
    point3 pnt0,pnt1;
    starxy s1;
    int maxcol_gr;
    gdImagePtr im1;

//    if (((heightc!=0)&&(heightc!=90))||((SIZEX!=800)&&(SIZEX<1200)))
    if (chosen_const<0) {
        if (((heightc!=0)&&(heightc!=90))||(SIZEX<800))
        {
            gdImageDestroy(im);
            im = gdImageCreateTrueColor(SIZEX+60, SIZEY);

            gdImageColorExact(im, 0,0,0);
            for (i=0;i<60;i++)
            {
                blue_gr[59-i]=gdTrueColor( 0, 0, (int)i*2);
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
        } else
        {
            int trnsc,colpix,h,az;
            fgn=(char *)malloc(4096*sizeof(char));
            sprintf(fgn,"/astrodata/sky_chart/template/gradient/grad_h%d_s%d.gd",(int)heightc,SIZEX);
            if((fgrad=fopen(fgn, "rb"))==NULL) dummy_gif(3);

            im1 = gdImageCreateFromGd(fgrad);
            fclose(fgrad);
            trnsc = gdImageGetTransparent(im1);
            maxcol_gr=gdImageColorsTotal(im1);

            gdImageDestroy(im);
            im = gdImageCreateTrueColor(im1->sx, im1->sy);
            gdImageColorExact(im, 0,0,0);
            SIZEX=im1->sx-60;
            SIZEY=im1->sy;
            if(maxcol_gr>60) maxcol_gr=60;
            for(i=0;i<maxcol_gr;i++){
                blue_gr[maxcol_gr-i-1]=gdTrueColor(im1->red[i],
                                                   im1->green[i],
                                                   im1->blue[i]);
            }

            for(az=0;az<SIZEX+60;az++)
            {
                for (h=0;h<SIZEY;h++)
                {
                    colpix=gdImageGetPixel(im1,az,h);

                    if (colpix!=trnsc)
                    {
                        gdImageSetPixel(im,az,h,
                                        gdTrueColor(im1->red[colpix],
                                                    im1->green[colpix],
                                                    im1->blue[colpix]));
                    }
                }
            }
            //gdImageDestroy(im1);
        }
    } else
    {
        gdImageDestroy(im);
        im = gdImageCreateTrueColor(SIZEX+60, SIZEY);

        gdImageColorExact(im, 0,0,0);
        for (i=0;i<60;i++)
        {
            blue_gr[59-i]=gdTrueColor( 0, 0, (int)i*2);
        }
    }
    for (i=0;i<60;i++)
    {
        mw_gr[59-i]=gdTrueColor( i, i, (int)i*2);
    }
}

void draw_hrzn_map(gdImagePtr im)
{
    FILE *fgif;
    gdImagePtr him;
    double azimc1;
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
    char *fhrzmap;
    point3 pnt0,pnt1;
    starxy s1;

    fhrzmap=(char *)malloc(4096*sizeof(char));
    if (hrz_map==1)
//    if ((((heightc!=0)&&(heightc!=90))||((SIZEX!=800)&&(SIZEX<1200)))||(heightc==90)&&(azimc!=0))
    if ((((heightc!=0)&&(heightc!=90))||(SIZEX<800))||(heightc==90)&&(azimc!=0))
    {

        sprintf(fhrzmap, "/astrodata/sky_chart/themes/%d.gif", theme_n);
//        fgif = fopen("./themes/2.gif", "rb");
        query_string_s=fhrzmap;
        if((fgif = fopen(fhrzmap, "rb"))==NULL) dummy_gif(2);
        him = gdImageCreateFromGif(fgif);
        fclose(fgif);
        trnsc = gdImageGetTransparent(him);
        maxcol = gdImageColorsTotal(him);
        if ((maxcol<0)||(maxcol>256)) dummy_gif(2);
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
            other_colors[kkk] = gdTrueColor(rh[kkk],gh[kkk],bh[kkk]);
        }
        //fprintf(ste,"Transp color index=%d\n",trnsc);
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
    } else
    {
        azimc1=azimc;
        if (azimc1<0) azimc1+=360;
///////        if (heightc==0)  sprintf(fhrzmap, "template/horizont/%d/%d_a%d_h0_s%d.gif", theme_n,theme_n,(int)azimc1, SIZEX);
        if (heightc==0)  sprintf(fhrzmap, "/astrodata/sky_chart/template/horizont/%d/%d_a%d_h0_s%d.gd", theme_n,theme_n,(int)azimc1, SIZEX);
///////        if (heightc==90) sprintf(fhrzmap, "template/horizont/%d/%d_a0_h90_s%d.gif", theme_n,theme_n,SIZEX);
        if (heightc==90) sprintf(fhrzmap, "/astrodata/sky_chart/template/horizont/%d/%d_a0_h90_s%d.gd", theme_n,theme_n,SIZEX);
        if ((fgif = fopen(fhrzmap, "rb"))==NULL) dummy_gif(4);
//        fgif = fopen("./themes/2.gif", "rb");
////////        him = gdImageCreateFromGif(fgif);
        him = gdImageCreateFromGd(fgif);
        fclose(fgif);
        trnsc = gdImageGetTransparent(him);
        maxcol = gdImageColorsTotal(him);
        if ((maxcol<0)||(maxcol>256)) dummy_gif(4);
        rh=(int *)malloc(sizeof(int)*(maxcol));
        gh=(int *)malloc(sizeof(int)*(maxcol));
        bh=(int *)malloc(sizeof(int)*(maxcol));
//        fprintf(ste,"MAXCOL=%d\n",maxcol);
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
            other_colors[kkk] = gdTrueColor(rh[kkk],gh[kkk],bh[kkk]);
        }
//        fprintf(ste,"Transp color index=%d\n",trnsc);
        for(az=0;az<SIZEX+60;az++)
        {
            for (h=0;h<SIZEY;h++)
            {
                colpix=gdImageGetPixel(him,az,h);

                if (colpix>180)
                {
                    colcur=180;
                } else colcur=colpix;

                if (colpix!=trnsc)
                {
                    gdImageSetPixel(im,az,h,other_colors[colcur]);
                }
            }
        }
    }
}

void draw_legend_pdf(CPDFdoc *pdf)
{
    int i;
    int r;
    char *c;
    char *h;
    static char *k[7]={"O","B","A","F","G","K","M",};
    c=(char *)malloc(sizeof(char)*5);
    h=(char *)malloc(sizeof(char)*25);

    h="Stellar Magnitudes";
    if(colstars==1) {
        cpdf_setrgbcolor(pdf,1.,1.,1.);
    } else {
        cpdf_setrgbcolor(pdf,0.,0.,0.);
    }
    cpdf_beginText(pdf,0);
    cpdf_setFont(pdf,"Times-Bold","MacRomanEncoding",14.0);
    cpdf_text(pdf,15,10,0.0,h);
    for (i=0;i<=(int)stmag;i++)
    {
        sprintf(c,"%d",i);
        cpdf_text(pdf, 13+i*40, 80, 0, c);
    }
    cpdf_endText(pdf);
    if(colstars==1) {
        cpdf_setgrayFill(pdf,1.0);
        cpdf_setgrayStroke(pdf,0.);
    } else {
        cpdf_setgrayFill(pdf,0.0);
        cpdf_setgrayStroke(pdf,1.0);
    }
    for (i=0;i<=(int)stmag;i++)
    {
        //      r = (int)((6.0*(exp(1.585*log(0.1+(stmag-i)))))/(int)(stmag));
        r = comp_radius(10*i,stmag);
        print_circle_pdf(pdf, 20+i*40, 30, r*1280.0/SIZEX);
    }
    if (colstars==1)
    {
//        h="Spectral Classes";
        for (i=0;i<=6;i++)
        {
            double rs = (double)( spectr[i] >> 16)/255.0;
            double gs = (double)((spectr[i] >> 8 ) % 256)/255.0;
            double bs = (double)( spectr[i] % 256)/255.0;
            cpdf_setrgbcolor(pdf,rs,gs,bs);
            cpdf_setrgbcolorFill(pdf,rs,gs,bs);
            cpdf_setrgbcolorStroke(pdf,0.,0.,0.);

            r = comp_radius(0,stmag);
            print_circle_pdf(pdf, SIZEX-200+i*40, 30, r*1280.0/SIZEX);
        }
        cpdf_setrgbcolor(pdf,1.,1.,1.);
        cpdf_beginText(pdf,0);
        cpdf_setFont(pdf,"Times-Bold","MacRomanEncoding",14.0);
        h="Spectral Classes";
        cpdf_text(pdf, SIZEX-200, 10, 0.0, h);
        for(i=0;i<=6;i++){
            cpdf_text(pdf, SIZEX-210+i*40, 80, 0.0, k[i]);
        }
        cpdf_endText(pdf);
    }
}

void draw_legend(gdImagePtr im)
{
    int i;
    int r;
    unsigned char *c;
    unsigned char *h;
    //char *k[8];
    static char *k[7]={"O","B","A","F","G","K","M",};
    c=(unsigned char *)malloc(sizeof(char)*5);
    h=(unsigned char *)malloc(sizeof(char)*25);
    h= (lang==1)?"�������� ��������" : (lang==0)? "Stellar Magnitudes" : "Stellar Magnitudes";
    //h="Stellar Magnitudes";
    gdImageString(im, fonts, 15, 10, h, white);
    for (i=0;i<=(int)stmag;i++)
    {
        //      r = (int)((6.0*(exp(1.585*log(0.1+(stmag-i)))))/(int)(stmag));
        r = comp_radius(10*i,stmag);
        sprintf(c,"%d",i);
        print_circle(im, 20+i*20, 30, r, white, SIZEX+60);
        gdImageString(im, fonts, 18+i*20, 45, c, white);
    }

    if (colstars==1)
    {
//        h="Spectral Classes";
        h=(lang==1)? "������������ ������":(lang==0)? "Spectral Classes" : "Spectral Classes";
        gdImageString(im, fonts, SIZEX-77, 10, h, white);
        for (i=0;i<=6;i++)
        {
            r = comp_radius(0,stmag);
            print_circle(im, SIZEX-90+i*20, 30, r, spectr[i], SIZEX+60);
            gdImageString(im, fonts, SIZEX-92+i*20, 45, k[i], white);
        }
    }
}

void draw_mark(gdImagePtr im)
{
    starxy s1;
    
    s1=CompPointCoord(azimc,heightc,S_time-mark_ra,mark_dec,0, stmag, 0.00);
    if (s1.sm > 0)
    {
        gdImageArc(im,s1.x,s1.y,mark_size,mark_size,0,360,white);
        gdImageArc(im,s1.x,s1.y,mark_size*0.7,mark_size*0.7,0,360,white);
        gdImageLine(im,s1.x,s1.y-mark_size/2.,s1.x,s1.y-mark_size/1.4,white);
        gdImageLine(im,s1.x,s1.y+mark_size/2.,s1.x,s1.y+mark_size/1.4,white);
        gdImageLine(im,s1.x-mark_size/2.,s1.y,s1.x-mark_size/1.4,s1.y,white);
        gdImageLine(im,s1.x+mark_size/2.,s1.y,s1.x+mark_size/1.4,s1.y,white);
    }
}

int cgiMain()
{
    gdImagePtr im;
    CPDFdoc *pdf;

    int	u;
    int	d;
    int ttt;
//    int	xx,yy,xx1,yy1,rr,coln;
    int	   rr,coln;
    double xx,yy,xx1,yy1;

    int xcsi,ycsi;
    char *bufPDF;
    int pdf_length;
    double
        aa,dd,aa1,dd1;
    point3 pntl;
    char **numarrs;

    cgiFormResultType cgi_r;

    int ll;
    
    ttt=0;
    pdf_output=0;
    //if (argc > 1) pdf_output=1;
    fonts=gdFontSmall;
    fontl=gdFontLarge;
    fontmb=gdFontMediumBold;
    fontg=gdFontGiant;

    ste=fopen("/dev/stderr","wt");
    
    //gettimeofday(&timev,&timez);
    //begin_time=(double)timev.tv_sec+(double)timev.tv_usec/1000000.0;
    get_the_time(&begin_time);
    
    query_string_s=(char *)calloc(16384,sizeof(char));
//    theme_n=(char *)malloc(256*sizeof(char));
    disp_planets=0;
    theme_n=0;
    colstars=0;
    hrz_map=1;
    S_time=5.3;
//    y00=-1.0;
    dcnam=0;

    if ((fbs=fopen("/astrodata/sky_chart/dat/bs.bin","rb"))==NULL) dummy_gif(1);

    //ste =fopen("/dev/stderr","wt");
    ste=stderr;
     
    d=0;
    read_data();
    read_bs();
    fclose(fbs);

    get_the_time(&readdata_time);
    
    cgi_r = cgiFormDouble("ut",&ut,0.0);
    cgi_r = cgiFormInteger("day",&day,1);
    cgi_r = cgiFormInteger("month",&month,1);
    cgi_r = cgiFormInteger("year",&year,2002);
    cgi_r = cgiFormDouble("longitude",&longi,-37.5);
    cgi_r = cgiFormDouble("latitude",&lati,55.75);
    cgi_r = cgiFormDouble("azimuth",&azimc,0.0);
    cgi_r = cgiFormDouble("height",&heightc,0.0);
    cgi_r = cgiFormDouble("m",&stmag,4.5);
    cgi_r = cgiFormInteger("dgrids",&dgrids,0);
    cgi_r = cgiFormInteger("dcbnd",&dcbnd,0);
    cgi_r = cgiFormInteger("dfig",&dfigs,1);
    cgi_r = cgiFormInteger("colstars",&colstars,1);
    cgi_r = cgiFormInteger("names",&dcnam,1);
    cgi_r = cgiFormInteger("xs",&SIZEX,1280);
    cgi_r = cgiFormInteger("theme",&theme_n,0);
    cgi_r = cgiFormInteger("dpl",&disp_planets,1);
    cgi_r = cgiFormInteger("pdf",&pdf_output,0);
    cgi_r = cgiFormInteger("chosen_const",&chosen_const,-5);
    cgi_r = cgiFormDouble("mark_ra",&mark_ra,0.0);
    cgi_r = cgiFormDouble("mark_dec",&mark_dec,0.0);
    cgi_r = cgiFormInteger("mark_size",&mark_size,-1);
    cgi_r = cgiFormInteger("drawmw",&drawmw,1);
    cgi_r = cgiFormInteger("lang",&lang,1);

    checkconst=0;
    chconstnum=0;
    cgi_r = cgiFormStringMultiple("constonly",&numarrs);
    if(cgi_r == cgiFormSuccess) {
        int ll=0;
        fprintf(stderr,"Found!!!\n");
        for(ll=0;numarrs[ll]!=NULL;ll++){
            chconstarr[chconstnum]=atoi(numarrs[ll]);
            chconstnum++;
            fprintf(stderr,"const_num=%s\n",numarrs[ll]);
        }
        checkconst=1;
    }
    cgiStringArrayFree(numarrs);

    //if (!SIZEX) SIZEX=800;
    //if (!stmag) stmag=4.5;
    if (pdf_output == 1)
        if (SIZEX < 1600) SIZEX=1280.;
    if (theme_n<=0) hrz_map=0;
    if ((!chosen_const)||(chosen_const>87)||(dcbnd==0))
        chosen_const=-2; else
            chosen_const--;

    year-=1900;
    year0=365.24219879-0.0000000614*year;
    S_time=comp_s_time();
    //    Grid_cell_set(xcsi,ycsi);
    //    if (zoomk<0.5) zoomk=1;
    //    SIZEY=3*SIZEX/4;

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

    if (pdf_output != 1) {
///////////////////////// JPEG DO NOT TOUCH !!!!!! ////////////////////
    	im = gdImageCreateTrueColor(1, 1);

    	prepare_image(im);
    	allocate_colors(im);
        if (drawmw==1) CompPlotMW(im,zoomk,alss,delss);
    	canvas_init(im,alss,delss,zoomk,darkgreen);
    	get_the_time(&grad_time);
        if (chosen_const>=0)
            draw_horizont(im);
        const_lines=0;
    	const_bnd=1;
    	coln=1;
    	//coln=0;
        gdImageSetThickness(im,1.0);
        Dashed[0]=darkgreen;
        Dashed[1]=darkgreen;
        Dashed[2]=darkgreen;
        Dashed[3]=gdTransparent;
        Dashed[4]=gdTransparent;
        Dashed[5]=gdTransparent;
        gdImageSetStyle(im,Dashed,6);
        if (dcbnd==1)
    	{
            CompCoord(im,zoomk,alss,delss);
            for(d=1;d<CBND;d++)
            {
            	xx=(int)(xconst[d-1]);
            	yy=(int)(yconst[d-1]);
            	xx1=(int)(xconst[d]);
            	yy1=(int)(yconst[d]);

            	aa=conbnd[2*(d-1)];
            	dd=conbnd[2*(d-1)+1];
            	aa1=conbnd[2*d];
            	dd1=conbnd[2*d+1];
            	if (((aa1*aa1+dd1*dd1)<0.05)||((aa*aa+dd*dd)<0.05))
            	{
                    xx1=(int)(xconst[d+coln]);
               	    yy1=(int)(yconst[d+coln]);
                    coln=d+1;
                    //coln=d;
                    d+=2;
            	} else
            	{
                    if ((xx1>-50)&&(xx1<(1280*imzoom+50))&&(yy1>-50)&&(yy1<(1280*imzoom+50))&&(xx>-50)&&(xx<(1280*imzoom+50))&&(yy>-50)&&(yy<(1280*imzoom+50)))
                    	gdImageLine(im,xx1,yy1,xx,yy,red);
                }
            }
            xx1=xconst[10898]; yy1=yconst[10898];
            xx =xconst[10897]; yy =yconst[10897];
            if ((xx1>-50)&&(xx1<(1280*imzoom+50))&&(yy1>-50)&&(yy1<(1280*imzoom+50))&&(xx>-50)&&(xx<(1280*imzoom+50))&&(yy>-50)&&(yy<(1280*imzoom+50)))
                gdImageLine(im,xx1,yy1,xx,yy,red);
            xx1=xconst[10897]; yy1=yconst[10897];
            xx =xconst[10998]; yy =yconst[10998];
            if ((xx1>-50)&&(xx1<(1280*imzoom+50))&&(yy1>-50)&&(yy1<(1280*imzoom+50))&&(xx>-50)&&(xx<(1280*imzoom+50))&&(yy>-50)&&(yy<(1280*imzoom+50)))
                gdImageLine(im,xx1,yy1,xx,yy,red);
            //gdImageLine(im,xconst[1],yconst[1],xx1,yy1,yellow);
    	}
        get_the_time(&bounds_time);
        if ((dfigs==1)||(dcnam==1))
        {
            const_bnd=0;
            if (dfigs==1) const_lines=1;
            CompCoord(im,zoomk,alss,delss);
        }
        dcnam=0;
        get_the_time(&figs_time);
        const_lines=0;
        const_bnd=0;
        CompCoord(im,zoomk,alss,delss);
        get_the_time(&stars_time);
        if (disp_planets==1)
        {
            compute_planets_pos();
            draw_planets(im);
        }
        get_the_time(&planets_time);
        draw_hrzn_map(im);
    	draw_legend(im);
        if ((mark_size>2)&&(mark_ra>=0)&&(mark_ra<24.0)&&(mark_dec>=-90)&&(mark_dec<=90))
            draw_mark(im);
        get_the_time(&horiz_time);
        draw_horizont(im);
#ifdef DEBUG
        draw_times(im);
#endif
        //fclose(ste);
//        print_png_header();
//        gdImagePngEx(im,stdout,1);
//        print_jpeg_header();
//        gdImageJpeg(im,stdout,95);
        gdImageTrueColorToPalette(im,1,256);
        print_gif_header();
        gdImageGif(im,stdout);
        gdImageDestroy(im);
    } else
    {
//////////////////// PDF //////////////////////////////////
        pdf = cpdf_open(0, NULL);

    	allocate_colors_pdf();
    	prepare_image_pdf(pdf);
//        draw_horizont_pdf(pdf);
    	//allocate_colors(im);
    	draw_legend_pdf(pdf);
        if (drawmw==1) CompPlotMW_pdf(pdf,zoomk,alss,delss);
    	canvas_init_pdf(pdf,alss,delss,zoomk,darkgreen);
    	get_the_time(&grad_time);
    	const_lines=0;
    	const_bnd=1;
    	coln=1;
        draw_horizont_pdf(pdf);
        cpdf_closepath(pdf);
        if (dcbnd==1)
    	{
            cpdf_setlinewidth(pdf,0.7);
            CompCoord_pdf(pdf,zoomk,alss,delss);
            for(d=1;d<CBND;d++)
            {
                if ((d % 300)==0)
                {
                    cpdf_closepath(pdf);
                    cpdf_stroke(pdf);
                    if(colstars==1) {
                        cpdf_setrgbcolorStroke(pdf,1.,0.9,0);
                    } else {
                        cpdf_setgrayStroke(pdf,0.);
                    }
                }
                xx=(xconst[d-1]);
            	yy=(yconst[d-1]);
            	xx1=(xconst[d]);
            	yy1=(yconst[d]);

            	aa=conbnd[2*(d-1)];
                dd=conbnd[2*(d-1)+1];
                aa1=conbnd[2*d];
                dd1=conbnd[2*d+1];
                if (((aa1*aa1+dd1*dd1)<0.05)||((aa*aa+dd*dd)<0.05))
                {
                    xx1=(xconst[d+coln]);
                    yy1=(yconst[d+coln]);
                    coln=d+1;
                    d++;d++;
                } else
                {
                    if ((xx1>-50)&&(xx1<(1280*imzoom+50))&&(yy1>-50)&&(yy1<(1280*imzoom+50))&&(xx>-50)&&(xx<(1280*imzoom+50))&&(yy>-50)&&(yy<(1280*imzoom+50)))
                    {
                        cpdf_moveto(pdf,xx1,yy1);
                        cpdf_lineto(pdf,xx,yy);
                    }
                }
            }
            xx1=xconst[10898]; yy1=yconst[10898];
            xx =xconst[10897]; yy =yconst[10897];
            if ((xx1>-50)&&(xx1<(1280*imzoom+50))&&(yy1>-50)&&(yy1<(1280*imzoom+50))&&(xx>-50)&&(xx<(1280*imzoom+50))&&(yy>-50)&&(yy<(1280*imzoom+50)))
            {
                cpdf_moveto(pdf,xx1,yy1);
                cpdf_lineto(pdf,xx,yy);
            }
            xx1=xconst[10897]; yy1=yconst[10897];
            xx =xconst[10998]; yy =yconst[10998];
            if ((xx1>-50)&&(xx1<(1280*imzoom+50))&&(yy1>-50)&&(yy1<(1280*imzoom+50))&&(xx>-50)&&(xx<(1280*imzoom+50))&&(yy>-50)&&(yy<(1280*imzoom+50)))
            {
                cpdf_moveto(pdf,xx1,yy1);
                cpdf_lineto(pdf,xx,yy);
            }
            cpdf_setlinewidth(pdf,0.25);
        }
        cpdf_closepath(pdf);
        get_the_time(&bounds_time);
        if ((dfigs==1)||(dcnam==1))
        {
            const_bnd=0;
            cpdf_setlinewidth(pdf,0.7);
            if (dfigs==1) const_lines=1;
            CompCoord_pdf(pdf,zoomk,alss,delss);
            cpdf_setlinewidth(pdf,0.25);
        }
        cpdf_closepath(pdf);
        dcnam=0;
        get_the_time(&figs_time);
        const_lines=0;
        const_bnd=0;
        CompCoord_pdf(pdf,zoomk,alss,delss);
        get_the_time(&stars_time);
        if (disp_planets==1)
        {
            compute_planets_pos();
            draw_planets_pdf(pdf);
        }
        get_the_time(&planets_time);
        //draw_hrzn_map(im);
        get_the_time(&horiz_time);
        //draw_horizont_pdf(pdf);
//#ifdef DEBUG
        //draw_times(im);
//#endif
    	fclose(ste);
        cpdf_finalizeAll(pdf);
        bufPDF = cpdf_getBufferForPDF(pdf, &pdf_length);
        print_pdf_header(pdf_length);
        fwrite((void *)bufPDF, 1, (size_t)pdf_length, stdout);
        //cpdf_savePDFmemoryStreamToFile(pdf, "/dev/stdout");
        cpdf_close(pdf);
    }
    return 0;
}

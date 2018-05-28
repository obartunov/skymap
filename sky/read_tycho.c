#include <math.h>
#include <stdlib.h>
#include "skyc.h"

long int *read_index_tycho(void)
{
    FILE *ft_ind;
    double tmp1;
    int tmp2;
    int i;
    unsigned long t_index;
    long int *t_ind;
    t_ind=(long int *)malloc(865*sizeof(long int));
    if((ft_ind=fopen(TYCHO_INDEX,"r"))==NULL) return NULL;
    for (i=0;i<864;i++)
    {
        fscanf(ft_ind,"%lf %d %lu\n",&tmp1,&tmp2,&t_index);
        t_ind[i]=t_index;
    }
    t_ind[864]=TYC_COUNT;
    fclose(ft_ind);
    return t_ind;
}

long int read_zone_tycho(MapParamsPtr skymap_par, 
                   long int *t_ind, FILE *ftyc, int j,
                   double ** tyccoord, short int ** tycmag,
                   double ** tycb_v, double ** tyc_pmra,
                   double ** tyc_pmdec, double ** tyc_par)
{
  long int i;
  int tcrd;
  unsigned char tycrec[16];
  long int NMAX;

  NMAX=(t_ind[j]-t_ind[j-1]);
  *tyccoord=(double *)malloc(2*sizeof(double)*NMAX);
  *tycmag=(short int *)malloc(sizeof(short int)*NMAX);
  *tycb_v=(double *)malloc(sizeof(double)*NMAX);
  *tyc_pmra=(double *)malloc(sizeof(double)*NMAX);
  *tyc_pmdec=(double *)malloc(sizeof(double)*NMAX);
  *tyc_par=(double *)malloc(sizeof(double)*NMAX);
  fseek(ftyc,16*t_ind[j-1],0);
  //fprintf(stderr,"NMAX=%d\n",NMAX);
  for(i=1;i<=NMAX;i++)
  {
      tcrd=fread(tycrec,sizeof(unsigned char),16,ftyc);
      (*tyccoord)[2*(i-1)+0]=24.000000*(((long int)tycrec[0]+256*(long int)tycrec[1]+65536*(long int)tycrec[2])/16777216.0); //RA
      (*tyccoord)[2*(i-1)+1] = (tycrec[5] >= 128)? 90.0*((tycrec[3]+256*tycrec[4]+65536*tycrec[5])/8388608.0-2.0) : 90.0*((tycrec[3]+256*tycrec[4]+65536*tycrec[5])/8388608.0); //DEC
      (*tycmag)[i-1]=(short int)((tycrec[6]+256*tycrec[7])/100);
      (*tycb_v)[i-1]=(double)((tycrec[8]+256*tycrec[9])/1000.0);
      if ((*tycb_v)[i-1]>=32.768) (*tycb_v)[i-1]=(*tycb_v)[i-1]-65.536;

      (*tyc_pmra)[i-1]=(double)((tycrec[10]+256*tycrec[11])/10.0);
      if ((*tyc_pmra)[i-1]>=3276.8) (*tyc_pmra)[i-1]=(*tyc_pmra)[i-1]-6553.6;
      (*tyc_pmdec)[i-1]=(double)((tycrec[12]+256*tycrec[13])/10.0);
      if ((*tyc_pmdec)[i-1]>=3276.8) (*tyc_pmdec)[i-1]=(*tyc_pmdec)[i-1]-6553.6;
      (*tyc_par)[i-1]=(double)((tycrec[14]+256*tycrec[15])/10.0);

      if ((*tycmag)[i-1]>(skymap_par->st_maglim*10.+10)) {
          NMAX=i;
          break;
      }
      //fprintf(stderr,"ra=%lf dec=%lf mag=%lf b-v=%lf\n",tyccoord0[2*(i-1)],tyccoord0[2*(i-1)+1],tycmag0[i-1]/10.0,tycb_v0[i-1]);
  }
  return NMAX;
}

int zone_check_tycho(MapParamsPtr skymap_par,int j)
{
    int chs_l=1;
    starxy stemp1;
    double au,du;
    int Value=0;
    au=(j-(int)((double)j/48.0)*48.0)*0.5;
    au=au+.25;
    if (au<0) au+=24;
    if (au>=24) au-=24;
    if(j<=432)
    {
        du=(int)(j/48.0)*10.0;
    }
    else
    {
        du=(int)((864-j)/48)*(-10);
    }
    if (j<=432) du=du+5; else du=du-5;

    if ((90.0-fabs(skymap_par->Dec_center))<=(30.000/skymap_par->zoomk)) chs_l=2;
    if ((fabs(skymap_par->Dec_center)<skymap_par->zoomk*2.)||(fabs(skymap_par->Dec_center)<=20)) chs_l=0;

    switch (chs_l)
    {
    case 0  :
        stemp1=CompEquCoord(skymap_par, au, du, 255);
        break;
    case 1  :
        stemp1=CompMidCoord(skymap_par, au, du, 255);
        break;
    case 2  :
        stemp1=CompPolCoord(skymap_par, au, du, 255);
        break;
    }
    
    skymap_par->DX=0.15*skymap_par->SIZEX*skymap_par->zoomk;
    skymap_par->DY=0.25*skymap_par->SIZEY*skymap_par->zoomk;
    if ((stemp1.x>(-skymap_par->DX*1.2))&&(stemp1.y>(-skymap_par->DY*1.2))&&(stemp1.x<(skymap_par->DX+1280*skymap_par->imzoom))&&(stemp1.y<(skymap_par->DY+960*skymap_par->imzoom)))
    {
        Value=1;
    }
    return Value;
}

long int read_zone_tycho_s(MapParamsPtr skymap_par, long int *t_ind,
                         FILE *ftyc, int j, star **tycho_zone)
{
  long int i;
  int tcrd;
  unsigned char tycrec[16];
  long int NMAX;

  NMAX=(t_ind[j]-t_ind[j-1]);
  *tycho_zone=(star *)malloc(sizeof(star)*NMAX);
  fseek(ftyc,16*t_ind[j-1],0);
  //fprintf(stderr,"NMAX=%d\n",NMAX);
  for(i=1;i<=NMAX;i++)
  {
      tcrd=fread(tycrec,sizeof(unsigned char),16,ftyc);
      (*tycho_zone)[i-1].ra  = 24.000000*(((long int)tycrec[0]+256*(long int)tycrec[1]+65536*(long int)tycrec[2])/16777216.0); //RA
      (*tycho_zone)[i-1].dec = (tycrec[5] >= 128)? 90.0*((tycrec[3]+256*tycrec[4]+65536*tycrec[5])/8388608.0-2.0) : 90.0*((tycrec[3]+256*tycrec[4]+65536*tycrec[5])/8388608.0); //DEC
      (*tycho_zone)[i-1].mag = (double)(tycrec[6]+256*tycrec[7])/1000.0;
      (*tycho_zone)[i-1].b_v =(double)((tycrec[8]+256*tycrec[9])/1000.0);
      if ((*tycho_zone)[i-1].b_v>=32.768) (*tycho_zone)[i-1].b_v=(*tycho_zone)[i-1].b_v-65.536;

      (*tycho_zone)[i-1].pm_ra=(double)((tycrec[10]+256*tycrec[11])/10.0);
      if ((*tycho_zone)[i-1].pm_ra>=3276.8) (*tycho_zone)[i-1].pm_ra=(*tycho_zone)[i-1].pm_ra-6553.6;
      (*tycho_zone)[i-1].pm_dec=(double)((tycrec[12]+256*tycrec[13])/10.0);
      if ((*tycho_zone)[i-1].pm_dec>=3276.8) (*tycho_zone)[i-1].pm_dec=(*tycho_zone)[i-1].pm_dec-6553.6;
      (*tycho_zone)[i-1].par=(double)((tycrec[14]+256*tycrec[15])/10.0);

      if ((*tycho_zone)[i-1].mag>(skymap_par->st_maglim+1.0)) {
          NMAX=i;
          break;
      }
      //fprintf(stderr,"ra=%lf dec=%lf mag=%lf b-v=%lf\n",tyccoord0[2*(i-1)],tyccoord0[2*(i-1)+1],tycmag0[i-1]/10.0,tycb_v0[i-1]);
  }
  return NMAX;
}

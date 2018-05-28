#include <math.h>
#include <stdlib.h>
#include "skyc.h"

long int **read_index_a20(char ***zone_fnames)
{
    FILE *fa20_ind;
    long int tmp2=0;
    double tmp1=0.;
    int i=0,j=0;
    unsigned long int a20_index=0;
    char i_fname[255];
    long int **a20_ind;

    //long int a20_ind[97][24];
    //char    zone_fnames[255][24];
    *zone_fnames=(char **)malloc(24*sizeof(char *));
    for (i=0;i<24;i++) (*zone_fnames)[i]=(char *)malloc(255*sizeof(char));

    a20_ind=(long int **)malloc(97*sizeof(long int *));
    for (i=0;i<97;i++) a20_ind[i]=(long int *)malloc(24*sizeof(long int));

    for (i=0;i<24;i++)
    {
        sprintf(i_fname,"%szone%04d.acc",A20_DATA_PATH,75*i);
        sprintf((*zone_fnames)[i],"%szone%04d.ctb",A20_DATA_PATH,75*i);
        if((fa20_ind=fopen(i_fname,"r"))==NULL) return NULL;
        for (j=0;j<96;j++)
        {
            int nread = fscanf(fa20_ind,"%lf %ld %ld\n",&tmp1,&a20_index,&tmp2);
		    if(nread < 3) { 
				fprintf(stderr, "Isufficienet read  in read_a20.c: %d < 3\n",nread);
				exit(1);
		    }

            a20_ind[j][i]=a20_index;
        }
        a20_ind[96][i]=a20_index+tmp2;
        fclose(fa20_ind);
    }
    return a20_ind;
}


long int read_zone_a20(MapParamsPtr skymap_par,
                       long int **a20_ind, char **zone_fnames,
                       int x, int y, double ** a20coord,
                       short int ** a20bmag, short int ** a20rmag)
{
    FILE *fa20;
    long int i;
    double ra_c,dec_c;
    int mag_c,mag_c8,bm,rm;
    int  a20rd;
    unsigned char a20rec[12];
    short int bmag,rmag;
    long int NMAX;

    if (x>96) x=96;
    if (x<0) x=0;
    if (y>23) y=23;
    if (y<0) y=0;

    NMAX=(a20_ind[x+1][y]-a20_ind[x][y]);
    *a20coord=(double *)malloc(2*sizeof(double)*NMAX);
    *a20bmag=(short int *)malloc(sizeof(short int)*NMAX);
    *a20rmag=(short int *)malloc(sizeof(short int)*NMAX);

    if((fa20=fopen(zone_fnames[y],"r"))==NULL) return -1;
    fseek(fa20,12*a20_ind[x][y],0);
    for (i=1;i<(a20_ind[x+1][y]-a20_ind[x][y]);i++)
    {
        a20rd=fread(a20rec,sizeof(unsigned char),12,fa20);
	    if(!a20rd) { 
			fprintf(stderr, "Canot read tycho zone in read_a20.c\n");
			exit(1);
	    }
        ra_c =(167772.16*a20rec[0]+655.36*a20rec[1]+
                    2.56*a20rec[2]+  0.01*a20rec[3])/(15.*3600.);
        dec_c=(167772.16*a20rec[4]+655.36*a20rec[5]+
                    2.56*a20rec[6]+  0.01*a20rec[7])/3600. - 90.;

        mag_c=16777216*a20rec[8]+65536*a20rec[9]+256*a20rec[10]+a20rec[11];
        if (mag_c<0) mag_c=abs(mag_c);
        mag_c8=(mag_c-1000000000*(int)(mag_c/1000000000));
        bm=(mag_c8-1000000*(int)(mag_c8/1000000));
        bmag = ((bm<=251000)&&(bm>0)) ? (short int)(bm/1000) : 215;
	if (bmag==0) bmag=214;
        if ((int)(mag_c/1000000000)==1) bmag=-bmag;
        rm=(bm-1000*(int)(bm/1000));
        rmag = ((rm<251)&&(rm>0)) ? (short int)rm : 215;
        if ((int)(mag_c/1000000000)==1) rmag=-rmag;
	if (rmag==0) rmag=214;
        (*a20coord)[2*(i-1)] = ra_c;
        (*a20coord)[2*(i-1)+1] = dec_c;
        (*a20bmag)[i-1]=bmag;
        (*a20rmag)[i-1]=rmag;
        
	if (abs(bmag)>(skymap_par->st_maglim*10.+5)) 
	{
          NMAX=i;
          break;
        }
    }
    fclose(fa20);
    return NMAX;
//    fprintf(stderr,"Zone %lf %lf read successfully\n",x*0.25,y*7.5-90.);
}

int zone_check_a20(MapParamsPtr skymap_par,int i,int j)
{
    int chs_l=1,k=0;
    starxy /* not used ? stemp1, */s_corn[8];
    double a_c,d_c,a_cz[8],d_cz[8];
    int Value=0;

    if (i>=96) i-=96;
    if (j>=24) j-=24;

    a_c=i*0.25;
    d_c=7.5*j-90.;
    a_cz[0]=a_c; a_cz[1]=a_c+0.125; a_cz[2]=a_c+0.25;
    a_cz[3]=a_c;                    a_cz[4]=a_c+0.25;
    a_cz[5]=a_c; a_cz[6]=a_c+0.125; a_cz[7]=a_c+0.25;

    d_cz[0]=d_c;       d_cz[1]=d_c;      d_cz[2]=d_c;
    d_cz[3]=d_c+3.750;                   d_cz[4]=d_c+3.750;
    d_cz[5]=d_c+7.50;  d_cz[6]=d_c+7.50; d_cz[7]=d_c+7.50;

    a_c+=.125;
    d_c+=3.75;

    if ((90.0-fabs(skymap_par->Dec_center))<=(30.000/skymap_par->zoomk)) chs_l=2;
    if ((fabs(skymap_par->Dec_center)<skymap_par->zoomk*2.)||(fabs(skymap_par->Dec_center)<=20)) chs_l=0;

    switch (chs_l)
    {
    case 0  :
        for (k=0;k<8;k++)
        {
            s_corn[k]=CompEquCoord(skymap_par, a_cz[k], d_cz[k], 255);
        }
//        stemp1=CompEquCoord(skymap_par, a_c, d_c, 255);
        Value = (((s_corn[0].x<0)&&(s_corn[1].x<0)&&(s_corn[2].x<0))||
                 ((s_corn[0].x>skymap_par->SIZEX)&&(s_corn[1].x>skymap_par->SIZEX)&&(s_corn[2].x>skymap_par->SIZEX))||
                 ((s_corn[0].y<0)&&(s_corn[3].y<0)&&(s_corn[5].y<0))||
                 ((s_corn[0].y>skymap_par->SIZEY)&&(s_corn[3].y>skymap_par->SIZEY)&&(s_corn[5].y>skymap_par->SIZEY))) ? 0 : 1;
        break;
    case 1  :
        for (k=0;k<8;k++)
        {
            s_corn[k]=CompEquCoord(skymap_par, a_cz[k], d_cz[k], 255);
        }
// not used ?        stemp1=CompEquCoord(skymap_par, a_c, d_c, 255);
        Value = (((s_corn[0].x<0)&&(s_corn[1].x<0)&&(s_corn[2].x<0))||
                 ((s_corn[0].x>skymap_par->SIZEX)&&(s_corn[1].x>skymap_par->SIZEX)&&(s_corn[2].x>skymap_par->SIZEX))||
                 ((s_corn[0].y<0)&&(s_corn[3].y<0)&&(s_corn[5].y<0))||
                 ((s_corn[0].y>skymap_par->SIZEY)&&(s_corn[3].y>skymap_par->SIZEY)&&(s_corn[5].y>skymap_par->SIZEY))) ? 0 : 1;
        break;
    case 2  :
        for (k=0;k<8;k++)
        {
            s_corn[k]=CompEquCoord(skymap_par, a_cz[k], d_cz[k], 255);
        }
// not used ?        stemp1=CompEquCoord(skymap_par, a_c, d_c, 255);
        Value = (((s_corn[0].x<0)&&(s_corn[1].x<0)&&(s_corn[2].x<0))||
                 ((s_corn[0].x>skymap_par->SIZEX)&&(s_corn[1].x>skymap_par->SIZEX)&&(s_corn[2].x>skymap_par->SIZEX))||
                 ((s_corn[0].y<0)&&(s_corn[3].y<0)&&(s_corn[5].y<0))||
                 ((s_corn[0].y>skymap_par->SIZEY)&&(s_corn[3].y>skymap_par->SIZEY)&&(s_corn[5].y>skymap_par->SIZEY))) ? 0 : 1;

        break;
    }

    return Value;
}

long int read_zone_a20_s(MapParamsPtr skymap_par,
                         long int **a20_ind, char **zone_fnames,
                         int x, int y,star **a20_zone)
{
    FILE *fa20;
    long int i;
    double ra_c,dec_c;
    int mag_c,mag_c8,bm,rm;
    int  a20rd;
    unsigned char a20rec[12];
    short int bmag,rmag;
    long int NMAX;

    if (x>96) x=96;
    if (x<0) x=0;
    if (y>23) y=23;
    if (y<0) y=0;

    NMAX=(a20_ind[x+1][y]-a20_ind[x][y]);
    *a20_zone=(star *)malloc(sizeof(star)*NMAX);

    if((fa20=fopen(zone_fnames[y],"r"))==NULL) return -1;
    fseek(fa20,12*a20_ind[x][y],0);
    for (i=1;i<(a20_ind[x+1][y]-a20_ind[x][y]);i++)
    {
        a20rd=fread(a20rec,sizeof(unsigned char),12,fa20);
	    if(!a20rd) { 
			fprintf(stderr, "Canot read tycho zone in read_a20.c\n");
			exit(1);
	    }
        ra_c =(167772.16*a20rec[0]+655.36*a20rec[1]+
                    2.56*a20rec[2]+  0.01*a20rec[3])/(15.*3600.);
        dec_c=(167772.16*a20rec[4]+655.36*a20rec[5]+
                    2.56*a20rec[6]+  0.01*a20rec[7])/3600. - 90.;

        mag_c=16777216*a20rec[8]+65536*a20rec[9]+256*a20rec[10]+a20rec[11];
        if (mag_c<0) mag_c=abs(mag_c);
        mag_c8=(mag_c-1000000000*(int)(mag_c/1000000000));
        bm=(mag_c8-1000000*(int)(mag_c8/1000000));
        bmag = ((bm<=251000)&&(bm>0)) ? (short int)(bm/1000) : 215;
	if (bmag==0) bmag=214;
        if ((int)(mag_c/1000000000)==1) bmag=-bmag;
        rm=(bm-1000*(int)(bm/1000));
        rmag = ((rm<251)&&(rm>0)) ? (short int)rm : 215;
        if ((int)(mag_c/1000000000)==1) rmag=-rmag;
	if (rmag==0) rmag=214;
        (*a20_zone)[i-1].ra=ra_c;
        (*a20_zone)[i-1].dec=dec_c;
        (*a20_zone)[i-1].bmag=bmag;
        (*a20_zone)[i-1].rmag=rmag;
        (*a20_zone)[i-1].mag=(double)bmag/10.0;
        
	if (abs(bmag)>(skymap_par->st_maglim*10.+5)) 
	{
          NMAX=i;
          break;
        }
    }
    fclose(fa20);
    return NMAX;
//    fprintf(stderr,"Zone %lf %lf read successfully\n",x*0.25,y*7.5-90.);
}


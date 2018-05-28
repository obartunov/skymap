#include <stdlib.h>
#include "skyc.h"

gal_rc3 * read_rc3data()
{
    gal_rc3 *rc3;
    FILE *frc3;
    unsigned char rc3rec[20];
    int i, rc3r;
    int ngcnum, icnum, ugcnum, pgcnum;

    rc3=(gal_rc3 *)malloc(23023*sizeof(gal_rc3));
    if((frc3=fopen(RC3_DATA,"rb"))==NULL) return NULL;
    for(i=1;i<=23023;i++)
    {
        rc3r=fread(rc3rec,sizeof(unsigned char),20,frc3);
		if(!rc3r) {
			fprintf(stderr, "fread returned zero in read_rc3ata\n");
			exit(1);
		}
        rc3[i-1].ra=24.000000*((double)rc3rec[0]+256*(double)rc3rec[1])/65536.00000;
        if ((rc3rec[2]+256*rc3rec[3]) > 32767)
            rc3[i-1].dec = 90.0*((rc3rec[2]/32400.0+256.0/32400.0*rc3rec[3])-65536.0/32400.0);
        else rc3[i-1].dec = 90.0*((rc3rec[2]/32400.0+256.0/32400.0*rc3rec[3]));

        rc3[i-1].mag=rc3rec[4]/10.0;
        rc3[i-1].type_obj=0;
        rc3[i-1].Asize=(rc3rec[15]+256.0*rc3rec[16])/60.0;
        rc3[i-1].Bsize=(rc3rec[17]+256.0*rc3rec[18])/60.0;
        rc3[i-1].fi=(Pi*rc3rec[5]/180.0);
        ngcnum =rc3rec[6]+rc3rec[7]*256;
        icnum  =rc3rec[8]+rc3rec[9]*256;
        ugcnum =rc3rec[10]+rc3rec[11]*256;
        pgcnum =rc3rec[12]+rc3rec[13]*256+rc3rec[14]*65536;
        (rc3[i-1].p_name)[0]=0;
        (rc3[i-1].name2)[0]=0;
        (rc3[i-1].name3)[0]=0;
        (rc3[i-1].name4)[0]=0;
        if (ngcnum > 0) {
            if (ngcnum < 10000)
                sprintf(rc3[i-1].p_name,"NGC%d",ngcnum);
            else
                sprintf(rc3[i-1].p_name,"NGC%d%c",
                        ngcnum % 10000, 64+(int)(ngcnum/10000));

            if (icnum > 0) {
                sprintf(rc3[i-1].name2,"IC%d",icnum);
                if (ugcnum > 0) {
                    sprintf(rc3[i-1].name3,"UGC%d",ugcnum);
                    if (pgcnum > 0)
                        sprintf(rc3[i-1].name4,"PGC%d",pgcnum);
                } else if (pgcnum > 0)
                    sprintf(rc3[i-1].name3,"PGC%d",pgcnum);
            } else
                if (ugcnum > 0) {
                    sprintf(rc3[i-1].name2,"UGC%d",ugcnum);
                    if (pgcnum > 0)
                        sprintf(rc3[i-1].name3,"PGC%d",pgcnum);
                }
                else if (pgcnum > 0)
                    sprintf(rc3[i-1].name2,"PGC%d",pgcnum);
        } else
            if (icnum > 0) {
                sprintf(rc3[i-1].p_name,"IC%d",icnum);
                if (ugcnum > 0) {
                    sprintf(rc3[i-1].name2,"UGC%d",ugcnum);
                    if (pgcnum > 0)
                        sprintf(rc3[i-1].name3,"PGC%d",pgcnum);
                } else if (pgcnum > 0)
                    sprintf(rc3[i-1].name2,"PGC%d",pgcnum);
            } else
                if (ugcnum > 0) {
                    sprintf(rc3[i-1].p_name,"UGC%d",ugcnum);
                    if (pgcnum > 0)
                        sprintf(rc3[i-1].name2,"PGC%d",pgcnum);
                }
                else if (pgcnum > 0)
                    sprintf(rc3[i-1].p_name,"PGC%d",pgcnum);

    }

    fclose(frc3);
    return rc3;
}

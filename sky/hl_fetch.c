#ifdef USE_HL
#include <stdlib.h>
#include <string.h>
#include "skyc.h"

int hl_fetch(MapParamsPtr skymap_par, gal_rc3 **hl_gal)
{
    int ntuples=-1;
    char *query;
    PGresult *hl_res;
    double fsize;

    if(PQstatus(skymap_par->hl_conn)!=0) {
        return -2;
    }
    fsize=42.0/skymap_par->zoomk;
    if(fsize > 7) fsize=7.0;
    query=(char*)malloc(sizeof(char)*500);
    if(skymap_par->suppress_unkmag_gal==1) {
        sprintf(query,"SELECT pgc,objname,12.0*long(crd2000)/pi(),180.0*lat(crd2000)/pi(),bt,pa,pow(10,logd25)/10.0,pow(10,logd25)/(10.0*pow(10,logr25)),numtype FROM hldata WHERE crd2000 @ '<(%lf,%lf),%lf>'::scircle AND (bt<=%6.2lf AND bt IS NOT NULL) AND numtype NOT IN (3,4,10,11)",
                Pi*skymap_par->RA_center/12.0,
                Pi*skymap_par->Dec_center/180.0,
                Pi*fsize/180.0,
                skymap_par->gal_maglim);
    } else {
        sprintf(query,"SELECT pgc,objname,12.0*long(crd2000)/pi(),180.0*lat(crd2000)/pi(),bt,pa,pow(10,logd25)/10.0,pow(10,logd25)/(10.0*pow(10,logr25)),numtype FROM hldata WHERE crd2000 @ '<(%lf,%lf),%lf>'::scircle AND (bt<=%6.2lf OR bt IS NULL) AND numtype NOT IN (3,4,10,11)",
                Pi*skymap_par->RA_center/12.0,
                Pi*skymap_par->Dec_center/180.0,
                Pi*fsize/180.0,
                skymap_par->gal_maglim);
    }
    /*fprintf(stderr,"%s\n",query);*/
    hl_res=PQexec(skymap_par->hl_conn,query);
    if ((PQresultStatus(hl_res) == PGRES_COMMAND_OK)||
        (PQresultStatus(hl_res) == PGRES_TUPLES_OK)||
        (PQresultStatus(hl_res) == PGRES_COPY_IN)||
        (PQresultStatus(hl_res) == PGRES_COPY_OUT)) {
        int i;
        ntuples=PQntuples(hl_res);
        /*fprintf(stderr,"%d objects selected from HyperLEDA\n",ntuples);*/
        if(ntuples > 0) {
            (*hl_gal)=(gal_rc3 *)malloc(ntuples*sizeof(gal_rc3));
            for (i=0;i<ntuples;i++){
                int pgc;
                char *pgvalue;
                pgvalue=PQgetvalue(hl_res,i,0);
                sscanf(pgvalue,"%d",&pgc);
                pgvalue=PQgetvalue(hl_res,i,1);
                strncpy( (((*hl_gal)[i]).p_name),pgvalue,30);
                pgvalue=PQgetvalue(hl_res,i,2);
                sscanf(pgvalue,"%lf",&(((*hl_gal)[i]).ra));
                if(PQgetisnull(hl_res,i,2)==1) (*hl_gal)[i].ra=-1.0;
                pgvalue=PQgetvalue(hl_res,i,3);
                sscanf(pgvalue,"%lf",&(((*hl_gal)[i]).dec));
                if(PQgetisnull(hl_res,i,3)==1) (*hl_gal)[i].dec=-100.0;
                pgvalue=PQgetvalue(hl_res,i,4);
                sscanf(pgvalue,"%lf",&(((*hl_gal)[i]).mag));
                if(PQgetisnull(hl_res,i,4)==1) (*hl_gal)[i].mag=-1.0;
                pgvalue=PQgetvalue(hl_res,i,5);
                sscanf(pgvalue,"%lf",&(((*hl_gal)[i]).fi));
                (*hl_gal)[i].fi/=(180.0/Pi);
                if(PQgetisnull(hl_res,i,5)==1) (*hl_gal)[i].fi=0.0;
                pgvalue=PQgetvalue(hl_res,i,6);
                sscanf(pgvalue,"%lf",&(((*hl_gal)[i]).Asize));
                if(PQgetisnull(hl_res,i,6)==1) (*hl_gal)[i].Asize=0.0;
                pgvalue=PQgetvalue(hl_res,i,7);
                sscanf(pgvalue,"%lf",&(((*hl_gal)[i]).Bsize));
                if(PQgetisnull(hl_res,i,7)==1) (*hl_gal)[i].Bsize=0.0;
                pgvalue=PQgetvalue(hl_res,i,8);
                sscanf(pgvalue,"%d",&(((*hl_gal)[i]).type_obj));

                /*fprintf(stderr,"i=%d, %s, %10.4f %10.4f %5.0f\n",
                          i,(*hl_gal)[i].p_name,(*hl_gal)[i].ra,(*hl_gal)[i].dec,(*hl_gal)[i].fi);*/

            }
        }
    }
    PQclear(hl_res);
    /*free(query);*/

    return ntuples;
}

int HL_connect(MapParamsPtr skymap_par)
{
    int stat;
    skymap_par->hl_conn = PQsetdbLogin(HL_HOST, HL_PORT, NULL, NULL,
                                     HL_DB,HL_USER,HL_PWD);
    stat=(PQstatus(skymap_par->hl_conn) == CONNECTION_OK)? 0 : 1;
    return stat;
}

void HL_disconnect(MapParamsPtr skymap_par)
{
    PQfinish(skymap_par->hl_conn);
}
#endif

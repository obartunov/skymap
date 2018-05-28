#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "skyc.h"

plotable_obj * read_objstxt(char *filename, long int *objnum)
{
    FILE *fobjs;
    plotable_obj *objs;
    int i=0,ssres=0;
    double ra,dec,ras,decs,objsize,minsymsize;
    int rah,ram,decd,decm,symtype,color,line_to,decsign;
    unsigned char dsign;
    char designation[255];
    char buff[1024];

    if ((fobjs=fopen(filename,"r"))==NULL) return NULL;
    objs=(plotable_obj *)malloc(2048*sizeof(plotable_obj));
    while((!feof(fobjs))&&(i<2047)){
        /*format: 23 13 11.333 +10 12 58.9  12.0    1   10.0   1  0 ObjectAAABBB CCC  */
        /*         R.A.(J2000)  Dec(J2000) objsize typ  size col line_to designation  */
        //if (i>1){
        //    if (log((double)i)/log(2.0) == floor(log((double)i)/log(2.0)))
        //        objs=(plotable_obj *)
        //            realloc(objs,4*i*sizeof(plotable_obj));
        //}
        buff[0]=0;
        //fscanf(fobjs,"%[^\n]",buff);
        //fprintf(stderr,"read_objstxt: |%s|\n",buff);

        //ssres=sscanf(buff,"%d %d %lf %c%d %d %lf %lf %d %lf %d %d %[^\n]",
        //       &rah,&ram,&ras,&dsign,&decd,&decm,&decs,&objsize,
        //       &symtype,&minsymsize,&color,&line_to,designation);
        ssres=fscanf(fobjs,"%d %d %lf %c%d %d %lf %lf %d %lf %d %d %[^\n]",
               &rah,&ram,&ras,&dsign,&decd,&decm,&decs,&objsize,
               &symtype,&minsymsize,&color,&line_to,designation);
        //if ((dsign!=0x2b)&&(dsign!=0x2d)){
        //    ssres=sscanf(buff,"%d %d %lf %d %d %lf %lf %d %lf %d %d %[^\n]",
        //           &rah,&ram,&ras,&decd,&decm,&decs,&objsize,
        //           &symtype,&minsymsize,&color,&line_to,designation);
        //    dsign=0x2b; /* "+" */
        //}
        //if (ssres > 12) {
            ra=(double)rah+(double)ram/60.0+ras/3600.0;
            decsign = (dsign=='-')? -1 : 1;
            if ((dsign!='-')&&(dsign!='+')) {
                decd=10*(dsign-0x30);
                dsign='+';
            }
            dec=((double)decd+(double)decm/60.0+decs/3600.0)*decsign;
            objs[i].ra=ra;
            objs[i].dec=dec;
            objs[i].symtype=symtype;
            objs[i].minsymsize=minsymsize;
            objs[i].color=color;
            objs[i].line_to=line_to;
            objs[i].objsize=objsize;
            strcpy(objs[i].designation,designation);
            i++;
        //}
    }
    *objnum=i;
    fclose(fobjs);
    return objs;
}

plotable_obj * get_objs_from_string(char *inp_string, long int *objnum)
{
    long int str_l;
    char *buff, *inp_s, *parse_s;
    plotable_obj *objs;
    int i=0,sscres=0;
    double ra,dec,ras,decs;
    int rah,ram,decd,decm,decsign;
    unsigned char dsign;
    char designation[25500];

    str_l=strlen(inp_string)+1;
    buff =(char *)malloc(sizeof(char)*str_l);
    inp_s=(char *)malloc(sizeof(char)*str_l);
    //objs=(plotable_obj *)malloc(sizeof(plotable_obj)*(1+str_l/23));
    objs=(plotable_obj *)malloc(sizeof(plotable_obj)*2048);
    inp_s=strcpy(inp_s,inp_string);
    //fprintf(stderr,"Input data:\n%s\n----------------\n",inp_string);
    parse_s=strtok_r(inp_s,"\n",&buff);
    while(parse_s!=NULL) {
        //fprintf(stderr,"i=%d       OKbegin\n",i);
        //fprintf(stderr,"parse_s: %s\n",parse_s);
        designation[0]=0;
        sscres=sscanf(parse_s,"%u %u %lf %c%u %u %lf %[^\n]",
               &rah,&ram,&ras,&dsign,&decd,&decm,&decs,designation);
        if ((dsign!=0x2b)&&(dsign!=0x2d)){
            sscres=sscanf(parse_s,"%u %u %lf %u %u %lf %[^\n]",
                          &rah,&ram,&ras,&decd,&decm,&decs,designation);
            dsign=0x2b; /* "+" */
        }
        //fprintf(stderr,"i=%d result of sscanf=%d\n",i,sscres);
        if (sscres >= 7){
            //fprintf(stderr,"symbol dsign='%c'\n",dsign);
            //fprintf(stderr,"Read from string: %d %d %f %c%d %d %f %s\n",
            //        rah,ram,ras,dsign,decd,decm,decs,designation);
            ra=(double)rah+(double)ram/60.0+ras/3600.0;
            decsign = (dsign=='-')? -1 : 1;
            dec=((double)decd+(double)decm/60.0+decs/3600.0)*decsign;
            objs[i].ra=ra;
            objs[i].dec=dec;
            objs[i].symtype=-1;
            objs[i].minsymsize=0;
            objs[i].color=1;
            objs[i].line_to=0;
            objs[i].objsize=0;
            strcpy(objs[i].designation,designation);
            i++;
        } else {
            sscres=sscanf(parse_s," %*[^\n]");
            fprintf(stderr,"i=%d result of 2-nd sscanf=%d\n",i,sscres);
        }
        //free(parse_s);
        parse_s=strtok_r(NULL,"\n",&buff);
    }
    free(inp_s);
    //free(buff);
    *objnum=i;
    return objs;
}

char *get_filename(long int obj_id, char **obj_title)
{
    FILE *fobjs;
    char *filename=NULL;
    char *obj_title_cur, *filename_cur;
    long int obj_id_cur=-1;
    int display_flag=0;

    if ((fobjs=fopen(OBJ_CAT,"rb"))==NULL) return filename;
    obj_title_cur = (char *)malloc(sizeof(char)*500);
    filename_cur  = (char *)malloc(sizeof(char)*500);
    while ((!feof(fobjs))&& !((obj_id_cur == obj_id)&&(display_flag==1))){
        fscanf(fobjs,"%ld %[^\t] %d %*f %*f %*f %[^\n]",
               &obj_id_cur,obj_title_cur,&display_flag,filename_cur);
    }
    if ((obj_id_cur==obj_id)&&(display_flag==1)) {
        filename  = (char *)malloc(sizeof(char)*(strlen(filename_cur)+2));
        strcpy(filename,filename_cur);
        *obj_title  = (char *)malloc(sizeof(char)*(strlen(obj_title_cur)+2));
        strcpy(*obj_title,obj_title_cur);
    }
    fclose(fobjs);
    free(obj_title_cur);
    free(filename_cur);

    return filename;
}

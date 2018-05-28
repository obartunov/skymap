#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>

#include "cgic.h"
#include "skyc.h"

double begin_time, readdata_time, grad_time,
    bounds_time, figs_time, stars_time,
    planets_time, galaxies_time, finish_time, process_time ;

void get_the_time(double *whichtime)
{
    struct timeval timev;
    struct timezone timez;
    gettimeofday(&timev,&timez);
    *whichtime=(double)timev.tv_sec+(double)timev.tv_usec/1000000.0;
}

void PlotStars(MapParamsPtr skymap_par,void *map, int *xcoords, int *ycoords, short int *radii, short int *mag, long int NMAX)
{
    long int i;
    char *stmaglab;

    gdImagePtr im;
    CPDFdoc *pdf;
    im=(gdImagePtr)map;
    pdf=(CPDFdoc *)map;

    stmaglab=(char *)malloc(50*sizeof(char));

    for(i=0;i<NMAX;i++)
    {
        if (abs(mag[i])<=skymap_par->st_maglim*10.) {
            sprintf(stmaglab,"%d",abs(mag[i]));
            if (radii[i]>=0)
            {
                if (mag[i]<0)
                    PlotCircle(skymap_par,map,xcoords[i],ycoords[i],radii[i],skymap_par->gifpar->green,42,20);
                else
                    PlotCircle(skymap_par,map,xcoords[i],ycoords[i],radii[i],skymap_par->gifpar->white,42,20);
                if ((skymap_par->print_starsmag==1)&&
                    ((skymap_par->st_mlpr_b-0.05)<=(double)fabs(mag[i]/10.0))&&
                    ((skymap_par->st_mlpr_f+0.05)>=fabs(mag[i]/10.0))&&
                    (xcoords[i]>41)&&(ycoords[i]>19)&&
                    (xcoords[i]<skymap_par->SIZEX-42)&&
                    (ycoords[i]<skymap_par->SIZEY-20))
                {
                    if (skymap_par->pdf!=1)
                        gdImageString(im,skymap_par->gifpar->font,xcoords[i]+radii[i],ycoords[i]-7,stmaglab,skymap_par->gifpar->yellow);
                    else
                        PDF_String(pdf,"Times-Bold","MacRomanEncoding",8.0,
                                   xcoords[i]+radii[i],ycoords[i]-5,stmaglab);
                }
            }
        }
    }
}

void canvas_init(MapParamsPtr skymap_par,void *map,int color,int draw_labels)
{
     int chs=1; //mid
     gdImagePtr im;
     CPDFdoc *pdf;
     im=(gdImagePtr)map;
     pdf=(CPDFdoc *)map;


     if (skymap_par->display_grids==1)
     {
         if ((skymap_par->pdf == 1)&&(draw_labels==0)) {
             cpdf_newpath(pdf);
         }
         chs=CoordType(skymap_par);
         //if ((fabs(skymap_par->Dec_center)<2.*skymap_par->zoomk)||(fabs(skymap_par->Dec_center)<=20))
         //    chs=0;    //equ
         //if ((90.0-fabs(skymap_par->Dec_center))<=(30.000/skymap_par->zoomk))
         //    chs=2; //pol

         if (chs==1) mid_init(skymap_par, map, color, draw_labels);
         if (chs==0) equ_init(skymap_par, map, color, draw_labels);
         if (chs==2) pol_init(skymap_par, map, color, draw_labels);

         if ((skymap_par->pdf == 1)&&(draw_labels==0)) {
             cpdf_moveto(pdf,0,0);
             cpdf_closepath(pdf);
             cpdf_stroke(pdf);
         }
     }
}

double auto_grid_cell_set(MapParamsPtr skymap_par,int gridmode)
{
    double ngrids;
    ngrids=1.0;
    if (((90.0-fabs(skymap_par->Dec_center))<=(30.000/skymap_par->zoomk))&&(gridmode==1)) return(30); //polar
    if (skymap_par->zoomk>=600)
    {
        //ngrids=(double)((int)(1.0/cos(Pi*skymap_par->Dec_center/180.0)))*10.0/skymap_par->zoomk;
        //ngrids=rint(3600.0*pow(2.,(double)(int)(log(10./((skymap_par->zoomk)*cos(Pi*(skymap_par->Dec_center)/180.0)))/0.6931471805599453))/1.5)/3600.0;
        ngrids=pow(2.0,(double)rint(log(60.0*50.0/skymap_par->zoomk)/log(2.0)))/240.0;
    }
    if ((skymap_par->zoomk>=600)&&(gridmode==1)) ngrids*= 5.0/4.0;
    if ((skymap_par->zoomk>=300)&&(skymap_par->zoomk<600))ngrids=(gridmode==2)?1/30.: 0.25/6.0;
    if ((skymap_par->zoomk>=150)&&(skymap_par->zoomk<300))ngrids=(gridmode==2)?1/20.: 0.25/3.0;
    if ((skymap_par->zoomk>=75)&&(skymap_par->zoomk<150))ngrids=(gridmode==2)? 1/12.: 0.125;
    if ((skymap_par->zoomk>=45)&&(skymap_par->zoomk<75)) ngrids=(gridmode==2)? 1/6.0: 0.25;
    if ((skymap_par->zoomk>=30)&&(skymap_par->zoomk<45)) ngrids=(gridmode==2)? 0.25 : 0.5;
    if ((skymap_par->zoomk>=20)&&(skymap_par->zoomk<30)) ngrids=(gridmode==2)? 1/3.0: 0.5;
    if ((skymap_par->zoomk>=15)&&(skymap_par->zoomk<20)) ngrids=(gridmode==2)? 0.5  : 0.75;
    if ((skymap_par->zoomk>=10)&&(skymap_par->zoomk<15)) ngrids=(gridmode==2)? 1.0  : 1.25;
    if ((skymap_par->zoomk>=8)&&(skymap_par->zoomk<10))  ngrids=(gridmode==2)? 1.5  : 2.5;
    if ((skymap_par->zoomk>=6)&&(skymap_par->zoomk<8))   ngrids=(gridmode==2)? 2.0  : 2.5;
    if ((skymap_par->zoomk>=5)&&(skymap_par->zoomk<6))   ngrids=(gridmode==2)? 3.0  : 5.0;
    if ((skymap_par->zoomk>=4)&&(skymap_par->zoomk<5))   ngrids=(gridmode==2)? 4.0  : 5.0;
    if ((skymap_par->zoomk>=3)&&(skymap_par->zoomk<4))   ngrids=(gridmode==2)? 5.0  : 7.5;
    if ((skymap_par->zoomk>=2)&&(skymap_par->zoomk<3))   ngrids=(gridmode==2)? 8.0  : 10.0;
    if ((skymap_par->zoomk>=0.4)&&(skymap_par->zoomk<2)) ngrids=(gridmode==2)? 10.0 : 15.0;
    //if (gridmode==1) ngrids=15.0*ngrids/4.0;
    return ngrids;
}

void Grid_cell_set(MapParamsPtr skymap_par)
{
    if (skymap_par->RA_grid_cellsize <= 0.0) skymap_par->RA_grid_cellsize=auto_grid_cell_set(skymap_par,1);
    if (skymap_par->Dec_grid_cellsize <= 0.0) skymap_par->Dec_grid_cellsize=auto_grid_cell_set(skymap_par,2);
}

void draw_times(MapParamsPtr skymap_par, gdImagePtr im)
{
    char *sss1;

    sss1=(char *)malloc(sizeof(char)*5);

    //sprintf(sss1,"Data : %f sec.",readdata_time-begin_time);
    //gdImageString(im, skymap_par->gifpar->font, 18, 75, sss1, skymap_par->gifpar->white);

    sprintf(sss1,"Prep : %f sec.",grad_time-begin_time);
    gdImageString(im, skymap_par->gifpar->font, 18, 85, sss1, skymap_par->gifpar->white);

//    sprintf(sss1,"Figs : %f sec.",figs_time-bounds_time);
//    gdImageString(im, skymap_par->gifpar->fonts, 18,105, sss1, skymap_par->gifpar->white);

    sprintf(sss1,"Stars: %f sec.",stars_time-grad_time);
    gdImageString(im, skymap_par->gifpar->font, 18,115, sss1, skymap_par->gifpar->white);

    sprintf(sss1,"Bound: %f sec.",bounds_time-stars_time);
    gdImageString(im, skymap_par->gifpar->font, 18, 95, sss1, skymap_par->gifpar->white);


//    sprintf(sss1,"Plan.: %f sec.",planets_time-stars_time);
//    gdImageString(im, skymap_par->gifpar->fonts, 18,125, sss1, skymap_par->gifpar->white);

    sprintf(sss1,"Galaxies: %f sec.",galaxies_time-stars_time);
    gdImageString(im, skymap_par->gifpar->font, 18,135, sss1, skymap_par->gifpar->white);
    get_the_time(&finish_time);
    process_time=finish_time-begin_time;

    sprintf(sss1,"Total: %f sec.",process_time);
    gdImageString(im, skymap_par->gifpar->font, 18, 60, sss1, skymap_par->gifpar->white);
}

void DrawLegend(MapParamsPtr skymap_par, void *map)
{
    int i, r;
    double stmag;
    char c[20];

    gdImagePtr im;
    CPDFdoc *pdf;
    im=(gdImagePtr)map;
    pdf=(CPDFdoc *)map;


    stmag=skymap_par->st_maglim;
    for (i=0;i<=(int)stmag;i++)
    {
        r = comp_radius(10*i,stmag);
        sprintf(c,"%d",i);
        PlotCircle(skymap_par, map, skymap_par->SIZEX+20, 40+i*30, r, skymap_par->gifpar->white, -60, 0);
        if (skymap_par->pdf != 1) {
            gdImageString(im, skymap_par->gifpar->font, skymap_par->SIZEX+45, 35+i*30, c, skymap_par->gifpar->white);
        } else {
            PDF_String(pdf,"Times-Bold","MacRomanEncoding",8.0,
                      skymap_par->SIZEX+45.0,35.0+i*30.0, c);
        }
    }
}

int cgiMain()
{
    gdImagePtr im;
    CPDFdoc *pdf;
    void *map;

    MapParamsPtr skymap_par;

    long int NMAX;
    gal_rc3 *rc3;
    int	u,i,j;
    FILE *ftest;
    cgiFormResultType cgi_r;
    int *xcoords, *ycoords;
    short int *radii;

    double  *a20coord, *tyccoord, *tycb_v, *tyc_pmra, *tyc_pmdec, *tyc_par;
    short int   *a20bmag,*tycmag;
    short int   *a20rmag;
    int	use_a20=0;
    int usepdf=0;
    int draw_labels=0;
    char *bufPDF;
    int pdf_length;
    long int *t_ind;
    long int **a20_ind;
    char **zone_fnames;
    double *conbnd;
    FILE *ftyc;
    int obj_id=-1;

/*#ifdef DEBUG*/
    int str_buf_size=0;
    long int obj_numb=0, obj_add_numb=0;
    plotable_obj *objs_add;
    plotable_obj *objlist;
    char *objfilename, *objfiletmp, *objtitle;
    objfiletmp=(char *)malloc(sizeof(char)*100);
/*#endif*/

    im=NULL;
    pdf=NULL;

    skymap_par = (MapParams *)malloc(sizeof(MapParams));
    skymap_par->gifpar = (GifParamsPtr)malloc(sizeof(GifParams));
    get_the_time(&begin_time);

    cgi_r = cgiFormDouble("a",&skymap_par->RA_center,5.0);
    cgi_r = cgiFormDouble("d",&skymap_par->Dec_center,0.0);
    cgi_r = cgiFormDouble("m",&skymap_par->st_maglim,7.0);
    cgi_r = cgiFormDouble("z",&skymap_par->zoomk,1.0);
    cgi_r = cgiFormInteger("gr",&skymap_par->display_grids,1);
    cgi_r = cgiFormInteger("xs",&skymap_par->SIZEX,800);
    cgi_r = cgiFormInteger("db",&skymap_par->display_constbnd,0);
    cgi_r = cgiFormInteger("rc3",&skymap_par->display_galaxies,1);
    cgi_r = cgiFormDouble("galmaxmag",&skymap_par->gal_maglim,15.0);
    cgi_r = cgiFormInteger("supunkgal",&skymap_par->suppress_unkmag_gal,1);
    cgi_r = cgiFormInteger("printsl",&skymap_par->print_starsmag,0);
    cgi_r = cgiFormDouble("slbr",&skymap_par->st_mlpr_b,0.0);
    cgi_r = cgiFormDouble("slf",&skymap_par->st_mlpr_f,13.0);
    cgi_r = cgiFormInteger("prn3",&skymap_par->print_galngc,0);
    cgi_r = cgiFormInteger("pri3",&skymap_par->print_galic,0);
    cgi_r = cgiFormInteger("pru3",&skymap_par->print_galugc,0);
    cgi_r = cgiFormInteger("prp3",&skymap_par->print_galpgc,0);
    cgi_r = cgiFormInteger("prsm3",&skymap_par->print_galmag,0);
    cgi_r = cgiFormDouble("xcs",&skymap_par->RA_grid_cellsize,0.0);
    cgi_r = cgiFormDouble("ycs",&skymap_par->Dec_grid_cellsize,0.0);
    cgi_r = cgiFormInteger("pdf",&skymap_par->pdf,0);
    cgi_r = cgiFormInteger("mirrorx",&skymap_par->mirrorx,0);
    cgi_r = cgiFormInteger("mirrory",&skymap_par->mirrory,0);
    cgi_r = cgiFormStringSpaceNeeded("plotlabels",&str_buf_size);
    skymap_par->plotlabels = (char *)malloc(sizeof(char)*(str_buf_size+2));
    cgi_r = cgiFormString("plotlabels",skymap_par->plotlabels,str_buf_size+1);
    cgi_r = cgiFormInteger("objid",&obj_id,-1);

    objlist = get_objs_from_string(skymap_par->plotlabels,&obj_numb);

#ifdef USE_HL
    cgi_r = cgiFormInteger("hl",&skymap_par->display_hlobjects,0);

    if ((skymap_par->display_hlobjects==1)&&(HL_connect(skymap_par) != 0))
        skymap_par->display_hlobjects=0;
#endif
    //cgi_r = cgiFormInteger("plot_add",&skymap_par->plot_add_data,0);
    //strcpy(objfilename,"/astrodata/sky_map_pro/adddata.txt");
    //objfilename=strcat(objfilename,objfiletmp);

    usepdf=skymap_par->pdf;

    Grid_cell_set(skymap_par);

    if (skymap_par->SIZEX > 4500.0) skymap_par->SIZEX = 3200.0;
    if (usepdf==1) {
        skymap_par->SIZEX= (skymap_par->SIZEX>=2559.0)? 2560.0 : 1280.0;
    }

    if (skymap_par->zoomk<0.5) skymap_par->zoomk=1;
    skymap_par->SIZEY=3*skymap_par->SIZEX/4;
    
    if (skymap_par->st_maglim>25.1) skymap_par->st_maglim=20.1;
    if ((skymap_par->st_maglim>=10.5)&&(skymap_par->zoomk>=5)) use_a20=1;
    skymap_par->imzoom=(double)skymap_par->SIZEX/1280.0;

    if (usepdf!=1) {
        im=PrepareImageGif(skymap_par);
        map = (void *)im;
    } else {
        pdf=PrepareImagePDF(skymap_par);
        cpdf_gsave(pdf);
        cpdf_newpath(pdf);
        cpdf_rect(pdf,41,19,skymap_par->SIZEX-84,skymap_par->SIZEY-40);
        cpdf_clip(pdf);
        cpdf_newpath(pdf);
        map = (void *)pdf;
    }
    if (usepdf==1) cpdf_setdash(pdf,"[4 4] 0");
    canvas_init(skymap_par,map,skymap_par->gifpar->darkgreen,draw_labels);
    if (usepdf==1) cpdf_nodash(pdf);
    get_the_time(&grad_time);

    if (usepdf==1) {
        cpdf_setgrayStroke(pdf,1.0);
        cpdf_setgrayFill(pdf, 0.0);
        cpdf_setlinewidth(pdf, 0.1);
    }
    if (use_a20==1)
    {
        if((ftest=fopen(A20_TEST,"r"))==NULL)
        {
            use_a20=0;
            gdImageString(im,skymap_par->gifpar->font,45,25,
                          "Warning !!! USNO A2.0 catalogue not found. Only TYCHOE catalogue is used",
                          skymap_par->gifpar->white);
        } else fclose(ftest);
    }
    if (use_a20==1)
    {
        a20_ind=read_index_a20(&zone_fnames);
        for(i=0;i<96;i++)
            for(j=0;j<24;j++)
            {
                if (zone_check_a20(skymap_par,i,j)==1)
                {
                    NMAX=read_zone_a20(skymap_par,a20_ind,zone_fnames,i,j,&a20coord,&a20bmag,&a20rmag);
                    if (NMAX == -1) soft_exit(skymap_par,map,"Error reading USNO A20 data. Cannot continue");
                    CompCoordinates(skymap_par,a20coord,a20bmag,&xcoords,&ycoords,&radii,NMAX,1);
                    PlotStars(skymap_par,map,xcoords,ycoords,radii,a20bmag,NMAX);
                    free(a20coord);  a20coord=NULL;
                    free(a20bmag);   a20bmag=NULL;
                    free(a20rmag);   a20rmag=NULL;
                    free(xcoords);   xcoords=NULL;
                    free(ycoords);   ycoords=NULL;
                    free(radii);     radii=NULL;
                }
            }
    }
    t_ind=read_index_tycho();
    if (((ftyc=fopen(TYCHO_DATA,"rb"))==NULL)||(t_ind==NULL))
        soft_exit(skymap_par,map,"Error reading TYCHO data. Cannot continue");
    for(u=1;u<=864;u++)
    {
        if (zone_check_tycho(skymap_par,u)==1)
        {
            NMAX=read_zone_tycho(skymap_par,t_ind,ftyc,u,&tyccoord,&tycmag,&tycb_v,&tyc_pmra,&tyc_pmdec,&tyc_par);
            CompCoordinates(skymap_par,tyccoord,tycmag,&xcoords,&ycoords,&radii,NMAX,1);
            PlotStars(skymap_par,map,xcoords,ycoords,radii,tycmag,NMAX);
            free(tyccoord);  tyccoord=NULL;
            free(tycmag);    tycmag=NULL;
            free(tycb_v);    tycb_v=NULL;
            free(tyc_pmra);  tyc_pmra=NULL;
            free(tyc_pmdec); tyc_pmdec=NULL;
            free(tyc_par);   tyc_par=NULL;
            free(xcoords);   xcoords=NULL;
            free(ycoords);   ycoords=NULL;
            free(radii);     radii=NULL;
        }
    }
    if (usepdf==1) {
        cpdf_setgrayStroke(pdf,0.0);
        cpdf_setgrayFill(pdf, 0.0);    /* Black */
        cpdf_setlinewidth(pdf, 0.5);
    }
    fclose(ftyc);
    get_the_time(&stars_time);

    if (skymap_par->display_galaxies==1) {
        rc3=read_rc3data();
        if (rc3 != NULL) CompAndPlotGalaxies(skymap_par,map,rc3,23023);
        free(rc3);
    }
#ifdef USE_HL
    if (skymap_par->display_hlobjects==1) {
        gal_rc3 *hl_gal;
        int ngal=hl_fetch(skymap_par,&hl_gal);
        /*fprintf(stderr,"%d objects returned by hl_fetch\n",ngal);*/
        if (ngal > 0)
            CompAndPlotGalaxies(skymap_par,map,hl_gal,(long int)ngal);
        /*fprintf(stderr,"%d objects plotted\n",ngal);*/
        HL_disconnect(skymap_par);
        skymap_par->hl_conn=NULL;
    }
#endif
    get_the_time(&galaxies_time);

    if ((obj_numb > 0)&&(objlist!=NULL))
        CompAndPlotObjects(skymap_par, map, objlist, obj_numb);

    objfilename=get_filename((long int)obj_id,&objtitle);
    if (objfilename != NULL) {
        objs_add=read_objstxt(objfilename,&obj_add_numb);
        if ((objs_add != NULL)&&(obj_add_numb > 0))
            CompAndPlotObjects(skymap_par, map, objs_add, obj_add_numb);
    }
    
    if (skymap_par->display_constbnd==1)
    {
	conbnd=NULL;
        conbnd=read_boundaries();
        if (conbnd != NULL) {
            if (usepdf==1) cpdf_setdash(pdf,"[2 2] 0");
            PlotConstBoundaries(skymap_par,map,conbnd);
            if (usepdf==1) cpdf_nodash(pdf);
        }
    }
    
    if (usepdf!=1) {
        gdImageRectangle(im,41,19,skymap_par->SIZEX-42,skymap_par->SIZEY-20,skymap_par->gifpar->green);
    } else {
        cpdf_grestore(pdf);
        cpdf_newpath(pdf);
        PDF_Line(pdf,41,19,skymap_par->SIZEX-42,19);
        PDF_Line(pdf,skymap_par->SIZEX-42,19,skymap_par->SIZEX-42,skymap_par->SIZEY-20);
        PDF_Line(pdf,skymap_par->SIZEX-42,skymap_par->SIZEY-20,41,skymap_par->SIZEY-20);
        PDF_Line(pdf,41,skymap_par->SIZEY-20,41,19);
        cpdf_moveto(pdf,0.,0.);
        cpdf_closepath(pdf);
        cpdf_stroke(pdf);
    }

    draw_labels=1;
    canvas_init(skymap_par,map,skymap_par->gifpar->green,draw_labels);
    if (usepdf==1) {
        cpdf_setgrayStroke(pdf,1.0);
        cpdf_setgrayFill(pdf, 0.0);    /* Black */
        cpdf_setlinewidth(pdf, 0.1);
    }
    DrawLegend(skymap_par,map);
    if (usepdf==1) {
        cpdf_setgrayStroke(pdf,0.0);
        cpdf_setgrayFill(pdf, 0.0);    /* Black */
        cpdf_setlinewidth(pdf, 0.5);
    }
    get_the_time(&bounds_time);
#ifdef DEBUG
    if (usepdf!=1) draw_times(skymap_par,im);
#endif
    if (usepdf!=1) {
        print_gif_header();
        gdImageGif(im,stdout);
        //  gdImageDestroy(im);
    } else {
        cpdf_finalizeAll(pdf);
        bufPDF = cpdf_getBufferForPDF(pdf, &pdf_length);
        print_pdf_header((long)pdf_length);
        fwrite((void *)bufPDF, 1, (size_t)pdf_length, stdout);
        //cpdf_savePDFmemoryStreamToFile(pdf, "/dev/stdout");
        cpdf_close(pdf);
    }
    return 0;
}

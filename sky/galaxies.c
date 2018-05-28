#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "skyc.h"

void PlotGal(MapParamsPtr skymap_par,void *map,int x,int y,double fi,int Asiz,int Bsiz,int color)
{
    gdImagePtr im;
    CPDFdoc *pdf;
    im=(gdImagePtr)map;
    pdf=(CPDFdoc *)map;

    if ((x>-Asiz/2+41)&&(y>-Asiz/2+19)&&(x<(1280*skymap_par->imzoom+Asiz/2)-42)&&(y<(960*skymap_par->imzoom+Asiz/2)-20))
    {
        if (skymap_par->pdf !=1)
            Draw_Ellipse(im,x,y,fi,Asiz,Bsiz,color);
        else {
            Draw_Arc_pdf(pdf,x,y,fi,Asiz/2.0,Bsiz/2.0,1,1);
            Draw_Arc_pdf(pdf,x,y,fi,Asiz/2.0,-Bsiz/2.0,-1,0);

            cpdf_moveto(pdf,0.,0.);
            cpdf_stroke(pdf);
        }
    }
}

void PlotGCl(MapParamsPtr skymap_par,void *map,int x,int y,int siz,int color)
{
    gdImagePtr im;
    CPDFdoc *pdf;
    im=(gdImagePtr)map;
    pdf=(CPDFdoc *)map;

    if ((x>-siz/2+41)&&(y>-siz/2+19)&&(x<(1280*skymap_par->imzoom+siz/2)-42)&&(y<(960*skymap_par->imzoom+siz/2)-20))
    {
        if (skymap_par->pdf !=1) {
            gdImageArc(im,x,y,siz,siz,0,360,color);
            gdImageLine(im,x-siz/2,y,x+siz/2,y,color);
            gdImageLine(im,x,y-siz/2,x,y+siz/2,color);
        } else {
            Draw_Arc_pdf(pdf,x,y,0,siz/2.0,siz/2.0,1,1);
            Draw_Arc_pdf(pdf,x,y,0,siz/2.0,-siz/2.0,-1,0);
            cpdf_moveto(pdf,x-siz/2.0,y);
            cpdf_lineto(pdf,x+siz/2.0,y);
            cpdf_moveto(pdf,x,y-siz/2.0);
            cpdf_lineto(pdf,x,y+siz/2.0);
            cpdf_moveto(pdf,0.,0.);
            cpdf_stroke(pdf);
        }
    }
}

void PlotOCl(MapParamsPtr skymap_par,void *map,int x,int y,int siz,int color)
{
    gdImagePtr im;
    CPDFdoc *pdf;
    im=(gdImagePtr)map;
    pdf=(CPDFdoc *)map;

    if ((x>-siz/2+41)&&(y>-siz/2+19)&&(x<(1280*skymap_par->imzoom+siz/2)-42)&&(y<(960*skymap_par->imzoom+siz/2)-20))
    {
        if (skymap_par->pdf !=1) {
            int i;
            for (i=0; i<6; i++)
                gdImageArc(im,x,y,siz,siz,2*i*30,(2*i+1)*30,color);
        } else {
            /*cpdf_setdash(pdf,"[4 4] 0");*/
            Draw_Arc_pdf(pdf,x,y,0,siz/2.0,siz/2.0,1,1);
            Draw_Arc_pdf(pdf,x,y,0,siz/2.0,-siz/2.0,-1,0);
            /*cpdf_nodash(pdf);*/
            cpdf_moveto(pdf,0.,0.);
            cpdf_stroke(pdf);
        }
    }
}

void PlotPLN(MapParamsPtr skymap_par,void *map,int x,int y,int siz,int color)
{
    gdImagePtr im;
    CPDFdoc *pdf;
    im=(gdImagePtr)map;
    pdf=(CPDFdoc *)map;

    if ((x>-siz/2+41)&&(y>-siz/2+19)&&(x<(1280*skymap_par->imzoom+siz/2)-42)&&(y<(960*skymap_par->imzoom+siz/2)-20))
    {
        if (skymap_par->pdf !=1) {
            gdImageArc(im,x,y,siz,siz,0,360,color);
            gdImageLine(im,x-siz/2,y,x-siz,y,color);
            gdImageLine(im,x,y-siz/2,x,y-siz,color);
            gdImageLine(im,x+siz/2,y,x+siz,y,color);
            gdImageLine(im,x,y+siz/2,x,y+siz,color);
        } else {
            Draw_Arc_pdf(pdf,x,y,0,siz/2.0,siz/2.0,1,1);
            Draw_Arc_pdf(pdf,x,y,0,siz/2.0,-siz/2.0,-1,0);
            cpdf_moveto(pdf,x-siz,y);
            cpdf_lineto(pdf,x-siz/2.0,y);
            cpdf_moveto(pdf,x+siz,y);
            cpdf_lineto(pdf,x+siz/2.0,y);
            cpdf_moveto(pdf,x,y-siz);
            cpdf_lineto(pdf,x,y-siz/2.0);
            cpdf_moveto(pdf,x,y+siz);
            cpdf_lineto(pdf,x,y+siz/2.0);
            cpdf_moveto(pdf,0.,0.);
            cpdf_stroke(pdf);
        }
    }
}

void PlotDN(MapParamsPtr skymap_par,void *map,int x,int y,int siz,int color)
{
    gdImagePtr im;
    CPDFdoc *pdf;
    im=(gdImagePtr)map;
    pdf=(CPDFdoc *)map;

    if ((x>-siz/2+41)&&(y>-siz/2+19)&&(x<(1280*skymap_par->imzoom+siz/2)-42)&&(y<(960*skymap_par->imzoom+siz/2)-20))
    {
        if (skymap_par->pdf !=1) {
            gdImageLine(im,x-siz/2,y-siz/2,x+siz/2,y-siz/2,color);
            gdImageLine(im,x+siz/2,y-siz/2,x+siz/2,y+siz/2,color);
            gdImageLine(im,x+siz/2,y+siz/2,x-siz/2,y+siz/2,color);
            gdImageLine(im,x-siz/2,y+siz/2,x-siz/2,y-siz/2,color);
        } else {
            cpdf_moveto(pdf,x-siz/2.0,y-siz/2.0);
            cpdf_lineto(pdf,x+siz/2.0,y-siz/2.0);
            cpdf_lineto(pdf,x+siz/2.0,y+siz/2.0);
            cpdf_lineto(pdf,x-siz/2.0,y+siz/2.0);
            cpdf_lineto(pdf,x-siz/2.0,y-siz/2.0);
            cpdf_moveto(pdf,0.,0.);
            cpdf_stroke(pdf);
        }
    }
}

void PlotCross(MapParamsPtr skymap_par,void *map,int x,int y,int siz,int color)
{
    gdImagePtr im;
    CPDFdoc *pdf;
    im=(gdImagePtr)map;
    pdf=(CPDFdoc *)map;

    if ((x>-siz/2+41)&&(y>-siz/2+19)&&(x<(1280*skymap_par->imzoom+siz/2)-42)&&(y<(960*skymap_par->imzoom+siz/2)-20))
    {
        if (skymap_par->pdf !=1) {
            gdImageLine(im,x-siz/2,y-siz/2,x+siz/2,y+siz/2,color);
            gdImageLine(im,x-siz/2,y+siz/2,x+siz/2,y-siz/2,color);
        } else {
            cpdf_moveto(pdf,x-siz/2.0,y-siz/2.0);
            cpdf_lineto(pdf,x+siz/2.0,y+siz/2.0);
            cpdf_moveto(pdf,x-siz/2.0,y+siz/2.0);
            cpdf_lineto(pdf,x+siz/2.0,y-siz/2.0);
            cpdf_moveto(pdf,0.,0.);
            cpdf_stroke(pdf);
        }
    }
}

void CompAndPlotGalaxies(MapParamsPtr skymap_par, void *map, gal_rc3 *rc3, long int gal_numb)
{
    int   chs;
    long int i;
    double Astek,Bstek;
    double fitek;
    char *ngc3lab;
    char *ic3lab;
    char *ugc3lab;
    char *pgc3lab;
    char *mag3lab;
    starxy startek;

    gdImagePtr im;
    CPDFdoc *pdf;
    im=(gdImagePtr)map;
    pdf=(CPDFdoc *)map;

    fitek=0;
    chs=CoordType(skymap_par);
    //if ((fabs(skymap_par->Dec_center)<2.*skymap_par->zoomk)||(fabs(skymap_par->Dec_center)<=20)) chs=0;
    //if ((90.0-fabs(skymap_par->Dec_center))<=(30.000/skymap_par->zoomk)) chs=2;
    ngc3lab=(char *)malloc(31*sizeof(char));
    ic3lab =(char *)malloc(31*sizeof(char));
    ugc3lab=(char *)malloc(31*sizeof(char));
    pgc3lab=(char *)malloc(31*sizeof(char));
    mag3lab=(char *)malloc(5*sizeof(char));

    for(i=1;i<=gal_numb;i++)
    {
        if (rc3[i-1].mag<skymap_par->gal_maglim) {
            ngc3lab=strncpy(ngc3lab, rc3[i-1].p_name,30);
            ic3lab=strncpy(ic3lab, rc3[i-1].name2,30);
            ugc3lab=strncpy(ugc3lab, rc3[i-1].name3,30);
            pgc3lab=strncpy(pgc3lab, rc3[i-1].name4,30);
            if (rc3[i-1].mag<0.01)
            {
                sprintf(mag3lab,"???");
            } else
                sprintf(mag3lab,"%4.1f",rc3[i-1].mag);
            Astek=rc3[i-1].Asize*skymap_par->zoomk*skymap_par->SIZEY/(50.0*60.0);
            Bstek=rc3[i-1].Bsize*skymap_par->zoomk*skymap_par->SIZEY/(50.0*60.0);
            switch(chs)
            {
            case 0:
                fitek=rc3[i-1].fi;
                startek=CompEquCoord(skymap_par, rc3[i-1].ra, rc3[i-1].dec, 10);
                break;
            case 1:
                startek=CompMidCoord(skymap_par, rc3[i-1].ra, rc3[i-1].dec, 10);
                fitek=rc3[i-1].fi-Pi*(skymap_par->RA_center-rc3[i-1].ra)*15.0*sin(-Pi*skymap_par->Dec_center/180.0)/180.0;
                if (fitek>=Pi) fitek-=Pi;
                if (fitek<0)   fitek+=Pi;
                break;
            case 2:
                startek=CompPolCoord(skymap_par, rc3[i-1].ra, rc3[i-1].dec, 10);
                fitek=rc3[i-1].fi-Pi*(skymap_par->RA_center-rc3[i-1].ra)*15.0/180.0;
                if (fitek>=Pi) fitek-=Pi;
                if (fitek<0) 	fitek+=Pi;
                break;
			default:
				fprintf(stderr,"Unknown case %d in galaxies.c\n", chs);
				exit(1);
            }
            if (skymap_par->mirrorx==1) fitek*=-1.0;
            if (skymap_par->mirrory==1) fitek=Pi-fitek;
            if (Astek<=8)
            {
                Astek=8;
                Bstek=5;
                fitek=Pi/2;
            }
            //			printf("%d %f %f %f %f %f\n",i,xtek,ytek,fitek,Astek,Bstek);
            if ((skymap_par->suppress_unkmag_gal==1)&&(rc3[i-1].mag<0.01))
                rc3[i-1].mag=skymap_par->gal_maglim+1;
            if ((rc3[i-1].mag<skymap_par->gal_maglim)&&
                (startek.x>19)&&(startek.y>41)&&
                (startek.x<skymap_par->SIZEX-42)&&(startek.y<skymap_par->SIZEY-20))
            {
                int objtype=rc3[i-1].type_obj;
                switch(objtype){
                case 0: /* ipim=0  => galaxy                          G */
                    PlotGal(skymap_par,map,startek.x,startek.y,fitek,Astek,Bstek,skymap_par->gifpar->blue);
                    break;
                case 1: /* ipim=1  => objet extragalactique non ident u */
                    PlotGal(skymap_par,map,startek.x,startek.y,fitek,Astek,Bstek,skymap_par->gifpar->green);
                    break;
                case 2: /* ipim=2  => objet extragalactique probable  g */
                    PlotGal(skymap_par,map,startek.x,startek.y,fitek,Astek,Bstek,skymap_par->gifpar->red);
                    break;
                case 5: /* ipim=5  => star cluster                    c */
                    PlotOCl(skymap_par,map,startek.x,startek.y,Astek,skymap_par->gifpar->yellow);
                    break;
                case 6: /* ipim=6  => SN remnant                      r */
                    PlotPLN(skymap_par,map,startek.x,startek.y,Astek,skymap_par->gifpar->green);
                    break;
                case 7: /* ipim=7  => HII region                      h */
                    PlotDN(skymap_par,map,startek.x,startek.y,Astek,skymap_par->gifpar->red);
                    break;
                case 8: /* ipim=8  => Nebuleuse gazeuse               n */
                    PlotDN(skymap_par,map,startek.x,startek.y,Astek,skymap_par->gifpar->green);
                    break;
                case 9: /* ipim=9  => Nebuleuse planetaire            p */
                    PlotPLN(skymap_par,map,startek.x,startek.y,Astek,skymap_par->gifpar->red);
                    break;
                case 10:/* ipim=10 => Etoile                          S */
                    PlotCross(skymap_par,map,startek.x,startek.y,1,skymap_par->gifpar->yellow);
                    break;
                case 11:/* ipim=11 => Objet non-identifie             ? */
                    break;
                case 12:/* ipim=12 => artefact                        ! */
                    break;
                case 13:/* ipim=13 => Supernova                       s */
                    PlotCross(skymap_par,map,startek.x,startek.y,2,skymap_par->gifpar->red);
                    break;
                case 14:/* ipim=14 => Multiple                        M */
                    PlotGal(skymap_par,map,startek.x,startek.y,fitek,Astek,Bstek,skymap_par->gifpar->darkgreen);
                    break;
                case 15:/* ipim=15 => Radiosource                     R */
                    PlotCross(skymap_par,map,startek.x,startek.y,2,skymap_par->gifpar->green);
                    break;
                case 55:/* 55=>Globular Cluster, my own designation */
                    PlotGCl(skymap_par,map,startek.x,startek.y,Astek,skymap_par->gifpar->green);
                    break;
                }

                if ((skymap_par->print_galngc==1)&&((rc3[i-1].p_name)[0]!=0))
                {
                    if (skymap_par->pdf != 1) {
                        gdImageLine(im,startek.x,startek.y,startek.x+10,startek.y+7,skymap_par->gifpar->green);
                        gdImageString(im,skymap_par->gifpar->font,startek.x+10,startek.y+2,ngc3lab,skymap_par->gifpar->yellow);
                    } else {
                        PDF_String(pdf,"Courier","MacRomanEncoding",8.0,
                                   startek.x+10,startek.y+2,ngc3lab);
                    }
                }
                if ((skymap_par->print_galic==1)&&((rc3[i-1].name2)[0]!=0))
                {
                    if (skymap_par->pdf != 1) {
                        gdImageLine(im,startek.x,startek.y,startek.x-5,startek.y+10,skymap_par->gifpar->green);
                        gdImageString(im,skymap_par->gifpar->font,startek.x-15,startek.y+10,ic3lab,skymap_par->gifpar->yellow);
                    } else {
                        //PDF_Line(pdf,startek.x,startek.y,startek.x-5,startek.y+10);
                        PDF_String(pdf,"Courier","MacRomanEncoding",8.0,
                                   startek.x-15,startek.y+10,ic3lab);
                    }
                }
                if ((skymap_par->print_galugc==1)&&((rc3[i-1].name2)[0]!=0))
                {
                    if (skymap_par->pdf != 1) {
                        gdImageLine(im,startek.x,startek.y,startek.x+10,startek.y-7,skymap_par->gifpar->green);
                        gdImageString(im,skymap_par->gifpar->font,startek.x+10,startek.y-12,ugc3lab,skymap_par->gifpar->yellow);
                    } else {
                        PDF_String(pdf,"Courier","MacRomanEncoding",8.0,
                                   startek.x+10,startek.y-12,ugc3lab);
                    }
                }
                if ((skymap_par->print_galpgc==1)&&((rc3[i-1].name2)[0]!=0))
                {
                    if (skymap_par->pdf != 1) {
                        gdImageLine(im,startek.x,startek.y,startek.x-5,startek.y-12,skymap_par->gifpar->green);
                        gdImageString(im,skymap_par->gifpar->font,startek.x-15,startek.y-22,pgc3lab,skymap_par->gifpar->yellow);
                    } else {
                        PDF_String(pdf,"Courier","MacRomanEncoding",8.0,
                                   startek.x-15,startek.y-22,pgc3lab);
                    }
                }
                if (skymap_par->print_galmag==1)
                {
                    if (skymap_par->pdf != 1) {
                        gdImageLine(im,startek.x,startek.y,startek.x-10,startek.y,skymap_par->gifpar->green);
                        gdImageString(im,skymap_par->gifpar->font,startek.x-26,startek.y-5,mag3lab,skymap_par->gifpar->yellow);
                    } else {
                        PDF_String(pdf,"Courier","MacRomanEncoding",8.0,
                                   startek.x-32,startek.y-5,mag3lab);
                    }
                }
            }
        }
    }
}

/*gal_rc3 * read_rc3data()
{
    gal_rc3 *rc3;
    FILE *frc3;
    unsigned char rc3rec[20];
    int i, rc3r;

    rc3=(gal_rc3 *)malloc(23023*sizeof(gal_rc3));
    if((frc3=fopen("/astrodata/sky_map_pro/dat/rc3.bin","rb"))==NULL) return NULL;
    for(i=1;i<=23023;i++)
      {
      rc3r=fread(rc3rec,sizeof(unsigned char),20,frc3);
        rc3[i-1].ra=24.000000*((double)rc3rec[0]+256*(double)rc3rec[1])/65536.00000;
           if ((rc3rec[2]+256*rc3rec[3]) > 32767)
             rc3[i-1].dec = 90.0*((rc3rec[2]/32400.0+256.0/32400.0*rc3rec[3])-65536.0/32400.0);
           else rc3[i-1].dec = 90.0*((rc3rec[2]/32400.0+256.0/32400.0*rc3rec[3]));

           rc3[i-1].mag=rc3rec[4]/10.0;
           rc3[i-1].type_obj=5;
           rc3[i-1].Asize=(rc3rec[15]+256.0*rc3rec[16])/60.0;
           rc3[i-1].Bsize=(rc3rec[17]+256.0*rc3rec[18])/60.0;
           rc3[i-1].fi=(Pi*rc3rec[5]/180.0);
           rc3[i-1].ngcnum =rc3rec[6]+rc3rec[7]*256;
           rc3[i-1].icnum  =rc3rec[8]+rc3rec[9]*256;
           rc3[i-1].ugcnum =rc3rec[10]+rc3rec[11]*256;
           rc3[i-1].pgcnum =rc3rec[12]+rc3rec[13]*256+rc3rec[14]*65536;
    }

    fclose(frc3);
    return rc3;
}*/

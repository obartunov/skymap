#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "skyc.h"

void PlotObject(MapParamsPtr skymap_par,void *map,int x,int y,plotable_obj obj,int color)
{
    int symtype;
    double symsize,symsize_real;
    gdImagePtr im;
    CPDFdoc *pdf;
    im=(gdImagePtr)map;
    pdf=(CPDFdoc *)map;

    symtype=(obj.symtype >= 11)? 10 : obj.symtype;
    if ((x>41)&&(y>19)&&(x<(1280*skymap_par->imzoom)-42)&&(y<(960*skymap_par->imzoom)-20))
    {
        symsize_real=(obj.objsize*(skymap_par->SIZEY*skymap_par->zoomk/3000.0));
        symsize = ((symsize_real>=obj.minsymsize)||(symsize_real<=0.0))? symsize_real : obj.minsymsize;

        if (skymap_par->pdf !=1)
            plot_marker(im,x,y,symtype,symsize,color);
        else {
            cpdf_marker(pdf,x,y,symtype,symsize/2.0);
        }
    }
}

void CompAndPlotObjects(MapParamsPtr skymap_par, void *map, plotable_obj *objlist, long int obj_numb)
{
    int   chs;
    long int i;
    starxy starcur;
    char *objlabel;
    long int xprev=0,yprev=0;
    //double symsize=0.0, symsize_real=0.0;

    gdImagePtr im;
    CPDFdoc *pdf;
    im=(gdImagePtr)map;
    pdf=(CPDFdoc *)map;

    chs=CoordType(skymap_par);
    objlabel=(char *)malloc(255*sizeof(char));

    if (skymap_par->pdf==1) {
        cpdf_setgrayStroke(pdf,0.0);
        cpdf_setgrayFill(pdf,1.0);
    }

    for(i=1;i<=obj_numb;i++){
        objlabel=strcpy(objlabel, objlist[i-1].designation);
        //fprintf(stderr,"Object %d designation: %s\n",i,objlabel);
        //fprintf(stderr,"Object %d RA:  %f\n",i,objlist[i-1].ra);
        //fprintf(stderr,"Object %d Dec: %f\n",i,objlist[i-1].dec);

        switch(chs){
        case 0:
            starcur=CompEquCoord(skymap_par, objlist[i-1].ra, objlist[i-1].dec, 10);
            break;
        case 1:
            starcur=CompMidCoord(skymap_par, objlist[i-1].ra, objlist[i-1].dec, 10);
            break;
        case 2:
            starcur=CompPolCoord(skymap_par, objlist[i-1].ra, objlist[i-1].dec, 10);
            break;
		default: 
			fprintf(stderr,"Unknown case in CompAndPlotObjects %d\n", chs);
			exit(1);
        }
        if ((starcur.x>19)&&(starcur.y>41)&&
            (starcur.x<skymap_par->SIZEX-42)&&(starcur.y<skymap_par->SIZEY-20))
        {
            /*plot the object*/
            if ((objlist[i-1].symtype >= 0)&&(objlist[i-1].objsize >= 0.0))
                PlotObject(skymap_par,map,starcur.x,starcur.y,objlist[i-1],skymap_par->gifpar->yellow);

            /*plot connection lines if needed*/

            if ((i>1)&&(objlist[i-1].line_to==1)) {
                if (skymap_par->pdf != 1) {
                    gdImageLine(im,xprev,yprev,starcur.x,starcur.y,skymap_par->gifpar->yellow);
                } else {
                    cpdf_newpath(pdf);
                    PDF_Line(pdf,xprev,yprev,starcur.x,starcur.y);
                    cpdf_closepath(pdf);
                    cpdf_stroke(pdf);
                }
            }

            if ((objlist[i-1].designation)[0]!=0)
            {
                if (skymap_par->pdf != 1) {
                    gdImageLine(im,starcur.x,starcur.y,starcur.x+10,starcur.y,skymap_par->gifpar->green);
                    gdImageString(im,skymap_par->gifpar->font,starcur.x+10,starcur.y-5,objlabel,skymap_par->gifpar->yellow);
                } else {
                    cpdf_setgrayFill(pdf,0.0);
                    PDF_String(pdf,"Courier","MacRomanEncoding",8.0,
                               starcur.x+10,starcur.y-5,objlabel);
                    cpdf_setgrayFill(pdf,1.0);
                }
            }
        }
        xprev=starcur.x;
        yprev=starcur.y;
    }
}

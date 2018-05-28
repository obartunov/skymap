#include <stdlib.h>
#include <math.h>
#include "skyc.h"

gdImagePtr PrepareImageGif(MapParamsPtr skymap_par)
{
    gdImagePtr im;
    im = gdImageCreate(skymap_par->SIZEX+60, skymap_par->SIZEY);

    skymap_par->gifpar->font=gdFontSmall;
    skymap_par->gifpar->black	    = gdImageColorAllocate(im, 0, 0, 0);
    skymap_par->gifpar->white	    = gdImageColorAllocate(im, 255, 255, 255);
    skymap_par->gifpar->yellow          = gdImageColorAllocate(im, 255, 255, 0);
    skymap_par->gifpar->red	            = gdImageColorAllocate(im, 255, 0, 0);
    skymap_par->gifpar->green	    = gdImageColorAllocate(im, 0, 255, 0);
    skymap_par->gifpar->blue	    = gdImageColorAllocate(im, 0, 0, 255);
    skymap_par->gifpar->darkgreen       = gdImageColorAllocate(im, 0, 128, 0);

    gdImageColorExact(im, 0,0,0);

    gdImageLine(im,skymap_par->SIZEX/2-10,skymap_par->SIZEY/2,skymap_par->SIZEX/2+10,skymap_par->SIZEY/2,skymap_par->gifpar->yellow);
    gdImageLine(im,skymap_par->SIZEX/2,skymap_par->SIZEY/2-10,skymap_par->SIZEX/2,skymap_par->SIZEY/2+10,skymap_par->gifpar->yellow);
    return im;
}

CPDFdoc *PrepareImagePDF(MapParamsPtr skymap_par)
{
    CPDFdoc *pdf;
    char *sss;
    int alh_p,alm_p,decd_p,decm_p,fdeg1,fdeg2;
    double als_p,decs_p,absdec,fmin1,fmin2;
    double URLshift=0.0;

    CPDFplotDomain *myDomain, *oldDomain;
    char degchar=161;

    alh_p=(int)skymap_par->RA_center;
    alm_p=abs(60.*(skymap_par->RA_center-alh_p));
    als_p=fabs(60.*((skymap_par->RA_center-(double)alh_p)*60.0-(double)alm_p));

    absdec=fabs(skymap_par->Dec_center);
    decd_p=(int)absdec;
    decm_p=abs(60.*(absdec-(double)decd_p));
    decs_p=fabs(60.*((absdec-(double)decd_p)*60.0-(double)decm_p));

    fdeg1=(int)(200.0/(0.9999999999*skymap_par->zoomk)/3.0);
    fmin1=60.0*(200.0/(0.9999999999*skymap_par->zoomk)/3.0 - (double)fdeg1);

    fdeg2=(int)(50.0/(0.9999999999*skymap_par->zoomk));
    fmin2=60.0*(50.0/(0.9999999999*skymap_par->zoomk) - (double)fdeg2);

    sss=(char *)malloc(sizeof(char)*255);
    pdf=cpdf_open(0, NULL);
    cpdf_enableCompression(pdf, YES);
    cpdf_init(pdf);
    cpdf_setDefaultDomainUnit(pdf,0.12);
    //cpdf_pageInit(pdf, 1, LANDSCAPE, A4, A4);
    if (skymap_par->SIZEX <=1280.01)
        cpdf_pageInit(pdf, 1, LANDSCAPE, A4, A4);
    else
        cpdf_pageInit(pdf, 1, LANDSCAPE, "0 0 1190 1684", "0 0 1190 1684"); //A2
    cpdf_rotate(pdf,-90.0);

    if (skymap_par->SIZEX <=1280.01)
        myDomain = cpdf_createPlotDomain(-10.5*inch,1.0*inch,9.0*inch,6.75*inch,
                                         0.,1280.,960.,0.,LINEAR,LINEAR,0);
    else
    {
        myDomain = cpdf_createPlotDomain(-21.0*inch,1.0*inch,18.0*inch,13.5*inch,
                                         0.,skymap_par->SIZEX,skymap_par->SIZEY,
                                         0.,LINEAR,LINEAR,0); //A2
        URLshift = 1.4;
    }
    oldDomain = cpdf_setPlotDomain(pdf, myDomain);
    cpdf_setgrayStroke(pdf,0.0);

    cpdf_setgrayFill(pdf, 0.0);    /* Black */
    cpdf_setlinewidth(pdf,0.5);

    cpdf_beginText(pdf, 0);
    cpdf_setFont(pdf, "Times-Roman", "MacRomanEncoding", 14.0);
    if (skymap_par->Dec_center > 0)
        sprintf(sss,"R.A.: %02dh%02dm%05.2fs, Dec.: +%02d%c%02d'%04.1f\", Field Size: %02d%c%05.2f' x %02d%c%05.2f'",
                alh_p,alm_p,als_p,decd_p,degchar,decm_p,decs_p,fdeg1,degchar,fmin1,fdeg2,degchar,fmin2);
    else
        sprintf(sss,"R.A.: %02dh%02dm%05.2fs, Dec.: -%02d%c%02d'%04.1f\", Field Size: %02d%c%05.2f' x %02d%c%05.2f'",
                alh_p,alm_p,als_p,decd_p,degchar,decm_p,decs_p,fdeg1,degchar,fmin1,fdeg2,degchar,fmin2);
    cpdf_text(pdf, 100, -10.0, 0.0, sss);

    cpdf_setFont(pdf, "Times-Roman", "MacRomanEncoding", 12.0);
    cpdf_text(pdf, skymap_par->SIZEX-640,skymap_par->SIZEY+25, 0.0, "Map produced by SkyMap Online. http://www.astronet.ru/");
    cpdf_endText(pdf);
    cpdf_rawSetActionURL(pdf,0.75*inch,(2.0+URLshift)*inch,(0.75+0.25)*inch,(2+URLshift+4.1)*inch,"http://www.astronet.ru/",NULL);

    cpdf_newpath(pdf); // center cross
    PDF_Line(pdf,skymap_par->SIZEX/2-10,skymap_par->SIZEY/2,skymap_par->SIZEX/2+10,skymap_par->SIZEY/2);
    PDF_Line(pdf,skymap_par->SIZEX/2,skymap_par->SIZEY/2-10,skymap_par->SIZEX/2,skymap_par->SIZEY/2+10);
    cpdf_stroke(pdf);
    cpdf_closepath(pdf);
    return pdf;
}

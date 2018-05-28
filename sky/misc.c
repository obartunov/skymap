#include <stdio.h>
#include "skyc.h"

void PDF_String(CPDFdoc *pdf, char *fontfam, char *fontenc,
               double fontsize, double x, double y, char *text)
{
    cpdf_beginText(pdf,0);
    cpdf_setFont(pdf,fontfam,fontenc,fontsize);
    cpdf_text(pdf,x,y+1.5*fontsize,0.0,text);
    cpdf_endText(pdf);
}

void print_gif_header()
{
    printf("Content-type: image/gif\n");
    printf("Pragma: no-cache\n\n");
}

void print_pdf_header(long length)
{
    printf("Content: sky_map.pdf%c",10);
    printf("Content-disposition: inline; filename=\"sky_map.pdf\"%c",10);
    printf("Content-type: application/pdf%c", 10);
    printf("Content-Length: %ld%c%c", length, 10, 10);
}

void soft_exit(MapParamsPtr skymap_par, void *map, char *msg)
{
    int pdf_length;
    char *bufPDF;

    gdImagePtr im;
    CPDFdoc *pdf;
    im=(gdImagePtr)map;
    pdf=(CPDFdoc *)map;

    if (skymap_par->pdf!=1) {
        gdImageString(im,skymap_par->gifpar->font,5,5,msg,
                      skymap_par->gifpar->white);
        print_gif_header();
        gdImageGif(im,stdout);
        //  gdImageDestroy(im);
    } else {
        PDF_String(pdf,"Times-Roman","MacRomanEncoding",15.0
                   ,skymap_par->SIZEX/2.0,skymap_par->SIZEY/2.0,msg);
        cpdf_finalizeAll(pdf);
        bufPDF = cpdf_getBufferForPDF(pdf, &pdf_length);
        print_pdf_header((long)pdf_length);
        fwrite((void *)bufPDF, 1, (size_t)pdf_length, stdout);
        cpdf_close(pdf);
    }
}

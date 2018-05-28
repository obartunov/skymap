#include <stdio.h>
#include "gd.h"

int main()
{
    gdImagePtr im;
    int black, blue, green;
    FILE *fff;

    im=gdImageCreate(18,18);
    black=gdImageColorAllocate(im,0,0,0);
    blue =gdImageColorAllocate(im,0,0,255);
    green=gdImageColorAllocate(im,0,128,0);
    gdImageColorTransparent(im,black);
    gdImageArc(im,17,17,36,36,0,360,blue);
    gdImageArc(im,17,17,34,34,0,360,green);
    gdImageArc(im,17,17,32,32,0,360,blue);
    
    fff=fopen("ne.gif","wb");
    gdImageGif(im,fff);
    fclose(fff);
    gdImageDestroy(im);

    im=gdImageCreate(18,18);
    black=gdImageColorAllocate(im,0,0,0);
    blue =gdImageColorAllocate(im,0,0,255);
    green=gdImageColorAllocate(im,0,128,0);
    gdImageColorTransparent(im,black);
    gdImageArc(im,-1,17,36,36,0,360,blue);
    gdImageArc(im,-1,17,34,34,0,360,green);
    gdImageArc(im,-1,17,32,32,0,360,blue);
    
    fff=fopen("nw.gif","wb");
    gdImageGif(im,fff);
    fclose(fff);
    gdImageDestroy(im);

    im=gdImageCreate(18,18);
    black=gdImageColorAllocate(im,0,0,0);
    blue =gdImageColorAllocate(im,0,0,255);
    green=gdImageColorAllocate(im,0,128,0);
    gdImageColorTransparent(im,black);
    gdImageArc(im,18,0,36,36,0,360,blue);
    gdImageArc(im,18,0,34,34,0,360,green);
    gdImageArc(im,18,0,32,32,0,360,blue);
    
    fff=fopen("se.gif","wb");
    gdImageGif(im,fff);
    fclose(fff);
    gdImageDestroy(im);

    im=gdImageCreate(18,18);
    black=gdImageColorAllocate(im,0,0,0);
    blue =gdImageColorAllocate(im,0,0,255);
    green=gdImageColorAllocate(im,0,128,0);
    gdImageColorTransparent(im,black);
    gdImageArc(im,0,0,36,36,0,360,blue);
    gdImageArc(im,0,0,34,34,0,360,green);
    gdImageArc(im,0,0,32,32,0,360,blue);
    
    fff=fopen("sw.gif","wb");
    gdImageGif(im,fff);
    fclose(fff);
    gdImageDestroy(im);

    return 0;
}

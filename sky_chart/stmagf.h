#include <stdio.h>
#include <math.h>
#define y00 -1.0

typedef struct
    gal{
        double  ra, dec, mag, fi, Asize, Bsize;
        long int        ngcnum, icnum, ugcnum, pgcnum;
        int     type_obj;
    } gal_rc3;

/*typedef struct
    starxy0 {
        long int        x;
        long int        y;
        int             sm;
    } starxy;*/

typedef struct
    starxy0 {
        double          x;
        double          y;
        int             sm;
    } starxy;

typedef struct
    radec0      {
        double  ra;
        double  dec;
    } radec;

typedef struct
    {
    	double x;
    	double y;
    	double z;
    } point3;

void small_circle(gdImagePtr im,int x,int y,int d,int color)
{
    if (d<=1) gdImageSetPixel(im,x,y,color);
    if (d==2) 
    {
        gdImageSetPixel(im,x,y,color);
        gdImageSetPixel(im,x-1,y-1,color);
        gdImageSetPixel(im,x,y-1,color);
        gdImageSetPixel(im,x-1,y,color);
	return;
    }
    if (d==3) 
    {
        gdImageSetPixel(im,x,y,color);
        gdImageSetPixel(im,x-1,y,color);
        gdImageSetPixel(im,x,y+1,color);
        gdImageSetPixel(im,x+1,y,color);
        gdImageSetPixel(im,x,y-1,color);

//        gdImageSetPixel(im,x+1,y+1,color);
//        gdImageSetPixel(im,x-1,y-1,color);
	return;
    }
    if (d==4) 
    {
	gdImageFilledRectangle(im,x-1,y-2,x,y+1,color);
        gdImageSetPixel(im,x-2,y-1,color);
        gdImageSetPixel(im,x-2,y,color);
        gdImageSetPixel(im,x+1,y-1,color);
        gdImageSetPixel(im,x+1,y,color);
	return;
    }
    if (d==5) 
    {
	gdImageFilledRectangle(im,x-1,y-1,x+1,y+1,color);
        gdImageSetPixel(im,x-2,y,color);
        gdImageSetPixel(im,x,y+2,color);
        gdImageSetPixel(im,x+2,y,color);
        gdImageSetPixel(im,x,y-2,color);

//        gdImageSetPixel(im,x-2,y-1,color);
//        gdImageSetPixel(im,x-1,y+2,color);
//        gdImageSetPixel(im,x+2,y+1,color);
//        gdImageSetPixel(im,x+1,y-2,color);
	return;
    }
    if (d==6) 
    {
	gdImageFilledRectangle(im,x-3,y-1,x+2,y,color);
	gdImageFilledRectangle(im,x-1,y-3,x,y-2,color);
	gdImageFilledRectangle(im,x-1,y+1,x,y+2,color);
        
        gdImageSetPixel(im,x-2,y-2,color);
        gdImageSetPixel(im,x+1,y+1,color);
        gdImageSetPixel(im,x-2,y+1,color);
        gdImageSetPixel(im,x+1,y-2,color);
	return;
    }
    if (d==7) 
    {
	gdImageFilledRectangle(im,x-2,y-2,x+2,y+2,color);
        gdImageSetPixel(im,x-3,y,color);
        gdImageSetPixel(im,x,y+3,color);
        gdImageSetPixel(im,x+3,y,color);
        gdImageSetPixel(im,x,y-3,color);
	return;
    }
    if (d==8) 
    {
	gdImageFilledRectangle(im,x-3,y-3,x+2,y+2,color);
	gdImageLine(im,x-4,y-1,x-4,y,color);
	gdImageLine(im,x-1,y-4,x,y-4,color);
	gdImageLine(im,x+3,y-1,x+3,y,color);
	gdImageLine(im,x-1,y+3,x,y+3,color);
	return;
    }
    if (d==9) 
    {
	gdImageFilledRectangle(im,x-3,y-2,x+3,y+2,color);
	gdImageLine(im,x-2,y-3,x+2,y-3,color);
	gdImageLine(im,x-2,y+3,x+2,y+3,color);
        gdImageSetPixel(im,x-4,y,color);
        gdImageSetPixel(im,x,y+4,color);
        gdImageSetPixel(im,x+4,y,color);
        gdImageSetPixel(im,x,y-4,color);
	return;
    }
    if (d==10) 
    {
	gdImageFilledRectangle(im,x-3,y-4,x+2,y+3,color);

	gdImageLine(im,x-4,y-3,x-4,y+2,color);
	gdImageLine(im,x+3,y-3,x+3,y+2,color);

	gdImageLine(im,x-5,y-1,x-5,y,color);
	gdImageLine(im,x-1,y-5,x,y-5,color);
	gdImageLine(im,x+4,y-1,x+4,y,color);
	gdImageLine(im,x-1,y+4,x,y+4,color);
	return;
    }
    if (d==11) 
    {
	gdImageFilledRectangle(im,x-4,y-2,x+4,y+2,color);
	gdImageLine(im,x-3,y-3,x+3,y-3,color);
	gdImageLine(im,x-3,y+3,x+3,y+3,color);
	gdImageLine(im,x-2,y-4,x+2,y-4,color);
	gdImageLine(im,x-2,y+4,x+2,y+4,color);
        gdImageSetPixel(im,x-5,y,color);
        gdImageSetPixel(im,x,y+5,color);
        gdImageSetPixel(im,x+5,y,color);
        gdImageSetPixel(im,x,y-5,color);
	return;
    }
    if (d==12) 
    {
	gdImageFilledRectangle(im,x-4,y-4,x+3,y+3,color);

	gdImageLine(im,x-5,y-3,x-5,y+2,color);
	gdImageLine(im,x-3,y-5,x+2,y-5,color);
	gdImageLine(im,x+4,y-3,x+4,y+2,color);
	gdImageLine(im,x-3,y+4,x+2,y+4,color);

	gdImageLine(im,x-6,y-1,x-6,y,color);
	gdImageLine(im,x-1,y-6,x,y-6,color);
	gdImageLine(im,x+5,y-1,x+5,y,color);
	gdImageLine(im,x-1,y+5,x,y+5,color);
	return;
    }
    if (d==13) 
    {
	gdImageFilledRectangle(im,x-5,y-3,x+5,y+3,color);
	gdImageLine(im,x-4,y-4,x+4,y-4,color);
	gdImageLine(im,x-4,y+4,x+4,y+4,color);
	gdImageLine(im,x-3,y-5,x+3,y-5,color);
	gdImageLine(im,x-3,y+5,x+3,y+5,color);
        gdImageSetPixel(im,x-6,y,color);
        gdImageSetPixel(im,x,y+6,color);
        gdImageSetPixel(im,x+6,y,color);
        gdImageSetPixel(im,x,y-6,color);
	return;
    }
    
}

void print_circle_pdf(CPDFdoc *pdf, float x, float y, float r)
{
	if (r>0) 
	{
//	    cpdf_marker(pdf, x, y, 0, 0.2+r/2.);
	    cpdf_marker(pdf, x, y, 0, 2.0*(0.5+r/2.));
	}
//	cpdf_fillAndStroke(pdf);
}

void print_circle(gdImagePtr im,int x,int y,int r,int color,int xs)
{
    int rad_of_circle;
    if ((x>-5)&&(y>-5)&&(x<(xs+5))&&(y<(xs+5)))
    {
        //gdImageArc(im,x,y,r+1,r+1,0,360,0);
	if (r<=13) small_circle(im,x,y,r,color); else
	{
            gdImageArc(im,x,y,r,r,0,360,color);
            if(r>1)
            {
                for(rad_of_circle=1;rad_of_circle<r;rad_of_circle++) 
                    gdImageArc(im,x,y,rad_of_circle,rad_of_circle,0,360,color);
            }
        }
    }
}

int comp_radius(int sm, double stmg)
{
    int rad_of_star;
//    rad_of_star = 1 + (int)(1.8*(0+stmg-0.1*(double)sm));
    rad_of_star = 1 + (int)(2.0*(0.5+(double)(int)stmg-0.1*sm));
    return(rad_of_star);
}

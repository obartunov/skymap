#include <stdio.h>
#include <math.h>

typedef struct
    gal{
        double  ra, dec, mag, fi, Asize, Bsize;
        long int        ngcnum, icnum, ugcnum, pgcnum;
        int     type_obj;
    } gal_rc3;

typedef struct
    starxy0 {
        long int        x;
        long int        y;
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

void print_circle(gdImagePtr im,int x,int y,int r,int color,int xs)
{
    int rad_of_circle;
    if ((x>-5)&&(y>-5)&&(x<(xs+5))&&(y<(xs+5)))
    {
        gdImageArc(im,x,y,r+1,r+1,0,360,0);
        gdImageArc(im,x,y,r,r,0,360,color);
        if(r>1)
        {
            for(rad_of_circle=1;rad_of_circle<r;rad_of_circle++) gdImageArc(im,x,y,rad_of_circle,rad_of_circle,0,360,color);
        }
    }
}

int comp_radius(int sm, double stmg)
{
    int rad_of_star;
    rad_of_star = 1 + (int)(1.8*(0+stmg-0.1*(double)sm));
    return(rad_of_star);
}

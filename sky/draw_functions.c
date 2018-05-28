#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "skyc.h"

void Draw_Ellipse(gdImagePtr im, double x, double y, double firad, double a, double b, int el_color)
{
    double k,k_1,k_2,sin_k_1,cos_k_1,sin_k_2,cos_k_2;
    double f,sin_f,cos_f;
    double a_step;
    long int n_points;
    f=firad+Pi/2.;
    sin_f=sin(f);
    cos_f=cos(f);
    a=a/2.;
    b=b/2.;
    n_points=12+(a+b)*Pi/2.;
    a_step=6.29/n_points;
    for (k=0;k<=n_points;k++)
    {
        k_1=(k-1.)*a_step;
        k_2=k*a_step;
        sin_k_1=sin(k_1);
        cos_k_1=cos(k_1);
        sin_k_2=sin(k_2);
        cos_k_2=cos(k_2);
        gdImageLine(im,
                    x+(b*sin_k_1*sin_f-a*cos_k_1*cos_f),
                    y+(b*sin_k_1*cos_f+a*cos_k_1*sin_f),
                    x+(b*sin_k_2*sin_f-a*cos_k_2*cos_f),
                    y+(b*sin_k_2*cos_f+a*cos_k_2*sin_f),
                    el_color);
    }
}


void PDF_Line(CPDFdoc *pdf, double x1, double y1, double x2, double y2)
{
    cpdf_moveto(pdf,x1,y1);
    cpdf_lineto(pdf,x2,y2);
}

void Draw_Arc_pdf(CPDFdoc *pdf, double x, double y, double firad,
              double a, double b, int draw_direction, int moveto_arg)
{
    //double k;
    //double f;
    double x_c_arc[7];
    double y_c_arc[7];
    double x_c_arc_rot[7];
    double y_c_arc_rot[7];
    int i;//,j;
    
    a*=2;
    b*=2;
    x_c_arc[0] = 0;
    x_c_arc[1] = -(5./18.)*b;
    x_c_arc[2] = -b/2.;
    x_c_arc[3] = -b/2.;
    x_c_arc[4] = -b/2.;
    x_c_arc[5] = -(5./18.)*b;
    x_c_arc[6] = 0;

    y_c_arc[0] = -a/2.;
    y_c_arc[1] = -a/2.;
    y_c_arc[2] = -(5./18.)*a;
    y_c_arc[3] = 0;
    y_c_arc[4] = (5./18.)*a;
    y_c_arc[5] = a/2.;
    y_c_arc[6] = a/2.;
    for (i=0;i<7;i++)
    {
        x_c_arc_rot[i]=x + cos(firad)*x_c_arc[i]+sin(firad)*y_c_arc[i];
        y_c_arc_rot[i]=y - sin(firad)*x_c_arc[i]+cos(firad)*y_c_arc[i];
    }

    if (moveto_arg > 0)
        cpdf_moveto(pdf,0,0);
    if (draw_direction>=0)
    {
        if (moveto_arg > 0)
            cpdf_moveto(pdf, x_c_arc_rot[0], y_c_arc_rot[0]);
        cpdf_curveto(pdf,
                     x_c_arc_rot[1], y_c_arc_rot[1],
                     x_c_arc_rot[2], y_c_arc_rot[2],
                     x_c_arc_rot[3], y_c_arc_rot[3]);
        cpdf_curveto(pdf,
                     x_c_arc_rot[4], y_c_arc_rot[4],
                     x_c_arc_rot[5], y_c_arc_rot[5],
                     x_c_arc_rot[6], y_c_arc_rot[6]);
        //if (moveto_arg > 0)
        //    cpdf_moveto(pdf, x_c_arc_rot[0], y_c_arc_rot[0]);
    } else
    {
        if (moveto_arg > 0)
            cpdf_moveto(pdf, x_c_arc_rot[6], y_c_arc_rot[6]);
        cpdf_curveto(pdf,
                     x_c_arc_rot[5], y_c_arc_rot[5],
                     x_c_arc_rot[4], y_c_arc_rot[4],
                     x_c_arc_rot[3], y_c_arc_rot[3]);
        cpdf_curveto(pdf,
                     x_c_arc_rot[2], y_c_arc_rot[2],
                     x_c_arc_rot[1], y_c_arc_rot[1],
                     x_c_arc_rot[0], y_c_arc_rot[0]);
        //if (moveto_arg > 0)
        //    cpdf_moveto(pdf, x_c_arc_rot[6], y_c_arc_rot[6]);
    }
}

void gdImageCircleBnd(gdImagePtr im, long int cx, long int cy, 
                      long int w, long int h,
                      long int x1, long int y1, long int x2, long int y2,
                      int color)
{
    int x, y, d;
    x = 0;
    //	y = w;
    y = w/2;
    d = 3-2*w;
    while (x <= y) {
        if ((cx+x>x1)&&(cx+x<x2)&&(cy+y>y1)&&(cy+y<y2))
            gdImageSetPixel(im, cx+x, cy+y, color);
        if ((cx+y>x1)&&(cx+y<x2)&&(cy+x>y1)&&(cy+x<y2))
            gdImageSetPixel(im, cx+y, cy+x, color);

        if ((cx+x>x1)&&(cx+x<x2)&&(cy-y>y1)&&(cy-y<y2))
            gdImageSetPixel(im, cx+x, cy-y, color);
        if ((cx+y>x1)&&(cx+y<x2)&&(cy-x>y1)&&(cy-x<y2))
            gdImageSetPixel(im, cx+y, cy-x, color);

        if ((cx-x>x1)&&(cx-x<x2)&&(cy+y>y1)&&(cy+y<y2))
            gdImageSetPixel(im, cx-x, cy+y, color);
        if ((cx-y>x1)&&(cx-y<x2)&&(cy+x>y1)&&(cy+x<y2))
            gdImageSetPixel(im, cx-y, cy+x, color);

        if ((cx-x>x1)&&(cx-x<x2)&&(cy-y>y1)&&(cy-y<y2))
            gdImageSetPixel(im, cx-x, cy-y, color);
        if ((cx-y>x1)&&(cx-y<x2)&&(cy-x>y1)&&(cy-x<y2))
            gdImageSetPixel(im, cx-y, cy-x, color);

        if (d < 0) {
            d += 4 * x + 6;
        } else {
            d += 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
/*    if (x == y) {
        gdImageSetPixel(im, cx+x, cy+y, color);
        gdImageSetPixel(im, cx+y, cy+x, color);

        gdImageSetPixel(im, cx+x, cy-y, color);
        gdImageSetPixel(im, cx+y, cy-x, color);

        gdImageSetPixel(im, cx-x, cy+y, color);
        gdImageSetPixel(im, cx-y, cy+x, color);

        gdImageSetPixel(im, cx-x, cy-y, color);
        gdImageSetPixel(im, cx-y, cy-x, color);
    }*/
}

void gdImageLineBnd(gdImagePtr im, int x1, int y1, int x2, int y2,
                    int xb1, int yb1, int xb2, int yb2, int color)
{
	int dx, dy, incr1, incr2, d, x, y, xend, yend, xdirflag, ydirflag;
	dx = abs(x2-x1);
	dy = abs(y2-y1);
	if (dy <= dx) {
		d = 2*dy - dx;
		incr1 = 2*dy;
		incr2 = 2 * (dy - dx);
		if (x1 > x2) {
			x = x2;
			y = y2;
			ydirflag = (-1);
			xend = x1;
		} else {
			x = x1;
			y = y1;
			ydirflag = 1;
			xend = x2;
		}
                if ((x>xb1)&&(x<xb2)&&(y>yb1)&&(y<yb2))
                    gdImageSetPixel(im, x, y, color);
		if (((y2 - y1) * ydirflag) > 0) {
			while (x < xend) {
				x++;
				if (d <0) {
					d+=incr1;
				} else {
					y++;
					d+=incr2;
				}
                                if ((x>xb1)&&(x<xb2)&&(y>yb1)&&(y<yb2))
                                    gdImageSetPixel(im, x, y, color);
			}
		} else {
			while (x < xend) {
				x++;
				if (d <0) {
					d+=incr1;
				} else {
					y--;
					d+=incr2;
				}
                                if ((x>xb1)&&(x<xb2)&&(y>yb1)&&(y<yb2))
                                    gdImageSetPixel(im, x, y, color);
			}
		}		
	} else {
		d = 2*dx - dy;
		incr1 = 2*dx;
		incr2 = 2 * (dx - dy);
		if (y1 > y2) {
			y = y2;
			x = x2;
			yend = y1;
			xdirflag = (-1);
		} else {
			y = y1;
			x = x1;
			yend = y2;
			xdirflag = 1;
		}
                if ((x>xb1)&&(x<xb2)&&(y>yb1)&&(y<yb2))
                    gdImageSetPixel(im, x, y, color);
		if (((x2 - x1) * xdirflag) > 0) {
			while (y < yend) {
				y++;
				if (d <0) {
					d+=incr1;
				} else {
					x++;
					d+=incr2;
				}
                                if ((x>xb1)&&(x<xb2)&&(y>yb1)&&(y<yb2))
                                    gdImageSetPixel(im, x, y, color);
                        }
		} else {
			while (y < yend) {
				y++;
				if (d <0) {
					d+=incr1;
				} else {
					x--;
					d+=incr2;
				}
                                if ((x>xb1)&&(x<xb2)&&(y>yb1)&&(y<yb2))
                                    gdImageSetPixel(im, x, y, color);
			}
		}
	}
}
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

void PlotCircle(MapParamsPtr skymap_par, void *map,int x,int y,int r,int color,int xbrd,int ybrd)
{
    int rad_of_circle;
    int xs=skymap_par->SIZEX;

    gdImagePtr im;
    CPDFdoc *pdf;
    im=(gdImagePtr)map;
    pdf=(CPDFdoc *)map;
    
//    if ((x>-6+xbrd)&&(y>-6+ybrd)&&(x<(xs+5-xbrd))&&(y<(xs+5-xbrd)))
    if ((x>xbrd)&&(y>ybrd)&&(x<(xs-xbrd))&&(y<(3*xs/4-ybrd)))
    {
        if (skymap_par->pdf!=1) {
            if (r<=12) {
                small_circle(im,x,y,r,color);
                small_circle(im,x,y,r,color);
            } else if (r==13) {
                gdImageArc(im,x,y,r+1,r+1,0,360,0);
                small_circle(im,x,y,r,color);
            } else
            {
                gdImageArc(im,x,y,r+1,r+1,0,360,0);
                small_circle(im,x,y,13,color);
                for(rad_of_circle=14;rad_of_circle<=r;rad_of_circle++)
                    gdImageArc(im,x,y,rad_of_circle,rad_of_circle,0,360,color);
            }
        } else {
            if (r>0) cpdf_marker(pdf, x, y, 0, 1.0+r/2.);
        }
    }
}

void plot_marker(gdImagePtr im,int x,int y,int mtype, int msize,int color)
{
    switch(mtype){
    case 0:
        gdImageArc(im,x,y,msize,msize,0,360,color);
        break;
    case 1:
        gdImageLine(im,x,y-msize/2,x+msize*0.433013,y+msize/4,color);
        gdImageLine(im,x+msize*0.433013,y+msize/4,x-msize*0.433013,y+msize/4,color);
        gdImageLine(im,x-msize*0.433013,y+msize/4,x,y-msize/2,color);
        break;
    case 2:
        gdImageLine(im,x,y-msize/2,x+msize/3,y,color);
        gdImageLine(im,x+msize/3,y,x,y+msize/2,color);
        gdImageLine(im,x,y+msize/2,x-msize/3,y,color);
        gdImageLine(im,x-msize/3,y,x,y-msize/2,color);
        break;
    case 3:
        gdImageRectangle(im,x-msize/2,y-msize/2,x+msize/2,y+msize/2,color);
        break;
    case 4:
    case 5:
    case 6:
        gdImageLine(im,x,y+msize/2,x+msize*0.433013,y-msize/4,color);
        gdImageLine(im,x+msize*0.433013,y-msize/4,x-msize*0.433013,y-msize/4,color);
        gdImageLine(im,x-msize*0.433013,y-msize/4,x,y+msize/2,color);
        break;
    case 7:
        gdImageLine(im,x-msize/2,y-msize/2,x+msize/2,y+msize/2,color);
        gdImageLine(im,x+msize/2,y+msize/2,x+msize/2,y-msize/2,color);
        gdImageLine(im,x+msize/2,y-msize/2,x-msize/2,y+msize/2,color);
        gdImageLine(im,x-msize/2,y+msize/2,x-msize/2,y-msize/2,color);
        break;
    case 8:
        gdImageLine(im,x-msize/2,y-msize/2,x+msize/2,y+msize/2,color);
        gdImageLine(im,x+msize/2,y+msize/2,x-msize/2,y+msize/2,color);
        gdImageLine(im,x-msize/2,y+msize/2,x+msize/2,y-msize/2,color);
        gdImageLine(im,x+msize/2,y-msize/2,x-msize/2,y-msize/2,color);
        break;
    case 9:
        gdImageLine(im,x,y-msize/2,x,y+msize/2,color);
        gdImageLine(im,x-msize/3,y-msize/3,x+msize/3,y+msize/3,color);
        gdImageLine(im,x+msize/3,y-msize/3,x-msize/3,y+msize/3,color);
        break;
    case 10:
        gdImageArc(im,x,y,msize/2,msize/2,0,360,color);
        gdImageLine(im,x+msize/4,y,x+msize/2,y,color);
        gdImageLine(im,x-msize/4,y,x-msize/2,y,color);
        gdImageLine(im,x,y+msize/4,x,y+msize/2,color);
        gdImageLine(im,x,y-msize/4,x,y-msize/2,color);
        break;
    }
}

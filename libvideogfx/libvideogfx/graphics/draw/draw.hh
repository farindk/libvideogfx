/*********************************************************************
  libvideogfx/graphics/draw/draw.hh

  purpose:

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de
   - Alexander Staller, alexander.staller@engineer.com

  modifications:
    30/Jan/2002 - Dirk Farin - New line clipping routine
    14/Aug/2000 - Alexander Staller - New functions: 
      DrawRectangle(), DrawCircle(), DrawEllipse(), DrawArrow()
      new class ArrowPainter, new implementation of DrawLine().
    14/Aug/2000 - Dirk Farin - new function: DrawFilledRectangleHV()
    17/Nov/1999 - Dirk Farin - converted bitmap drawing functions to
                               function templates.
 ********************************************************************************
    LibVideoGfx - video processing library
    Copyright (C) 2002  Dirk Farin

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ********************************************************************************/

#ifndef LIBVIDEOGFX_GRAPHICS_DRAW_DRAW_HH
#define LIBVIDEOGFX_GRAPHICS_DRAW_DRAW_HH

#include <math.h>
#include <algorithm>

#include <libvideogfx/graphics/datatypes/bitmap.hh>
#include <libvideogfx/graphics/datatypes/image.hh>
#include <libvideogfx/graphics/datatypes/primitives.hh>

#ifndef M_PI
#define M_PI 3.1415926535
#endif

namespace videogfx {

  /* This function-call is very inefficient but provides clipping. */
  template <class T> void DrawPoint     (Bitmap<T>&,int x,int y,T color);
  template <class T> void DrawPoint     (Image<T>& ,int x,int y,Color<T> color);

  /* Set all of the bitmap to the specified color. */
  template <class T> void Clear         (Bitmap<T>&,T color);
  template <class T> void Clear         (Image<T>& ,Color<T> color);

  /* Draw filled H/V-aligned rectangle. */
  template <class T> void FillRectangle(Bitmap<T>&,int x0,int y0,int x1,int y1,T color);
  template <class T> void FillRectangle(Image<T>&, int x0,int y0,int x1,int y1,Color<T> color);

  /* Clip line to a rectangle. If false is returned, the line is not visible. */
  bool ClipLine(int& x0,int& y0,int& x1,int& y1,        // line parameters
		int r_x0,int r_y0,int r_x1,int r_y1);   // clipping rectangle

  // Fast line drawing, line-clipping is included.
  template <class T> void DrawLine(Bitmap<T>& bm,int x0,int y0,int x1, int y1,T color);
  template <class T> void DrawLine(Image<T>&  bm,int x0,int y0,int x1, int y1,Color<T> color);

  /* Only draws every 4th dot. */
  template <class T> void DrawDottedLine(Bitmap<T>&,int x1,int y1,int x2,int y2,T color);

  // Draw a circle
  template <class T> void DrawCircle(Bitmap<T>& bm,int x0,int y0, int radius,T color,bool fill=false);
  template <class T> void DrawCircle(Image<T>&  bm,int x0,int y0, int radius,Color<T> color,bool fill=false);

  template <class T> void DrawRectangle(Bitmap<T>& bm,int x1,int y1,int w, int h,T color);
  template <class T> void DrawRectangle(Image<T>&  bm,int x1,int y1,int w, int h,Color<T> color);

  /* This function draws a line and places a head on one (x1,y1) if bothends==false or
     both (if bothends==true) sides of the line. */
  template <class T> void DrawArrow(Bitmap<T>& bm,int x0,int y0,int x1, int y1,T color,
				    double alpha=20.0,int len=7,bool bothends = false);


  /* A PolylinePainter helps to draw polygons.
     You initialize the painter with a bitmap or an image and add points of the
     polygon to it. Finally, you have to stroke the path. You can close the path
     beforehand by calling "Close()".
  */
  template <class T> class PolylinePainter
  {
  public:
    PolylinePainter()
      : d_bm(NULL), d_img(NULL), d_began(false) { }

    void SetBitmap(Bitmap<T>& b, T value)   { d_bm = &b; d_img=NULL; d_color.c[0]=value; d_began=false; }
    void SetImage (Image<T>& b, Color<T> c) { d_img = &b, d_bm=NULL; d_color=c; d_began=false; }

    void AddPoint(int x,int y)
    {
      if (!d_began) { d_start.x = d_last.x = x; d_start.y = d_last.y = y; d_began=true; return; }
      if (d_bm)  DrawLine(*d_bm, d_last.x, d_last.y, x,y, d_color.c[0]);
      if (d_img) DrawLine(*d_img,d_last.x, d_last.y, x,y, d_color);
      d_last.x = x;
      d_last.y = y;
    }
    void AddPoint(Point2D<int> p) { AddPoint(p.x,p.y); }
    void Close() { AddPoint(d_start); Stroke(); }
    void Stroke() { d_began=false; }

  private:
    Bitmap<T>*   d_bm;
    Image<T>*    d_img;
    Color<T>     d_color;
    bool         d_began;
    Point2D<int> d_start;
    Point2D<int> d_last;
  };


  // ---------------------------- implementation -------------------------


  template <class T> void DrawPoint(Bitmap<T>& bm,int x,int y,T color)
  {
    if (x<0 || y<0) return;
    if (x>=bm.AskWidth() || y>=bm.AskHeight()) return;

    bm.AskFrame()[y][x]=color;
  }

  template <class T> void DrawPoint(Image<T>& bm,int x,int y,Color<T> color)
  {
    ImageParam param = bm.AskParam();

    for (int i=0;i<4;i++)
      {
	BitmapChannel b = (BitmapChannel)i;
	if (!bm.AskBitmap(b).IsEmpty())
	  DrawPoint(bm.AskBitmap(b), param.ChromaScaleH(b,x), param.ChromaScaleV(b,y), color.c[i]);
      }
  }

  template <class T> void Clear(Bitmap<T>& bm,T color)
  {
    T*const* p = bm.AskFrame();
  
    if (sizeof(T)==1)
      {
	for (int y=0;y<bm.AskHeight();y++)
	  memset(p[y],color,bm.AskWidth());
      }
    else
      {
	FillRectangle(bm,0,0,bm.AskWidth()-1,bm.AskHeight()-1,color);
      }
  }

  template <class T> void Clear         (Image<T>& img,Color<T> color)
  {
    ImageParam param = img.AskParam();

    for (int i=0;i<4;i++)
      {
	BitmapChannel b = (BitmapChannel)i;
	if (!img.AskBitmap(b).IsEmpty())
	  Clear(img.AskBitmap(b), color.c[i]);
      }
  }

  template <class T> void FillRectangle(Bitmap<T>& bm,int x0,int y0,int x1,int y1,T color)
  {
    T*const* p = bm.AskFrame();

    for (int y=y0;y<=y1;y++)
      for (int x=x0;x<=x1;x++)
	p[y][x] = color;
  }


  template <class T> void FillRectangle(Image<T>& bm, int x0,int y0,int x1,int y1,Color<T> color)
  {
    ImageParam param = bm.AskParam();

    for (int i=0;i<4;i++)
      {
	BitmapChannel b = (BitmapChannel)i;
	if (!bm.AskBitmap(b).IsEmpty())
	  FillRectangle(bm.AskBitmap(b),
			param.ChromaScaleH(b,x0), param.ChromaScaleV(b,y0),
			param.ChromaScaleH(b,x1), param.ChromaScaleV(b,y1),
			color.c[i]);
      }
  }


  // main function to draw a line very fast. Clipping is included, so don't think about it
  template <class T> void DrawLine(Bitmap<T>& bm,int x0,int y0,int x1, int y1,T color)
  {
    T*const* p = bm.AskFrame();

    if (!ClipLine(x0,y0,x1,y1, 0,0,bm.AskWidth()-1,bm.AskHeight()-1))
      return;

    if (abs(y1-y0)>abs(x1-x0))
      {
	if (y1<y0) {  std::swap(x0,x1); std::swap(y0,y1); }

	int xinc;
	int dy = y1 - y0;
	int dx = x1 - x0;

	if (dx < 0)
	  {
	    xinc = -1;
	    dx = -dx;
	  }
	else
	  xinc = 1;

	int d = 2* dx - dy;
	int incrE = 2 * dx;  // Increment used for move to E
	int incrNE = 2 * (dx - dy);   // increment used for move to NE
	int y = y0;
	int x = x0;

	while (y <= y1)
	  {
	    p[y][x] = color;
     
	    if (d <= 0)   // Choose E
	      {
		d = d + incrE;
		y++;
	      }
	    else         // Choose NE
	      {
		d = d + incrNE;
		y++;
		x = x + xinc;
	      }
	  }
      }
    else
      {
	if (x1<x0) {  std::swap(x0,x1); std::swap(y0,y1); }

	int yinc;
	int dx = x1 - x0;
	int dy = y1 - y0;

	if (dy < 0)
	  {
	    yinc = -1;
	    dy = -dy;
	  }
	else
	  yinc = 1;

	int d = 2* dy - dx;
	int incrE = 2 * dy;  // Increment used for move to E
	int incrNE = 2 * (dy - dx);   // increment used for move to NE
	int x = x0;
	int y = y0;

	while (x <= x1)
	  {
	    p[y][x] = color;
     
	    if (d <= 0)   // Choose E
	      {
		d = d + incrE;
		x++;
	      }
	    else         // Choose NE
	      {
		d = d + incrNE;
		x++;
		y = y + yinc;
	      }
	  }
      }
  }

  template <class T> void DrawLine(Image<T>&  bm,int x0,int y0,int x1, int y1,Color<T> color)
  {
    ImageParam param = bm.AskParam();

    for (int i=0;i<4;i++)
      {
	BitmapChannel b = (BitmapChannel)i;
	if (!bm.AskBitmap(b).IsEmpty())
	  DrawLine(bm.AskBitmap(b),
		   param.ChromaScaleH(b,x0), param.ChromaScaleV(b,y0),
		   param.ChromaScaleH(b,x1), param.ChromaScaleV(b,y1),
		   color.c[i]);
      }
  }

  template <class T> void DrawDottedLine(Bitmap<T>& bm,int x1,int y1,int x2,int y2,T color)
  {
    int vx = x2-x1;
    int vy = y2-y1;
    DrawPoint(bm,x1,y1,color);

    if (abs(vx)>abs(vy))
      {
	for (int x=0;x!=vx;x+=4*sign(vx))
	  DrawPoint(bm , x1+x , vy*x/vx+y1 , color);
      }
    else
      {
	for (int y=0;y!=vy;y+=4*sign(vy))
	  DrawPoint(bm , x1+vx*y/vy , y1+y , color);
      }
  }


  //Pointer is set to this function if the circle is full visible 
  template <class T> void CirclePoints_Direct(Bitmap<T>& bm,int x0,int y0,int dx,int dy, T color)
  {
    T*const* p = bm.AskFrame();

    p[y0-dy][x0-dx]=color;
    p[y0-dy][x0+dx]=color;
    p[y0+dy][x0-dx]=color;
    p[y0+dy][x0+dx]=color;

    p[y0-dx][x0-dy]=color;
    p[y0-dx][x0+dy]=color;
    p[y0+dx][x0-dy]=color;
    p[y0+dx][x0+dy]=color;
  }

  //Pointer is set to this function if the circle is not fully visible 
  template <class T> static void CirclePoints_Save(Bitmap<T>& bm,int x0,int y0,int dx,int dy,T color)
  {
    T*const* p = bm.AskFrame();

    static int xMax= bm.AskWidth();
    static int yMax= bm.AskHeight();

    int y0dy1=y0-dy;
    int y0dy2=y0+dy;
    int x0dx1=x0-dx;
    int x0dx2=x0+dx;

    int y0dx1=y0-dx;
    int y0dx2=y0+dx;
    int x0dy1=x0-dy;
    int x0dy2=x0+dy;

  
    if ((y0dy1)>=0 && (x0dx1)>=0 && (y0dy1)<yMax && (x0dx1)<xMax)
      p[y0dy1][x0dx1]=color;
    if ((y0dy1)>=0 && (x0dx2)>=0 && (y0dy1)<yMax && (x0dx2)<xMax)
      p[y0dy1][x0dx2]=color;
    if ((y0dy2)>=0 && (x0dx1)>=0 && (y0dy2)<yMax && (x0dx1)<xMax)
      p[y0+dy][x0-dx]=color;
    if ((y0dy2)>=0 && (x0dx2)>=0 && (y0dy2)<yMax && (x0dx2)<xMax)
      p[y0+dy][x0+dx]=color;


    if ((y0dx1)>=0 && (x0dy1)>=0 && (y0dx1)<yMax && (x0dy1)<xMax)
      p[y0-dx][x0-dy]=color;
    if ((y0dx1)>=0 && (x0dy2)>=0 && (y0dx1)<yMax && (x0dy2)<xMax)
      p[y0-dx][x0+dy]=color;
    if ((y0dx2)>=0 && (x0dy1)>=0 && (y0dx2)<yMax && (x0dy1)<xMax)
      p[y0+dx][x0-dy]=color;
    if ((y0dx2)>=0 && (x0dy2)>=0 && (y0dx2)<yMax && (x0dy2)<xMax)
      p[y0+dx][x0+dy]=color;
  }

  template <class T> static void CirclePoints_Fill(Bitmap<T>& bm,int x0,int y0,int dx,int dy, T color)
  {
    int y0dy1=y0-dy;
    int y0dy2=y0+dy;
    int x0dx1=x0-dx;
    int x0dx2=x0+dx;

    int y0dx1=y0-dx;
    int y0dx2=y0+dx;
    int x0dy1=x0-dy;
    int x0dy2=x0+dy;

    DrawLine(bm,x0dx1,y0dy1,x0dx2,y0dy1,color);
    DrawLine(bm,x0dy1,y0dx1,x0dy2,y0dx1,color);
    DrawLine(bm,x0dy2,y0dx2,x0dy1,y0dx2,color);
    DrawLine(bm,x0dx2,y0dy2,x0dx1,y0dy2,color);      
  }

  // main function to draw a circle
  template <class T> void DrawCircle(Bitmap<T>& bm,int x0,int y0, int radius,T color,bool fill)
  {
    int x,y,d;

    void (*drawpoints)(Bitmap<T>& bm,int x0,int y0,int dx,int dy,T color);

    x=0;
    y=radius;
    d=1-radius;

    if (fill)
      { drawpoints = CirclePoints_Fill; }
    else if (x0-radius>=0 && x0+radius<bm.AskWidth() &&
	     y0-radius>=0 && y0+radius<bm.AskHeight())
      { drawpoints = CirclePoints_Direct; }
    else
      { drawpoints = CirclePoints_Save; }

    drawpoints(bm,x0,y0,x,y,color);

    while (y>x)
      {
	if (d<0)
	  {
	    d=d+2*x+3;
	    x++;
	  }
	else
	  {
	    d=d+2*(x-y)+5;
	    x++;
	    y--;
	  }
	drawpoints(bm,x0,y0,x,y,color);
      }
  }

  template <class T> void DrawCircle(Image<T>&  bm,int x0,int y0, int radius,Color<T> color,bool fill)
  {
    ImageParam param = bm.AskParam();

    if (param.colorspace == Colorspace_YUV)
      {
	AssertDescr(param.chroma == Chroma_420 ||
		    param.chroma == Chroma_444,"cannot draw circle because chroma pixels are not square");
      }

    for (int i=0;i<4;i++)
      {
	BitmapChannel b = (BitmapChannel)i;
	if (!bm.AskBitmap(b).IsEmpty())
	  DrawCircle(bm.AskBitmap(b),
		     param.ChromaScaleH(b,x0), param.ChromaScaleV(b,y0),
		     param.ChromaScaleH(b,radius), color.c[i], fill);
      }
  }


  template <class T> void DrawRectangle(Bitmap<T>& bm,int x1,int y1,int w, int h,T color)
  {
    T*const* p = bm.AskFrame();

    DrawLine(bm,x1,y1,x1+w,y1,color);
    DrawLine(bm,x1+w,y1,x1+w,y1+h,color);
    DrawLine(bm,x1+w,y1+h,x1,y1+h,color);
    DrawLine(bm,x1,y1+h,x1,y1,color);
  }


  template <class T> void DrawRectangle(Image<T>&  bm,int x1,int y1,int w, int h,Color<T> color)
  {
    ImageParam param = bm.AskParam();

    for (int i=0;i<4;i++)
      {
	BitmapChannel b = (BitmapChannel)i;
	if (!bm.AskBitmap(b).IsEmpty())
	  DrawRectangle(bm.AskBitmap(b),
			param.ChromaScaleH(b,x1), param.ChromaScaleV(b,y1),
			param.ChromaScaleH(b,w),  param.ChromaScaleH(b,h),
			color.c[i]);
      }
  }

  template <class T> void DrawArrow(Bitmap<T>& bm,int x0,int y0,int x1, int y1, T color,
				    double alpha,int l,bool bothends)
  {
    DrawLine(bm,x0,y0,x1,y1,color);     

    alpha *= M_PI/180.0;

    // draw head of arrow
    int xa,ya,dxp,dyp;
    double norm;
      
    dxp = x1-x0;
    dyp = y1-y0;

    norm = ::sqrt(((double)(dxp*dxp+dyp*dyp));

    if (norm>0.0)
      {
	xa = (int)((::cos(alpha)*dxp-::sin(alpha)*dyp)*l/norm);
	ya = (int)((::sin(alpha)*dxp+::cos(alpha)*dyp)*l/norm);

	DrawLine(bm,x1,y1,x1-xa,y1-ya,color);     

	if (bothends == true)
	  {
	    DrawLine(bm,x0,y0,x0+xa,y0+ya,color);
	  }

	xa = (int)(( ::cos(alpha)*dxp+::sin(alpha)*dyp)*l/norm);
	ya = (int)((-::sin(alpha)*dxp+::cos(alpha)*dyp)*l/norm);

	DrawLine(bm,x1,y1,x1-xa,y1-ya,color);  

	if (bothends == true)
	  {
	    DrawLine(bm,x0,y0,x0+xa,y0+ya,color);
	  }
      }
  }

}

#endif

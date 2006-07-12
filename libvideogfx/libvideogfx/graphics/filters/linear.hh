/*********************************************************************
  libvideogfx/graphics/filters/linear.hh

  purpose:
    linear filters:
    - LowPass ( 0 1 0 / 1 4 1 / 0 1 0 )
    - LowPass ( 1 1 1 / 1 1 1 / 1 1 1 )

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
    21/Jul/1999 - Dirk Farin - first implementation
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

#ifndef LIBVIDEOGFX_GRAPHICS_FILTERS_LINEAR_HH
#define LIBVIDEOGFX_GRAPHICS_FILTERS_LINEAR_HH

#include <libvideogfx/graphics/datatypes/image.hh>
#include <libvideogfx/graphics/draw/blit.hh>
#include <libvideogfx/containers/array.hh>
#include <libvideogfx/containers/array2.hh>
#include <algorithm>

namespace videogfx {

  template <class PelIn,class PelOut> void ConvolveH (Bitmap<PelOut>& dst,const Bitmap<PelIn>& src,
						      const Array<double>& filter);
  template <class PelIn,class PelOut> void ConvolveV (Bitmap<PelOut>& dst,const Bitmap<PelIn>& src,
						      const Array<double>& filter);
  template <class PelIn,class PelOut> void ConvolveHV(Bitmap<PelOut>& dst,const Bitmap<PelIn>& src,
						      const Array<double>& filter);

  template <class Pel> void ConvolveH (Bitmap<Pel>& bm, const Array<double>& filter);
  template <class Pel> void ConvolveV (Bitmap<Pel>& bm, const Array<double>& filter);
  template <class Pel> void ConvolveHV(Bitmap<Pel>& bm, const Array<double>& filter);

  template <class PelIn,class PelOut> void Prewitt_Hor(Bitmap<PelOut>& dst,const Bitmap<PelIn>& src);
  template <class PelIn,class PelOut> void Prewitt_Ver(Bitmap<PelOut>& dst,const Bitmap<PelIn>& src);
  template <class PelIn,class PelOut> void Sobel_Hor  (Bitmap<PelOut>& dst,const Bitmap<PelIn>& src);
  template <class PelIn,class PelOut> void Sobel_Ver  (Bitmap<PelOut>& dst,const Bitmap<PelIn>& src);

  /* 3x3 filters for "laplacian of gaussian"
     The 3-digit number specifies the filter. First digit is coefficient in the corner,
     second digit, is coefficient in the middle of the sides, and last digit is coefficient in the center. */
  void LaplOfGauss3x3_014(Bitmap<short>& dst,const Bitmap<Pixel>& src);
  void LaplOfGauss3x3_118(Bitmap<short>& dst,const Bitmap<Pixel>& src);
  void LaplOfGauss3x3_124(Bitmap<short>& dst,const Bitmap<Pixel>& src);

  /* Calculate a map of the gradient strength in the specified bitmap.
     Note: the result is scaled by a constant factor.
  */
  void CalcGaussGradientStrength(Bitmap<int16>& gradient,const Bitmap<Pixel>& bm,double sigma=1.5);

  void NormalizeFilter(Array<double>& filter); // Make coefficients sum up to 1.0 .

  void CreateGaussFilter     (Array<double>& filter,double sigma,double cutoffval=0.01,bool normalize=true);
  void CreateGaussDerivFilter(Array<double>& filter,double sigma,double cutoffval=0.01);


  // ------------------------ implementation -------------------------------------

  template <class PelIn,class PelOut> void ConvolveH(Bitmap<PelOut>& dst,const Bitmap<PelIn>& src,
						     const Array<double>& filter)
  {
    int left = -filter.AskBase();
    int right=  filter.AskSize()-left-1;

    int border=src.AskBorder();

    int w = src.AskWidth();
    int h = src.AskHeight();

    dst.Create(w,h,border);

    const double* f = filter.AskData();

    const PelIn*const* sp = src.AskFrame();
    PelOut*const* dp = dst.AskFrame();

    int xs = -border+left;
    int xe = w-1+border-right;


    // filter main image

    for (int y=0;y<h;y++)
      for (int x=xs;x<=xe;x++)
	{
	  double sum=0.0;
	  for (int xx=-left;xx<=right;xx++)
	    sum += f[xx]*sp[y][x+xx];
    
	  dp[y][x] = (PelOut)sum;
	}

    // filter left border

    if (xs>0)
      {
	Array<double> borderfilter = filter;
      
	double* bf = borderfilter.AskData();

	int first=filter.AskBase();
	for (int x=xs-1;x>=0;x--)
	  {
	    first++;
	    bf[first] += bf[first-1];

	    for (int y=0;y<h;y++)
	      {
		double sum=0.0;
		for (int xx=first;xx<=right;xx++)
		  sum += bf[xx]*sp[y][x+xx];

		dp[y][x] = (PelOut)sum;
	      }
	  }
      }

    // filter right border

    if (xe<w-1)
      {
	Array<double> borderfilter = filter;
      
	double* bf = borderfilter.AskData();

	int last=right;
	for (int x=xe+1;x<w;x++)
	  {
	    last--;
	    bf[last] += bf[last+1];

	    for (int y=0;y<h;y++)
	      {
		double sum=0.0;
		for (int xx=-left;xx<=last;xx++)
		  sum += bf[xx]*sp[y][x+xx];

		dp[y][x] = (PelOut)sum;
	      }
	  }
      }
  }


  template <class PelIn,class PelOut> void ConvolveV(Bitmap<PelOut>& dst,const Bitmap<PelIn>& src,
						     const Array<double>& filter)
  {
    int left = -filter.AskBase();
    int right=  filter.AskSize()-left-1;

    int border=src.AskBorder();

    int w = src.AskWidth();
    int h = src.AskHeight();

    dst.Create(w,h,border);

    const double* f = filter.AskData();

    const PelIn*const* sp = src.AskFrame();
    PelOut*const* dp = dst.AskFrame();

    int ys = -border+left;
    int ye = h-1+border-right;


    // filter main image

    for (int y=ys;y<=ye;y++)
      for (int x=0;x<w;x++)
	{
	  double sum=0.0;
	  for (int yy=-left;yy<=right;yy++)
	    sum += f[yy]*sp[y+yy][x];
    
	  dp[y][x] = (PelOut)sum;
	}

    // filter left border

    if (ys>0)
      {
	Array<double> borderfilter = filter;
      
	double* bf = borderfilter.AskData();

	int first=filter.AskBase();
	for (int y=ys-1;y>=0;y--)
	  {
	    first++;
	    bf[first] += bf[first-1];

	    for (int x=0;x<w;x++)
	      {
		double sum=0.0;
		for (int yy=first;yy<=right;yy++)
		  sum += bf[yy]*sp[y+yy][x];

		dp[y][x] = (PelOut)sum;
	      }
	  }
      }

    // filter right border

    if (ye<h-1)
      {
	Array<double> borderfilter = filter;
      
	double* bf = borderfilter.AskData();

	int last=right;
	for (int y=ye+1;y<h;y++)
	  {
	    last--;
	    bf[last] += bf[last+1];

	    for (int x=0;x<w;x++)
	      {
		double sum=0.0;
		for (int yy=-left;yy<=last;yy++)
		  sum += bf[yy]*sp[y+yy][x];

		dp[y][x] = (PelOut)sum;
	      }
	  }
      }
  }


  template <class PelIn,class PelOut> void ConvolveHV(Bitmap<PelOut>& dst,const Bitmap<PelIn>& src,
						      const Array<double>& filter)
  {
    Bitmap<double> tmpbm;
    ConvolveH(tmpbm,src,filter);
    ConvolveV(dst,tmpbm,filter);
  }


  template <class PelIn,class PelOut> void Convolve(Bitmap<PelOut>& dst,const Bitmap<PelIn>& src,
						    const Array2<double>& filter)
  {
    int r_left   = filter.AskXBase();
    int r_right  = filter.AskWidth()-filter.AskXBase()-1;
    int r_top    = filter.AskYBase();
    int r_bottom = filter.AskHeight()-filter.AskYBase()-1;

    // make a copy of the input with border to simplify computation
    int border = std::max(std::max(r_left,r_right),std::max(r_top,r_bottom));
    Bitmap<PelIn> bordsrc = src.Clone(border);
    ExtrudeIntoBorder(bordsrc);


    int w=src.AskWidth(), h=src.AskHeight();
    dst.Create(w,h);

    /* */ PelOut*const* dp =     dst.AskFrame();
    const PelIn* const* sp = bordsrc.AskFrame();

    for (int y=0;y<h;y++)
      for (int x=0;x<w;x++)
	{
	  double sum = 0.0;

	  for (int fy=-r_top;fy<=r_bottom;fy++)
	    for (int fx=-r_left;fx<=r_right;fx++)
	      sum += filter[fy][fx] * sp[y+fy][x+fx];

	  dp[y][x] = (PelOut)sum;
	}
  }


  template <class Pel> void ConvolveH(Bitmap<Pel>& bm,const Array<double>& filter)
  {
    Bitmap<Pel> tmpbm;
    ConvolveH(tmpbm,bm,filter);
    Copy(bm,tmpbm);
  }

  template <class Pel> void ConvolveV(Bitmap<Pel>& bm,const Array<double>& filter)
  {
    Bitmap<Pel> tmpbm;
    ConvolveV(tmpbm,bm,filter);
    Copy(bm,tmpbm);
  }

  template <class Pel> void ConvolveHV(Bitmap<Pel>& bm,const Array<double>& filter)
  {
    Bitmap<double> tmpbm;
    ConvolveH(tmpbm,bm,filter);
    ConvolveV(bm,tmpbm,filter);
  }


  template <class PelIn,class PelOut> void Prewitt_Hor(Bitmap<PelOut>& dst,const Bitmap<PelIn>& src)
  {
    Array<double> filter(3,-1);
    filter[-1]=1.0/3;
    filter[ 0]=1.0/3;
    filter[ 1]=1.0/3;

    Bitmap<double> tmpbm;
    ConvolveV(tmpbm,src,filter);

    filter[-1]=-1;
    filter[ 0]=0;
    filter[ 1]=1;

    ConvolveH(dst,tmpbm,filter);
  }

  template <class PelIn,class PelOut> void Prewitt_Ver(Bitmap<PelOut>& dst,const Bitmap<PelIn>& src)
  {
    Array<double> filter(3,-1);
    filter[-1]=1.0/3;
    filter[ 0]=1.0/3;
    filter[ 1]=1.0/3;

    Bitmap<double> tmpbm;
    ConvolveH(tmpbm,src,filter);

    filter[-1]=-1;
    filter[ 0]=0;
    filter[ 1]=1;

    ConvolveV(dst,tmpbm,filter);
  }

  template <class PelIn,class PelOut> void Sobel_Hor(Bitmap<PelOut>& dst,const Bitmap<PelIn>& src)
  {
    Array<double> filter(3,-1);
    filter[-1]=1.0/4;
    filter[ 0]=2.0/4;
    filter[ 1]=1.0/4;

    Bitmap<double> tmpbm;
    ConvolveV(tmpbm,src,filter);

    filter[-1]=-1;
    filter[ 0]=0;
    filter[ 1]=1;

    ConvolveH(dst,tmpbm,filter);
  }

  template <class PelIn,class PelOut> void Sobel_Ver(Bitmap<PelOut>& dst,const Bitmap<PelIn>& src)
  {
    Array<double> filter(3,-1);
    filter[-1]=1.0/4;
    filter[ 0]=2.0/4;
    filter[ 1]=1.0/4;

    Bitmap<double> tmpbm;
    ConvolveH(tmpbm,src,filter);

    filter[-1]=-1;
    filter[ 0]=0;
    filter[ 1]=1;

    ConvolveV(dst,tmpbm,filter);
  }

}

#endif

/********************************************************************************
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

#include "libvideogfx/graphics/filters/linear.hh"

#include <math.h>
#include <iostream>
using namespace std;

namespace videogfx {

#if 0
  /* Low-pass filtering is done using this kernel:

     1   /  0  1  0  \
     - * |  1  4  1  |
     8   \  0  1  0  /
  */
  void LowPass_5pt(const Image_YUV<Pixel>& img,Image_YUV<Pixel>& dest);


  /* Low-pass filtering is done using this kernel:

     1   /  1  1  1  \
     - * |  1  1  1  |
     9   \  1  1  1  /
  */
  void LowPass_3x3mean(const Image_YUV<Pixel>& img,Image_YUV<Pixel>& dest);

  /* Apply interpolation filter with the kernel (1 3 3 1)//4 .
   * 'src' and 'dst' may be the same.
   */
  void InterpolateH_Tap4(const Pixel*const* src,Pixel*const* dst,int dst_width,int height);
  void InterpolateV_Tap4(const Pixel*const* src,Pixel*const* dst,int width,int dst_height);
#endif


#if 0
  void LowPass_5pt(const Image_YUV<Pixel>& img,Image_YUV<Pixel>& dest)
  {
    // Get image parameters and assure that they are in the right format.
    ImageParam_YUV param;
    img.GetParam(param);

    ImageParam_YUV param2;
    dest.GetParam(param2);

    assert(param.chroma ==Chroma444);
    assert(param2.chroma==Chroma444);
    assert(&img != &dest);  // Lowpass needs two image buffers for correct operation.

    // Give hint as the destination image will be completely overwritten.
    dest.Hint_ContentIsNotUsedAnymore();


    Pixel*const* yp2  = dest.AskFrameY();
    Pixel*const* up2  = dest.AskFrameU();
    Pixel*const* vp2  = dest.AskFrameV();

    const Pixel*const* yp  = img.AskFrameY_const();
    const Pixel*const* up  = img.AskFrameU_const();
    const Pixel*const* vp  = img.AskFrameV_const();

    /* Do lowpass filtering.
       We filter all of the image except a one pixel wide border because the
       filter size is 3x3. This border will simply be copied from the original
       image.
    */
     
    int w = param.width;
    int h = param.height;

    for (int y=1;y<h-1;y++)
      for (int x=1;x<w-1;x++)
	{
	  yp2[y][x] = (  yp[y-1][x  ] +
			 yp[y+1][x  ] +
			 yp[y  ][x-1] +
			 yp[y  ][x+1] +
			 4*yp[y  ][x  ]    +4)/8;
	  up2[y][x] = (  up[y-1][x  ] +
			 up[y+1][x  ] +
			 up[y  ][x-1] +
			 up[y  ][x+1] +
			 4*up[y  ][x  ]    +4)/8;
	  vp2[y][x] = (  vp[y-1][x  ] +
			 vp[y+1][x  ] +
			 vp[y  ][x-1] +
			 vp[y  ][x+1] +
			 4*vp[y  ][x  ]    +4)/8;
	}

    // Copy border from old image to filtered one.

    for (int x=0;x<w;x++)
      {
	yp2[  0][x]=yp[  0][x]; up2[  0][x]=up[  0][x]; vp2[  0][x]=vp[  0][x];
	yp2[h-1][x]=yp[h-1][x]; up2[h-1][x]=up[h-1][x]; vp2[h-1][x]=vp[h-1][x];
      }

    for (int y=0;y<h;y++)
      {
	yp2[y][  0]=yp[y][  0]; up2[y][0  ]=up[y][  0]; vp2[y][  0]=vp[y][  0];
	yp2[y][w-1]=yp[y][w-1]; up2[y][w-1]=up[y][w-1]; vp2[y][w-1]=vp[y][w-1];
      }
  }



  void LowPass_3x3mean(const Image_YUV<Pixel>& img,Image_YUV<Pixel>& dest)
  {
    // Get image parameters and assure that they are in the right format.
    ImageParam_YUV param;
    img.GetParam(param);

    ImageParam_YUV param2;
    dest.GetParam(param2);

    assert(param.chroma ==Chroma444);
    assert(param2.chroma==Chroma444);
    assert(&img != &dest);  // Lowpass needs two image buffers for correct operation.

    // Give hint as the destination image will be completely overwritten.
    dest.Hint_ContentIsNotUsedAnymore();


    Pixel*const* yp2  = dest.AskFrameY();
    Pixel*const* up2  = dest.AskFrameU();
    Pixel*const* vp2  = dest.AskFrameV();

    const Pixel*const* yp  = img.AskFrameY_const();
    const Pixel*const* up  = img.AskFrameU_const();
    const Pixel*const* vp  = img.AskFrameV_const();

    /* Do lowpass filtering.
       We filter all of the image except a one pixel wide border because the
       filter size is 3x3. This border will simply be copied from the original
       image.
    */
     
    int w = param.width;
    int h = param.height;

    for (int y=1;y<param.height-1;y++)
      for (int x=1;x<param.width-1;x++)
	{
	  yp2[y][x] = (  yp[y-1][x-1] +
			 yp[y-1][x  ] +
			 yp[y-1][x+1] +
			 yp[y  ][x-1] +
			 yp[y  ][x  ] +
			 yp[y  ][x+1] +
			 yp[y+1][x-1] +
			 yp[y+1][x  ] +
			 yp[y+1][x+1]    +4)/9;
	  up2[y][x] = (  up[y-1][x-1] +
			 up[y-1][x  ] +
			 up[y-1][x+1] +
			 up[y  ][x-1] +
			 up[y  ][x  ] +
			 up[y  ][x+1] +
			 up[y+1][x-1] +
			 up[y+1][x  ] +
			 up[y+1][x+1]    +4)/9;
	  vp2[y][x] = (  vp[y-1][x-1] +
			 vp[y-1][x  ] +
			 vp[y-1][x+1] +
			 vp[y  ][x-1] +
			 vp[y  ][x  ] +
			 vp[y  ][x+1] +
			 vp[y+1][x-1] +
			 vp[y+1][x  ] +
			 vp[y+1][x+1]    +4)/9;
	}

    // Copy border from old image to filtered one.

    for (int x=0;x<param.width;x++)
      {
	yp2[  0][x]=yp[  0][x]; up2[  0][x]=up[  0][x]; vp2[  0][x]=vp[  0][x];
	yp2[h-1][x]=yp[h-1][x]; up2[h-1][x]=up[h-1][x]; vp2[h-1][x]=vp[h-1][x];
      }

    for (int y=0;y<param.height;y++)
      {
	yp2[y][  0]=yp[y][  0]; up2[y][0  ]=up[y][  0]; vp2[y][  0]=vp[y][  0];
	yp2[y][w-1]=yp[y][w-1]; up2[y][w-1]=up[y][w-1]; vp2[y][w-1]=vp[y][w-1];
      }
  }


  void InterpolateH_Tap4(const Pixel*const* src,Pixel*const* dst,int dst_width,int height)
  {
    int src_width = (dst_width+1)/2;

    // Create a chroma-line buffer in the range [-1 .. cw]
    Pixel* line = new Pixel[src_width+2];
    Pixel* linep = &line[1];

    // Process interpolation filter
    for (int y=0;y<height;y++)
      {
	// copy line and duplicate pixels at the end
	for (int x=0;x<src_width;x++)
	  linep[x] = src[y][x];
	linep[-1]        = linep[0];
	linep[src_width] = linep[src_width-1];

	// Apply filter (1 3 3 1)//4
	for (int x=0;x<src_width;x++)
	  {
	    dst[y][2*x  ] = (linep[x]*3 + linep[x-1]+2)/4;
	    dst[y][2*x+1] = (linep[x]*3 + linep[x+1]+2)/4;
	  }
      }

    delete[] line;
  }


  void InterpolateV_Tap4(const Pixel*const* src,Pixel*const* dst,int width,int dst_height)
  {
    int src_height = (dst_height+1)/2;

    // Create a chroma-line buffer in the range [-1 .. cw]
    Pixel* line = new Pixel[src_height+2];
    Pixel* linep = &line[1];

    // Process interpolation filter
    for (int x=0;x<width;x++)
      {
	// copy line and duplicate pixels at the end
	for (int y=0;y<src_height;y++)
	  linep[y] = src[y][x];
	linep[-1]         = linep[0];
	linep[src_height] = linep[src_height-1];

	// Apply filter (1 3 3 1)//4
	for (int y=0;y<src_height;y++)
	  {
	    dst[2*y  ][x] = (linep[y]*3 + linep[y-1]+2)/4;
	    dst[2*y+1][x] = (linep[y]*3 + linep[y+1]+2)/4;
	  }
      }

    delete[] line;
  }

#endif


  void NormalizeFilter(Array<double>& filter)
  {
    double sum=0.0;
    int i0 = filter.AskStartIdx();
    int i1 = filter.AskEndIdx();

    double* f = filter.AskData();

    for (int i=i0;i<=i1;i++)
      sum += f[i];

    AssertDescr(sum != 0.0, "Filter cannot be normalized since coefficients sum to zero.");

    const double fact = 1.0/sum;

    for (int i=i0;i<=i1;i++)
      f[i] *= fact;
  }


  void CreateGaussFilter(Array<double>& filter,double sigma,double cutoffval,bool normalize)
  {
#define MAXRANGE 100
    double filt[MAXRANGE];

    double minus_twosigma2inv = -1.0/(2*sigma*sigma);

    int lastidx=MAXRANGE-1;
    for (int i=0;i<MAXRANGE;i++)
      {
	filt[i] = exp(i*i*minus_twosigma2inv);

	if (filt[i] < cutoffval)
	  { lastidx = i-1; break; }
      }

    if (lastidx==MAXRANGE-1)
      throw "CreateGaussFilter(): Gauss filter is too wide.";

    filter.Create(2*lastidx+1 , -lastidx);
    double* f = filter.AskData();

    for (int i=0;i<=lastidx;i++)
      f[-i]=f[i]=filt[i];

    if (normalize) NormalizeFilter(filter);
  }


  void CreateGaussDerivFilter(Array<double>& filter,double sigma,double cutoffval)
  {
    CreateGaussFilter(filter,sigma,cutoffval,false);

    for (int i=filter.AskStartIdx();i<=filter.AskEndIdx();i++)
      filter.AskData()[i] *= i;

    // normalize

    double sum=0.0;

    int i0 = filter.AskStartIdx();
    int i1 = filter.AskEndIdx();

    double* f = filter.AskData();

    for (int i=i0;i<=i1;i++)
      sum += i*f[i];

    const double fact = 1.0/sum;

    for (int i=i0;i<=i1;i++)
      f[i] *= fact;
  }

  void CalcGaussGradientStrength(Bitmap<int16>& gradient,const Bitmap<Pixel>& bm,double sigma)
  {
    int w = bm.AskWidth();
    int h = bm.AskHeight();

    Array<double> lowpass,deriv;
    CreateGaussFilter(lowpass,sigma,0.1);
    CreateGaussDerivFilter(deriv,sigma,0.1);

    Bitmap<Pixel> tmp;
    Bitmap<int16> deriv_h, deriv_v;

    ConvolveH(tmp, bm, lowpass);
    ConvolveV(deriv_v, tmp, deriv);

    ConvolveV(tmp, bm, lowpass);
    ConvolveH(deriv_h, tmp, deriv);


    // calculate gradient strength

    gradient.Create(w,h);

    {
      int16*const* valp = gradient.AskFrame();
      const int16*const* hp = deriv_h.AskFrame();
      const int16*const* vp = deriv_v.AskFrame();

      for (int y=0;y<h;y++)
	for (int x=0;x<w;x++)
	  valp[y][x] = (hp[y][x]*hp[y][x] + vp[y][x]*vp[y][x])/4;  // divide by four to limit range
    }
  }



  template void ConvolveH (Bitmap<Pixel>&,const Bitmap<Pixel>&,const Array<double>&);
  template void ConvolveV (Bitmap<Pixel>&,const Bitmap<Pixel>&,const Array<double>&);
  template void ConvolveHV(Bitmap<Pixel>&,const Bitmap<Pixel>&,const Array<double>&);

  template void ConvolveH (Bitmap<Pixel>&,const Array<double>&);
  template void ConvolveV (Bitmap<Pixel>&,const Array<double>&);
  template void ConvolveHV(Bitmap<Pixel>&,const Array<double>&);

}

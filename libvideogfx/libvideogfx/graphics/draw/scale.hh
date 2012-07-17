/*********************************************************************
  libvideogfx/graphics/draw/scale.hh

  purpose:
    Functions for extending borders, change format of bitmaps...

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
    04/Jul/2000 - Dirk Farin - bitmap format conversion and helpers
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

#ifndef LIBVIDEOGFX_GRAPHICS_DRAW_SCALE_HH
#define LIBVIDEOGFX_GRAPHICS_DRAW_SCALE_HH

#include <libvideogfx/graphics/datatypes/bitmap.hh>
#include <libvideogfx/graphics/datatypes/image.hh>

namespace videogfx {

  /* TODO: we seem to have a subpixel image shift in the Scale_NN and Scale_Bilinear functions.
     Check that this is correct... Same probably for the BinomialDownsample() function.
  */

  template <class Pel> void Scale_NN      (Bitmap<Pel>& dst,const Bitmap<Pel>& src, int newWidth, int newHeight);
  template <class Pel> void Scale_Bilinear(Bitmap<Pel>& dst,const Bitmap<Pel>& src, int newWidth, int newHeight);

  template <class Pel> void DoubleSize_Dup  (Bitmap<Pel>& dst,const Bitmap<Pel>& src);
  template <class Pel> void DoubleSize_Dup_H(Bitmap<Pel>& dst,const Bitmap<Pel>& src);
  template <class Pel> void DoubleSize_Dup_V(Bitmap<Pel>& dst,const Bitmap<Pel>& src);

  template <class Pel> void HalfSize_Avg  (Bitmap<Pel>& dst,const Bitmap<Pel>& src);
  template <class Pel> void HalfSize_Avg_H(Bitmap<Pel>& dst,const Bitmap<Pel>& src);
  template <class Pel> void HalfSize_Avg_V(Bitmap<Pel>& dst,const Bitmap<Pel>& src);

  template <class Pel> void ScaleDownOctaves_Avg(Bitmap<Pel>& dst,const Bitmap<Pel>& src, int nOctaves);


  /* Copy scaled version of region into another bitmap.
   */
  template <class Pel> void CopyScaled(Bitmap<Pel>& dst,       int dstx0,int dsty0, int dw,int dh,
				       const Bitmap<Pel>& src, int srcx0,int srcy0, int sw,int sh);
  template <class Pel> void CopyScaled(Image<Pel>& dst,       int dstx0,int dsty0, int dw,int dh,
				       const Image<Pel>& src, int srcx0,int srcy0, int sw,int sh);

  // Same as above, complete image/bitmap is inserted.
  template <class Pel> void CopyScaled(Bitmap<Pel>& dst,       int dstx0,int dsty0, int dw,int dh,
				       const Bitmap<Pel>& src);
  template <class Pel> void CopyScaled(Image<Pel>& dst,       int dstx0,int dsty0, int dw,int dh,
				       const Image<Pel>& src);


  // ------------------------------- implementation -----------------------------------

  template <class Pel> void DoubleSize_Dup  (Bitmap<Pel>& dst,const Bitmap<Pel>& src)
  {
    assert(&dst != &src);

    const int w = src.AskWidth();
    const int h = src.AskHeight();

    dst.Create(w*2,h*2);

    const Pel*const* sp = src.AskFrame();
    Pel*const* dp = dst.AskFrame();

    for (int y=0;y<h;y++)
      for (int x=0;x<w;x++)
	{
	  dp[y*2  ][x*2  ] =
	    dp[y*2  ][x*2+1] =
	    dp[y*2+1][x*2  ] =
	    dp[y*2+1][x*2+1] = sp[y][x];
	}
  }

  template <class Pel> void DoubleSize_Dup_H(Bitmap<Pel>& dst,const Bitmap<Pel>& src)
  {
    assert(&dst != &src);

    const int w = src.AskWidth();
    const int h = src.AskHeight();

    dst.Create(w*2,h);

    const Pel*const* sp = src.AskFrame();
    Pel*const* dp = dst.AskFrame();

    for (int y=0;y<h;y++)
      for (int x=0;x<w;x++)
	{
	  dp[y][x*2  ] =
	    dp[y][x*2+1] = sp[y][x];
	}
  }

  template <class Pel> void DoubleSize_Dup_V(Bitmap<Pel>& dst,const Bitmap<Pel>& src)
  {
    assert(&dst != &src);

    const int w = src.AskWidth();
    const int h = src.AskHeight();

    dst.Create(w,h*2);

    const Pel*const* sp = src.AskFrame();
    Pel*const* dp = dst.AskFrame();

    for (int y=0;y<h;y++)
      for (int x=0;x<w;x++)
	{
	  dp[y*2  ][x] =
	    dp[y*2+1][x] = sp[y][x];
	}
  }

  template <class Pel> void HalfSize_Avg  (Bitmap<Pel>& dst,const Bitmap<Pel>& src)
  {
    assert(&dst != &src);

    const int w = src.AskWidth();
    const int h = src.AskHeight();

    const int newh = (h+1)/2;
    const int neww = (w+1)/2;

    dst.Create(neww,newh);

    const Pel*const* sp = src.AskFrame();
    Pel*const* dp = dst.AskFrame();

    for (int y=0;y<h/2;y++)
      for (int x=0;x<w/2;x++)
	{
	  dp[y][x] = (sp[2*y  ][2*x] + sp[2*y  ][2*x+1] +
		      sp[2*y+1][2*x] + sp[2*y+1][2*x+1]) / 4;
	}

    if (neww*2 != w)
      {
	assert(w==neww*2-1);
	for (int y=0;y<newh;y++)
	  dp[y][neww-1] = sp[2*y][w-1];
      }

    if (newh*2 != h)
      {
	assert(h==newh*2-1);
	for (int x=0;x<neww;x++)
	  dp[newh-1][x] = sp[h-1][2*x];
      }
  }

  template <class Pel> void HalfSize_Avg_H(Bitmap<Pel>& dst,const Bitmap<Pel>& src)
  {
    assert(&dst != &src);

    const int w = src.AskWidth();
    const int h = src.AskHeight();

    const int neww = (w+1)/2;

    dst.Create(neww,h);

    const Pel*const* sp = src.AskFrame();
    Pel*const* dp = dst.AskFrame();

    for (int y=0;y<h;y++)
      for (int x=0;x<w/2;x++)
	{
	  dp[y][x] = (sp[y][2*x] + sp[y][2*x+1]) / 2;
	}

    if (neww*2 != w)
      {
	assert(w==neww*2-1);
	for (int y=0;y<h;y++)
	  dp[y][neww-1] = sp[y][w-1];
      }
  }

  template <class Pel> void HalfSize_Avg_V(Bitmap<Pel>& dst,const Bitmap<Pel>& src)
  {
    assert(&dst != &src);

    const int w = src.AskWidth();
    const int h = src.AskHeight();

    const int newh = (h+1)/2;

    dst.Create(w,newh);

    const Pel*const* sp = src.AskFrame();
    Pel*const* dp = dst.AskFrame();

    for (int y=0;y<h/2;y++)
      for (int x=0;x<w;x++)
	{
	  dp[y][x] = (sp[2*y  ][x] + sp[2*y+1][x]) / 2;
	}

    if (newh*2 != h)
      {
	assert(h==newh*2-1);
	for (int x=0;x<w;x++)
	  dp[newh-1][x] = sp[h-1][x];
      }
  }

  template <class Pel> void ScaleDownOctaves_Avg(Bitmap<Pel>& dst,const Bitmap<Pel>& src, int nOctaves)
  {
    if (nOctaves==0)
      { dst = src.Clone(); }
    else
      {
	HalfSize_Avg(dst,src);
	nOctaves--;

	while (nOctaves>0)
	  {
	    Bitmap<Pel> tmp = dst;
	    dst.Release();
	    
	    HalfSize_Avg(dst,tmp);
	    nOctaves--;
	  }
      }
  }

  template <class Pel> void CopyScaled(Bitmap<Pel>& dst,       int dstx0,int dsty0, int dw,int dh,
				       const Bitmap<Pel>& src, int srcx0,int srcy0, int sw,int sh)
  {
    if (src.IsEmpty()) return;

    const Pel*const* sp = src.AskFrame();
    /* */ Pel*const* dp = dst.AskFrame();

    int w = dst.AskWidth();
    int h = dst.AskHeight();
    int srcw = src.AskWidth();
    int srch = src.AskHeight();

    for (int y=std::max(0,-dsty0);y<dh;y++)
      {
	int srcy = srcy0 + y*sh/dh;
	if (srcy<0) continue;
	if (srcy>=srch) break;
	if (dsty0+y>=h) break;

	for (int x=std::max(0,-dstx0);x<dw;x++)
	  {
	    int srcx = srcx0+x*sw/dw;
	    if (srcx<0) continue;
	    if (srcx>srcw) break;
	    if (dstx0+x>=w) break;

	    dp[dsty0+y][dstx0+x] = sp[srcy][srcx];
	  }
      }
  }


  template <class Pel> void CopyScaled(Image<Pel>& dst,       int dstx0,int dsty0, int dw,int dh,
				       const Image<Pel>& src, int srcx0,int srcy0, int sw,int sh)
  {
    ImageParam param = src.AskParam();

    for (int i=0;i<4;i++)
      {
	BitmapChannel b = (BitmapChannel)i;
	CopyScaled(dst.AskBitmap(b),
		   param.ChromaScaleH(b,dstx0),param.ChromaScaleV(b,dsty0),
		   param.ChromaScaleH(b,dw), param.ChromaScaleV(b,dh),

		   src.AskBitmap(b),
		   param.ChromaScaleH(b,srcx0),param.ChromaScaleV(b,srcy0),
		   param.ChromaScaleH(b,sw), param.ChromaScaleV(b,sh));
      }
  }

  template <class Pel> void CopyScaled(Bitmap<Pel>& dst,       int dstx0,int dsty0, int dw,int dh,
				       const Bitmap<Pel>& src)
  {
    CopyScaled(dst,dstx0,dsty0,dw,dh, src,
	       -src.AskXOffset(),-src.AskYOffset(),src.AskWidth()-src.AskXOffset(),src.AskHeight()-src.AskYOffset());
  }

  template <class Pel> void CopyScaled(Image<Pel>& dst,       int dstx0,int dsty0, int dw,int dh,
				       const Image<Pel>& src)
  {
    CopyScaled(dst,dstx0,dsty0,dw,dh, src,
	       -src.AskXOffset(),-src.AskYOffset(),src.AskWidth()-src.AskXOffset(),src.AskHeight()-src.AskYOffset());
  }



  inline void FillScaleMapping(int* mapping, unsigned char* factor, int w, int newW)
  {
    for (int x=0;x<newW;x++)
      {
	if (factor==NULL)
	  { mapping[x] = (w-1)*x/(newW -1); }
	else
	  {
	    int pos = 256*(w-1)*x/(newW -1);
	    mapping[x] = pos/256;
	    factor[x]  = pos%256;
	  }
      }
  }


  template <class Pel> void Scale_NN(Bitmap<Pel>& dst,const Bitmap<Pel>& src, int newWidth, int newHeight)
  {
    int* mapX = new int[newWidth];
    int* mapY = new int[newHeight];

    int w = src.getWidth();
    int h = src.getHeight();

    FillScaleMapping(mapX, NULL, w, newWidth);
    FillScaleMapping(mapY, NULL, h, newHeight);

    dst.Create(newWidth, newHeight);

    const Pel*const* in  = src.AskFrame();
    /* */ Pel*const* out = dst.AskFrame();

    for (int y=0;y<newHeight;y++)
      {
	const int iy = mapY[y];

	for (int x=0;x<newWidth;x++)
	  {
	    dst[y][x] = in[iy][ mapX[x] ];
	  }
      }

    delete[] mapX;
    delete[] mapY;
  }


  template <class Pel> void Scale_Bilinear(Bitmap<Pel>& dst,const Bitmap<Pel>& src, int newWidth, int newHeight)
  {
    int* mapX = new int[newWidth];
    int* mapY = new int[newHeight];
    unsigned char* factorX = new unsigned char[newWidth];
    unsigned char* factorY = new unsigned char[newHeight];

    int w = src.getWidth();
    int h = src.getHeight();

    // we set the maximum value to w-1 so that we can always access the pixel at mapX[x]+1
    FillScaleMapping(mapX, factorX, w-1, newWidth);
    FillScaleMapping(mapY, factorY, h-1, newHeight);

    dst.Create(newWidth, newHeight);

    const Pel*const* in  = src.AskFrame();
    /* */ Pel*const* out = dst.AskFrame();

    for (int y=0;y<newHeight;y++)
      {
	const int iy = mapY[y];
	const int factor = factorY[y];

	for (int x=0;x<newWidth;x++)
	  {
	    int ix = mapX[x];

	    Pel A = in[iy][ix];
	    Pel B = in[iy][ix+1];
	    Pel C = in[iy+1][ix];
	    Pel D = in[iy+1][ix+1];

	    Pel left  = (A*256 + (C-A)*factor) / 256;
	    Pel right = (B*256 + (D-B)*factor) / 256;

	    dst[y][x] = (left*256 + (right-left)*factorX[x]) / 256;
	  }
      }

    delete[] mapX;
    delete[] mapY;
    delete[] factorX;
    delete[] factorY;
  }

}


#endif

#/*********************************************************************
  libvideogfx/graphics/draw/scale.hh

  purpose:
    Functions for extending borders, change format of bitmaps...

  notes:

  to do:

  author(s):
   - Dirk Farin, farin@ti.uni-mannheim.de
     University Mannheim, Dept. Circuitry and Simulation
     B 6,26 EG, room 0.10 / D-68131 Mannheim / Germany

  modifications:
    04/Jul/2000 - Dirk Farin - bitmap format conversion and helpers
 *********************************************************************/

#ifndef LIBVIDEOGFX_GRAPHICS_DRAW_SCALE_HH
#define LIBVIDEOGFX_GRAPHICS_DRAW_SCALE_HH

#include <libvideogfx/graphics/datatypes/bitmap.hh>
#include <libvideogfx/graphics/datatypes/image.hh>

namespace videogfx {

  template <class Pel> void DoubleSize_Dup  (Bitmap<Pel>& dst,const Bitmap<Pel>& src);
  template <class Pel> void DoubleSize_Dup_H(Bitmap<Pel>& dst,const Bitmap<Pel>& src);
  template <class Pel> void DoubleSize_Dup_V(Bitmap<Pel>& dst,const Bitmap<Pel>& src);

  template <class Pel> void HalfSize_Avg  (Bitmap<Pel>& dst,const Bitmap<Pel>& src);
  template <class Pel> void HalfSize_Avg_H(Bitmap<Pel>& dst,const Bitmap<Pel>& src);
  template <class Pel> void HalfSize_Avg_V(Bitmap<Pel>& dst,const Bitmap<Pel>& src);


  /* Copy scaled version of region into another bitmap.
   */
  template <class Pel> void CopyScaled(Bitmap<Pel>& dst,       int dstx0,int dsty0, int dw,int dh,
				       const Bitmap<Pel>& src, int srcx0,int srcy0, int sw,int sh);
  template <class Pel> void CopyScaled(Image<Pel>& dst,       int dstx0,int dsty0, int dw,int dh,
				       const Image<Pel>& src, int srcx0,int srcy0, int sw,int sh);


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

  template <class Pel> void CopyScaled(Bitmap<Pel>& dst,       int dstx0,int dsty0, int dw,int dh,
				       const Bitmap<Pel>& src, int srcx0,int srcy0, int sw,int sh)
  {
    if (src.IsEmpty()) return;

    const Pel*const* sp = src.AskFrame();
    Pel*const* dp = dst.AskFrame();

    Assert(dst.AskWidth()  >= dstx0+dw);
    Assert(dst.AskHeight() >= dsty0+dh);
    Assert(src.AskWidth()  >= srcx0+sw);
    Assert(src.AskHeight() >= srcy0+sh);

    for (int y=0;y<dh;y++)
      for (int x=0;x<dw;x++)
	dp[dsty0+y][dstx0+x] = sp[srcy0+y*sh/dh][srcx0+x*sw/dw];
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

}

#endif

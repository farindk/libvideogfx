#/*********************************************************************
  libvideogfx/graphics/draw/format.hh

  purpose:
    Functions for extending borders, change format of bitmaps...

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
    04/Jul/2000 - Dirk Farin - bitmap format conversion and helpers
 *********************************************************************/

#ifndef LIBVIDEOGFX_GRAPHICS_DRAW_BMFORMAT_HH
#define LIBVIDEOGFX_GRAPHICS_DRAW_BMFORMAT_HH

#include <libvideogfx/graphics/datatypes/bitmap.hh>
#include <libvideogfx/graphics/datatypes/image.hh>


// --------- bitmap type conversion ----------

template <class A,class B> void ConvertBitmap(Bitmap<B>& dst,const Bitmap<A>& src);
void PixelDifferenceToPixel(Bitmap<Pixel>& dst,const Bitmap<int16>& src);

template <class T> void Crop(Image<T>& dest, const Image<T>& src,
			     int left, int right, int top, int bottom);


// -----------------------------------------------------------------------------------------
// --------------------------------- implementation ----------------------------------------
// -----------------------------------------------------------------------------------------

template <class A,class B> void ConvertBitmap(Bitmap<B>& dst,const Bitmap<A>& src)
{
  int w=src.AskWidth();
  int h=src.AskHeight();

  dst.Create(w,h);

  const A*const* ap = src.AskFrame();
        B*const* bp = dst.AskFrame();

  for (int y=0;y<h;y++)
    for (int x=0;x<w;x++)
      bp[y][x] = static_cast<B>(ap[y][x]);
}


template <class T> void Crop(Image<T>& dest, const Image<T>& src,
			     int left, int right, int top, int bottom)
{
  ImageParam param = src.AskParam();

  param.width -= left+right;
  param.height -= top+bottom;

  dest.Create(param);

  Copy(dest, 0,0,
       src, left, top,
       param.width, param.height);
}

#endif

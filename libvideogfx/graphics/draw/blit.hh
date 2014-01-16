/*********************************************************************
  libvideogfx/graphics/draw/blit.hh

  purpose:
    Functions for copying images, extracting rectangles, ...

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
    29/Jan/2002 - Dirk Farin - first implementation
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

#ifndef LIBVIDEOGFX_GRAPHICS_DRAW_BLIT_HH
#define LIBVIDEOGFX_GRAPHICS_DRAW_BLIT_HH

#include <libvideogfx/graphics/datatypes/bitmap.hh>
#include <libvideogfx/graphics/datatypes/image.hh>
#include <libvideogfx/graphics/datatypes/primitives.hh>

namespace videogfx {

  /* Copy bitmap "src" into "dst". The destination bitmap will be created to have the
     same size as the source bitmap. If the source bitmap is empty, then the destination
     bitmap content is released.
     If the destination is shared, a new bitmap will be created.
  */
  template <class Pel> void CopyToNew(Bitmap<Pel>& dst,const Bitmap<Pel>& src);

  /* Logically the same as Image.Clone(). However, when the destination image is
     already of the right size and not shared, only the image content is copied
     without allocating new memory. */
  template <class Pel> void CopyToNew(Image<Pel>& dst,const Image<Pel>& src);

  /* Copy bitmap to destination. Destination bitmap must exist and will not
     be decoupled if it is shared. */
  template <class Pel> void Copy(Bitmap<Pel>& dst,const Bitmap<Pel>& src);
  template <class Pel> void Copy(Image<Pel>& dst,const Image<Pel>& src);

  /* Copy a region of one bitmap into another bitmap.
   */
  template <class Pel> void Copy(Bitmap<Pel>& dst,       int dstx0,int dsty0,
				 const Bitmap<Pel>& src, int srcx0,int srcy0, int w,int h);
  template <class Pel> void Copy(Image<Pel>& dst,       int dstx0,int dsty0,
				 const Image<Pel>& src, int srcx0,int srcy0, int w,int h);

  /* Copy the outer 'border' rows and columns of the src into the destination image.
     Useful for transfering the border when filters are not applied at the border area.
  */
  template <class Pel> void CopyBorder(Bitmap<Pel>& dst,const Bitmap<Pel>& src,int border);


  /* Copy the image content at the border lines into the border area.
   */
  template <class Pel> void ExtrudeIntoBorder(Bitmap<Pel>&);
  template <class Pel> void ExtrudeIntoBorder(Image<Pel>&);

  /* Fill the image border with constant value.
   */
  template <class Pel> void FillBorder(Bitmap<Pel>&, Pel value);
  template <class Pel> void FillBorder(Image<Pel>&, Color<Pel> value);


  // ------------------------------- implementation -----------------------------------

  template <class Pel> void CopyToNew(Bitmap<Pel>& dst,const Bitmap<Pel>& src)
  {
    if (src.IsEmpty())
      { dst.Release(); return; }

    const int w = src.AskWidth();
    const int h = src.AskHeight();

    dst.Create(w,h);

    assert(w == dst.AskWidth());
    assert(h == dst.AskHeight());

    const Pel*const* sp = src.AskFrame();
    Pel*const* dp = dst.AskFrame();

    for (int y=0;y<h;y++)
      memcpy(dp[y],sp[y],w*sizeof(Pel));
  }

  template <class Pel> void CopyToNew(Image<Pel>& dst,const Image<Pel>& src)
  {
    for (int i=0;i<4;i++)
      CopyToNew(dst.AskBitmap((BitmapChannel)i), src.AskBitmap((BitmapChannel)i));

    dst.SetParam(src.AskParam());
  }


  template <class Pel> void Copy(Bitmap<Pel>& dst,const Bitmap<Pel>& src)
  {
    if (src.IsEmpty())
      {
	dst.Release();
	return;
      }

    const int w = src.AskWidth();
    const int h = src.AskHeight();

    assert(w == dst.AskWidth());
    assert(h == dst.AskHeight());

    const Pel*const* sp = src.AskFrame();
    Pel*const* dp = dst.AskFrame();

    for (int y=0;y<h;y++)
      memcpy(dp[y],sp[y],w*sizeof(Pel));
  }

  template <class Pel> void Copy(Image<Pel>& dst,const Image<Pel>& src)
  {
    for (int i=0;i<4;i++)
      Copy(dst.AskBitmap((BitmapChannel)i), src.AskBitmap((BitmapChannel)i));

    dst.SetParam(src.AskParam());
  }

  template <class Pel> void Copy(Bitmap<Pel>& dst,       int dstx0,int dsty0,
				 const Bitmap<Pel>& src, int srcx0,int srcy0, int w,int h)
  {
    if (src.IsEmpty()) return;

    const Pel*const* sp = src.AskFrame();
    Pel*const* dp = dst.AskFrame();

    for (int y=0;y<h;y++)
      memcpy(&dp[y+dsty0][dstx0],&sp[srcy0+y][srcx0],w*sizeof(Pel));
  }

  template <class Pel> void Copy(Image<Pel>& dst,       int dstx0,int dsty0,
				 const Image<Pel>& src, int srcx0,int srcy0, int w,int h)
  {
    ImageParam param = src.AskParam();

    for (int i=0;i<4;i++)
      {
	BitmapChannel b = (BitmapChannel)i;
	Copy(dst.AskBitmap(b),  param.ChromaScaleH(b,dstx0),param.ChromaScaleV(b,dsty0),
	     src.AskBitmap(b),  param.ChromaScaleH(b,srcx0),param.ChromaScaleV(b,srcy0),
	     param.ChromaScaleH(b,w), param.ChromaScaleV(b,h));
      }
  }


  template <class Pel> void CopyBorder(Bitmap<Pel>& dst,const Bitmap<Pel>& src,int border)
  {
    int w=src.AskWidth(), h=src.AskHeight();
    const Pel*const* sp = src.AskFrame();
    Pel*const* dp = dst.AskFrame();

    for (int y=0;y<border;y++)
      for (int x=0;x<w;x++)
	{
	  dp[y][x] = sp[y][x];
	  dp[h-1-y][x] = sp[h-1-y][x];
	}

    for (int y=border;y<h-border;y++)
      for (int x=0;x<border;x++)
	{
	  dp[y][x] = sp[y][x];
	  dp[y][w-1-x] = sp[y][w-1-x];
	}
  }


  template <class Pel> void ExtrudeIntoBorder(Bitmap<Pel>& bm)
  {
    Pel*const* p = bm.AskFrame();
    int w = bm.AskWidth();
    int h = bm.AskHeight();
    int border = bm.AskBorder();
    int xo = bm.AskXOffset();
    int yo = bm.AskYOffset();

    for (int b=1;b<=border;b++)
      for (int x=-xo;x<w-xo;x++)
	{
	  p[-b-yo][x] = p[-yo][x];
	  p[h+b-1-yo][x] = p[h-1-yo][x];
	}

    for (int y=-border-yo;y<h+border-yo;y++)
      for (int b=1;b<=border;b++)
	{
	  p[y][-b-xo] = p[y][-xo];
	  p[y][w+b-1-xo] = p[y][w-1-xo];
	}
  }

  template <class Pel> void ExtrudeIntoBorder(Image<Pel>& img)
  {
    for (int i=0;i<4;i++)
      if (!img.AskBitmap(BitmapChannel(i)).IsEmpty())
	ExtrudeIntoBorder(img.AskBitmap(BitmapChannel(i)));
  }

  template <class Pel> void FillBorder(Bitmap<Pel>& bm, Pel value)
  {
    Pel*const* p = bm.AskFrame();
    int w = bm.AskWidth();
    int h = bm.AskHeight();
    int border = bm.AskBorder();
    int xo = bm.AskXOffset();
    int yo = bm.AskYOffset();

    for (int b=1;b<=border;b++)
      for (int x=-xo;x<w-xo;x++)
	{
	  p[-b-yo][x] = p[h+b-1-yo][x] = value;
	}

    for (int y=-border-yo;y<h+border-yo;y++)
      for (int b=1;b<=border;b++)
	{
	  p[y][-b-xo] = p[y][w+b-1-xo] = value;
	}
  }

  template <class Pel> void FillBorder(Image<Pel>& img, Color<Pel> value)
  {
    for (int i=0;i<4;i++)
      if (!img.AskBitmap(i).IsEmpty())
	FillBorder(img.AskBitmap(i), value.c[i]);
  }

}

#endif

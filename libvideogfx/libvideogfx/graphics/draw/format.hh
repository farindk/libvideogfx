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

#ifndef LIBVIDEOGFX_GRAPHICS_DRAW_BMFORMAT_HH
#define LIBVIDEOGFX_GRAPHICS_DRAW_BMFORMAT_HH

#include <libvideogfx/graphics/datatypes/bitmap.hh>
#include <libvideogfx/graphics/datatypes/image.hh>

namespace videogfx {

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

}

#endif

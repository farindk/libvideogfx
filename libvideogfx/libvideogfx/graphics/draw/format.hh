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
#include <libvideogfx/graphics/datatypes/primitives.hh>

namespace videogfx {

  /** Create a greyscale image from a bitmap. */
  template <class T> Image<T> MakeImage(Bitmap<T>&);

  // --------- bitmap type conversion ----------

  template <class A,class B> void ConvertBitmap(Bitmap<B>& dst,const Bitmap<A>& src);
  void PixelDifferenceToPixel(Bitmap<Pixel>& dst,const Bitmap<int16>& src);

  template <class T> void Crop(Image<T>& dest, const Image<T>& src,
			       int left, int right, int top, int bottom);

  /** Enlarge the bitmap by adding additional pixel rows/columns around it. The reference coordinate
      system stays the same. Hence, when adding to the left or the top, the origin moves into the image
      area and X/YOffset will be greater than 0. */
  template <class T> void Enlarge(Bitmap<T>& dest, const Bitmap<T>& src,
				  int addleft, int addtop, int addright, int addbottom,T bkgcolor);
  template <class T> void Enlarge(Image<T>& dest, const Image<T>& src,
				  int addleft, int addtop, int addright, int addbottom,Color<T> bkgcolor);


  // -----------------------------------------------------------------------------------------
  // --------------------------------- implementation ----------------------------------------
  // -----------------------------------------------------------------------------------------

  template <class T> Image<T> MakeImage(Bitmap<T>& bm)
  {
    Image<T> img;
    img.ReplaceBitmap(Bitmap_Y, bm);

    ImageParam spec;
    spec.width  = bm.AskWidth();
    spec.height = bm.AskHeight();
    spec.colorspace = Colorspace_Greyscale;
    img.SetParam(spec);

    return img;
  }

  template <class A,class B> void ConvertBitmap(Bitmap<B>& dst,const Bitmap<A>& src)
  {
    int w=src.AskWidth();
    int h=src.AskHeight();
    int x0=src.AskXOffset();
    int y0=src.AskYOffset();

    dst.Create(w,h);
    dst.MoveZero(x0,y0);

    const A*const* ap = src.AskFrame();
    B*const* bp = dst.AskFrame();

    for (int y=-y0;y<h-y0;y++)
      for (int x=-x0;x<w-x0;x++)
	bp[y][x] = static_cast<B>(ap[y][x]);
  }


  template <class T> void Crop(Image<T>& dest, const Image<T>& src,
			       int left, int right, int top, int bottom)
  {
    ImageParam param = src.AskParam();

    param.width -= left+right;
    param.height -= top+bottom;
    param.xoffset = param.yoffset = 0;

    dest.Create(param);

    Copy(dest, 0,0,
	 src, left, top,
	 param.width, param.height);
  }

  template <class T> void Enlarge(Bitmap<T>& dest, const Bitmap<T>& src,
				  int addleft, int addtop, int addright, int addbottom, T bkgcolor)
  {
    // create new bitmap and fill with background color

    dest.Create( src.AskWidth()+addleft+addright, src.AskHeight()+addtop+addbottom );
    Clear(dest,bkgcolor);

    dest.MoveZero(addleft + src.AskXOffset(),
		  addtop  + src.AskYOffset());

    Copy(dest, -src.AskXOffset(), -src.AskYOffset(),
	 src,  -src.AskXOffset(), -src.AskYOffset(), src.AskWidth(), src.AskHeight());
  }


  template <class T> void Enlarge(Image<T>& dest, const Image<T>& src,
				  int addleft, int addtop, int addright, int addbottom,Color<T> bkgcolor)
  {
    ImageParam param = src.AskParam();
    param.width  += addleft+addright;
    param.height += addtop +addbottom;
    param.xoffset += addleft;
    param.yoffset += addtop;

    dest.Create(param);
    Clear(dest,bkgcolor);

    Copy(dest, -src.AskXOffset(), -src.AskYOffset(),
	 src,  -src.AskXOffset(), -src.AskYOffset(), src.AskWidth(), src.AskHeight());
  }
}

#endif

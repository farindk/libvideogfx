/*********************************************************************
  grey2rgb_scalar.hh

  purpose:
    Scalar routines to convert greyscale images to RGB raw.

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
   16/Jul/2000 - Dirk Farin - first implementation
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

#ifndef LIBVIDEOGFX_GRAPHICS_INTERNAL_GREY2RGB_SCALAR_HH
#define LIBVIDEOGFX_GRAPHICS_INTERNAL_GREY2RGB_SCALAR_HH

#include <libvideogfx/graphics/color/img2raw.hh>
#include "libvideogfx/graphics/color/internal_img2raw.hh"

namespace videogfx {

  /* Convert greyscale to RGB components in 32bit entities in arbitrary order.
   */
  class i2r_grey_32bit : public Image2RawRGB_Transform
  {
  public:
    virtual ~i2r_grey_32bit() { }
    virtual void Transform(const Image<Pixel>&,uint8* mem,int firstline,int lastline);

    static bool s_CanConvert(const Image<Pixel>&,const RawRGBImageSpec&);
    virtual bool CanConvert(const Image<Pixel>& i,const RawRGBImageSpec& s) { return s_CanConvert(i,s); }

    virtual const char* TransformationName() { return "scalar grey to 32bit RGB"; }
  };


  class i2r_grey_16bit : public Image2RawRGB_Transform
  {
  public:
    virtual ~i2r_grey_16bit() { }
    virtual void Transform(const Image<Pixel>&,uint8* mem,int firstline,int lastline);

    static bool s_CanConvert(const Image<Pixel>&,const RawRGBImageSpec&);
    virtual bool CanConvert(const Image<Pixel>& i,const RawRGBImageSpec& s) { return s_CanConvert(i,s); }

    virtual const char* TransformationName() { return "scalar grey to 16bit RGB"; }
  };



  class i2r_grey_4bit : public Image2RawRGB_Transform
  {
  public:
    virtual ~i2r_grey_4bit() { }
    virtual void Transform(const Image<Pixel>&,uint8* mem,int firstline,int lastline);

    static bool s_CanConvert(const Image<Pixel>&,const RawRGBImageSpec&);
    virtual bool CanConvert(const Image<Pixel>& i,const RawRGBImageSpec& s) { return s_CanConvert(i,s); }

    virtual const char* TransformationName() { return "scalar grey to 4bit gray-scale"; }
  };

}

#endif

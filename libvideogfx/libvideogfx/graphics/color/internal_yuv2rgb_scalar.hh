/*********************************************************************
  libvideogfx/graphics/color/internal_yuv2rgb_scalar.hh

  purpose:
    Transform YUV data into 16bit true color RGB raw data.
    Every bit organization in a 16bit field and endianess
    translation is supported.

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
    08/Aug/1999 - Dirk Farin - code imported from DVDview and
                               slightly modified
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

#ifndef LIBVIDEOGFX_GRAPHICS_COLOR_INTERNAL_YUV2RGB_SCALAR_HH
#define LIBVIDEOGFX_GRAPHICS_COLOR_INTERNAL_YUV2RGB_SCALAR_HH

#include "libvideogfx/graphics/color/img2raw.hh"
#include "libvideogfx/graphics/color/internal_img2raw.hh"

namespace videogfx {

  class i2r_yuv_16bit : public Image2RawRGB_Transform
  {
  public:
    virtual ~i2r_yuv_16bit() { }

    virtual void Transform(const Image<Pixel>&,uint8* mem,int firstline,int lastline);

    static bool s_CanConvert(const Image<Pixel>&,const RawRGBImageSpec&);
    virtual bool CanConvert(const Image<Pixel>& i,const RawRGBImageSpec& s) { return s_CanConvert(i,s); }

    virtual const char* TransformationName() { return "scalar YUV to 16bit RGB"; }

  private:
  };


  /* Convert YUV 4:2:0 and place RGB components in 32bit entities in arbitrary order.
   */
  class i2r_yuv_32bit : public Image2RawRGB_Transform
  {
  public:
    virtual ~i2r_yuv_32bit() { }
    virtual void Transform(const Image<Pixel>&,uint8* mem,int firstline,int lastline);

    static bool s_CanConvert(const Image<Pixel>&,const RawRGBImageSpec&);
    virtual bool CanConvert(const Image<Pixel>& i,const RawRGBImageSpec& s) { return s_CanConvert(i,s); }

    virtual const char* TransformationName() { return "scalar YUV to 32bit RGB"; }
  };



  /* Convert YUV 4:2:2 and place RGB components in 32bit entities in arbitrary order.
   */
  class i2r_yuv422_32bit : public Image2RawRGB_Transform
  {
  public:
    virtual ~i2r_yuv422_32bit() { }
    virtual void Transform(const Image<Pixel>&,uint8* mem,int firstline,int lastline);

    static bool s_CanConvert(const Image<Pixel>&,const RawRGBImageSpec&);
    virtual bool CanConvert(const Image<Pixel>& i,const RawRGBImageSpec& s) { return s_CanConvert(i,s); }

    virtual const char* TransformationName() { return "scalar 4:2:2-YUV to 32bit RGB"; }
  };



  /* Convert YUV 4:4:4 and place RGB components in 32bit entities in arbitrary order.
   */
  class i2r_yuv444_32bit : public Image2RawRGB_Transform
  {
  public:
    virtual ~i2r_yuv444_32bit() { }
    virtual void Transform(const Image<Pixel>&,uint8* mem,int firstline,int lastline);

    static bool s_CanConvert(const Image<Pixel>&,const RawRGBImageSpec&);
    virtual bool CanConvert(const Image<Pixel>& i,const RawRGBImageSpec& s) { return s_CanConvert(i,s); }

    virtual const char* TransformationName() { return "scalar 4:4:4-YUV to 32bit RGB"; }
  };

}

#endif

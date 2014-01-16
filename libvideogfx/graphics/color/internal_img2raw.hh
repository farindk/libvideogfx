/*********************************************************************
  libvideogfx/graphics/color/internal_img2raw.hh

  purpose:

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
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

#ifndef LIBVIDEOGFX_GRAPHICS_COLOR_INTERNAL_IMG2RAW_HH
#define LIBVIDEOGFX_GRAPHICS_COLOR_INTERNAL_IMG2RAW_HH

#include <libvideogfx/graphics/datatypes/image.hh>

namespace videogfx {

  class Image2RawRGB_Transform
  {
  public:
    virtual ~Image2RawRGB_Transform() { }
    virtual bool CanConvert(const Image<Pixel>&,const RawRGBImageSpec&) = 0;
    void SetSpec(const RawRGBImageSpec& spec) { d_spec=spec; Init(); }
    virtual void Transform(const Image<Pixel>&,uint8* mem,int firstline,int lastline) { assert(0); }
    virtual void Init() { }

    virtual const char* TransformationName() = 0;

  protected:
    RawRGBImageSpec d_spec;
  };

}

#endif

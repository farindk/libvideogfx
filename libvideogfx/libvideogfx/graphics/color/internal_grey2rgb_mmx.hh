/*********************************************************************
  internal_grey2rgb_mmx.hh

  purpose:
    MMX routines for greyscale images to RGB raw conversion.

  notes:

  to do:

  author(s):
   - Dirk Farin, Kapellenweg 15, 72070 Tuebingen, Germany,
     email: farindk@trick.informatik.uni-stuttgart.de

  modifications:
   16/Jul/2000 - Dirk Farin - first implementation
 *********************************************************************/

#ifndef LIBVIDEOGFX_GRAPHICS_COLOR_INTERNAL_GREY2RGB_MMX_HH
#define LIBVIDEOGFX_GRAPHICS_COLOR_INTERNAL_GREY2RGB_MMX_HH

#include <libvideogfx/graphics/color/img2raw.hh>


class i2r_grey_16bit_mmx : public Image2RawRGB_Transform
{
public:
  virtual ~i2r_grey_16bit_mmx() { }

  virtual void Transform(const Image<Pixel>&,uint8* mem,int firstline,int lastline);

  static bool s_CanConvert(const Image<Pixel>&,const RawRGBImageSpec&);
  virtual bool CanConvert(const Image<Pixel>& i,const RawRGBImageSpec& s) { return s_CanConvert(i,s); }

  virtual const char* TransformationName() { return "grey to 16bit RGB, MMX accelerated"; }
};


class i2r_grey_32bit_mmx : public Image2RawRGB_Transform
{
public:
  virtual ~i2r_grey_32bit_mmx() { }

  virtual void Transform(const Image<Pixel>&,uint8* mem,int firstline,int lastline);

  static bool s_CanConvert(const Image<Pixel>&,const RawRGBImageSpec&);
  virtual bool CanConvert(const Image<Pixel>& i,const RawRGBImageSpec& s) { return s_CanConvert(i,s); }

  virtual const char* TransformationName() { return "grey to 32bit RGB, MMX accelerated"; }
};

#endif

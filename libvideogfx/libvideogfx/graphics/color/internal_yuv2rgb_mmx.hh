/*********************************************************************
  yuv2rgb_mmx.hh

  purpose:

  notes:

  to do:

  author(s):
   - Dirk Farin, Kapellenweg 15, 72070 Tuebingen, Germany,
     email: farindk@trick.informatik.uni-stuttgart.de

  modifications:
   01/Feb/99 - Dirk Farin
     - interface definition
 *********************************************************************/

#ifndef LIBVIDEOGFX_GRAPHICS_COLOR_INTERNAL_YUV2RGB_MMX_HH
#define LIBVIDEOGFX_GRAPHICS_COLOR_INTERNAL_YUV2RGB_MMX_HH

#include <libvideogfx/graphics/color/img2raw.hh>


class i2r_16bit_mmx : public Image2RawRGB_Transform
{
public:
  virtual ~i2r_16bit_mmx() { }

  virtual void Transform(const Image<Pixel>&,uint8* mem,int firstline,int lastline);

  static bool s_CanConvert(const Image<Pixel>&,const RawRGBImageSpec&);
  virtual bool CanConvert(const Image<Pixel>& i,const RawRGBImageSpec& s) { return s_CanConvert(i,s); }

  virtual const char* TransformationName() { return "YUV to 16bit RGB, MMX accelerated"; }
};


class i2r_32bit_BGR_mmx : public Image2RawRGB_Transform
{
public:
  virtual ~i2r_32bit_BGR_mmx() { }

  virtual void Transform(const Image<Pixel>&,uint8* mem,int firstline,int lastline);

  static bool s_CanConvert(const Image<Pixel>&,const RawRGBImageSpec&);
  virtual bool CanConvert(const Image<Pixel>& i,const RawRGBImageSpec& s) { return s_CanConvert(i,s); }

  virtual const char* TransformationName() { return "YUV to 32bit RGB, (BGR format), MMX accelerated"; }
};

#endif

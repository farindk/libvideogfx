/*********************************************************************
  libvideogfx/graphics/color/internal_rgb2rgb_scalar.hh

  purpose:
    Transform YUV data into 16bit true color RGB raw data.
    Every bit organization in a 16bit field and endianess
    translation is supported.

  notes:

  to do:

  author(s):
   - Dirk Farin, farin@ti.uni-mannheim.de
     University Mannheim, Dept. Circuitry and Simulation
     B 6,26 EG, room 0.10 / D-68131 Mannheim / Germany

  modifications:
    08/Aug/1999 - Dirk Farin - code imported from DVDview and
                               slightly modified
 *********************************************************************/

#ifndef LIBVIDEOGFX_GRAPHICS_COLOR_INTERNAL_RGB2RGB_SCALAR_HH
#define LIBVIDEOGFX_GRAPHICS_COLOR_INTERNAL_RGB2RGB_SCALAR_HH

#include <libvideogfx/graphics/color/img2raw.hh>

namespace videogfx {

  /* Place RGB components in 32bit entities in arbitrary order.
     For the special cases that can be handled by the classes i2r_xrgb and i2r_xbgr you
     should use them as they are a bit faster.
  */
  class i2r_32bit : public Image2RawRGB_Transform
  {
  public:
    virtual ~i2r_32bit() { }
    virtual void Transform(const Image<Pixel>&,uint8* mem,int firstline,int lastline);

    static bool s_CanConvert(const Image<Pixel>&,const RawRGBImageSpec&);
    virtual bool CanConvert(const Image<Pixel>& i,const RawRGBImageSpec& s) { return s_CanConvert(i,s); }

    virtual const char* TransformationName() { return "scalar 32bit RGB"; }
  };


  /* Place RGB components in 16bit entities in arbitrary order.
     For the special cases that can be handled by the classes i2r_xrgb and i2r_xbgr you
     should use them as they are a bit faster.
  */
  class i2r_16bit : public Image2RawRGB_Transform
  {
  public:
    virtual ~i2r_16bit() { }
    virtual void Transform(const Image<Pixel>&,uint8* mem,int firstline,int lastline);

    static bool s_CanConvert(const Image<Pixel>&,const RawRGBImageSpec&);
    virtual bool CanConvert(const Image<Pixel>& i,const RawRGBImageSpec& s) { return s_CanConvert(i,s); }

    virtual const char* TransformationName() { return "scalar 16bit RGB"; }
  };

}

#endif

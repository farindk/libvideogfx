/*********************************************************************
  grey2rgb_scalar.hh

  purpose:
    Scalar routines to convert greyscale images to RGB raw.

  notes:

  to do:

  author(s):
   - Dirk Farin, Kapellenweg 15, 72070 Tuebingen, Germany,
     email: farindk@trick.informatik.uni-stuttgart.de

  modifications:
   16/Jul/2000 - Dirk Farin - first implementation
 *********************************************************************/

#ifndef DISPLAY_GREY2RGB_SCALAR_HH
#define DISPLAY_GREY2RGB_SCALAR_HH

#include <libvideogfx/graphics/color/img2raw.hh>


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

#endif

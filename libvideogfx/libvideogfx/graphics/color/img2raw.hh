/*********************************************************************
  libvideogfx/graphics/color/img2raw.hh

  purpose:
    Classes that allow image data stored in Image<Pixel> objects
    in either RGB or YUV format to be converted to a format the
    hardware directly understands. This can be (for example) the
    data ordering required for X11 XImages. Only RGB outputs
    are supported so far.

    Special hardware oriented convertion routines should be
    integrated here. The appropriate transformation is selected
    automagically.

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
    18/Jul/2000 - Dirk Farin - complete reimplementation
    29/Jul/1999 - Dirk Farin - first implementation
 *********************************************************************/

#ifndef LIBVIDEOGFX_GRAPHICS_LOWLEVEL_IMG2RAW_HH
#define LIBVIDEOGFX_GRAPHICS_LOWLEVEL_IMG2RAW_HH

#include <libvideogfx/graphics/datatypes/image.hh>


/* Image manipulation operators that are executed on the fly while converting to raw image data.
 */
struct RawImageOperator
{
  RawImageOperator()
    : dest_width(0), dest_height(0),
      upscale_factor(0), downscale_factor(0),
      greyscale_only(false)
  { }

  int    dest_width,dest_height;

  int    upscale_factor;
  int    downscale_factor;

  bool   greyscale_only;
};


/** RGB-output data format specification
 */
struct RawRGBImageSpec : public RawImageOperator
{
  int  bytes_per_line;
  int  bits_per_pixel;
  bool little_endian;

  int r_mask,r_bits,r_shift;
  int g_mask,g_bits,g_shift;
  int b_mask,b_bits,b_shift;

  // example:   mask: 0xFF0000, bits: 8, shift = 16

  void SetRGBMasks(uint32 r,uint32 g,uint32 b);

  void Debug_ShowParam() const;
};


/** Image to raw-format converter class.
 */
class Image2RawRGB
{
public:
   Image2RawRGB();
  ~Image2RawRGB();

  void SetOutputSpec(const RawRGBImageSpec& spec) { d_spec=spec; }

  void Transform(const Image<Pixel>&,uint8* mem,int firstline=0,int lastline=-1);

private:
  RawRGBImageSpec  d_spec;

  class Image2RawRGB_Transform* d_last_rgb_transform;
};





// --------------------- following definitions are only for internal use --------------------------


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

#endif

/*
 *  img2raw.cc
 */

#include "config.h"
#include "libvideogfx/error.hh"
#include "libvideogfx/macros.hh"
#include "libvideogfx/arch/cpu.hh"

#include <iostream>
using namespace std;

#include "libvideogfx/graphics/color/img2raw.hh"

#include "internal_grey2rgb_scalar.hh"
#include "internal_yuv2rgb_scalar.hh"
#include "internal_rgb2rgb_scalar.hh"

#if ENABLE_MMX
#include "internal_yuv2rgb_mmx.hh"
#include "internal_grey2rgb_mmx.hh"
#endif

#include "libvideogfx/utility/bitmanip.hh"


namespace videogfx {

#define SHOWLINE(x,y) cout << #x ": " << x << y


  void RawRGBImageSpec::SetRGBMasks(uint32 r,uint32 g,uint32 b)
  {
    CalcBitsShift(r_mask=r,r_bits,r_shift);
    CalcBitsShift(g_mask=g,g_bits,g_shift);
    CalcBitsShift(b_mask=b,b_bits,b_shift);
  }


  void RawRGBImageSpec::Debug_ShowParam() const
  {
    SHOWLINE(bytes_per_line,endl);
    SHOWLINE(bits_per_pixel,endl);
    cout << "little_endian: " << (little_endian ? "little" : "big") << endl;
    SHOWLINE(r_mask," "); SHOWLINE(r_bits," "); SHOWLINE(r_shift,endl);
    SHOWLINE(g_mask," "); SHOWLINE(g_bits," "); SHOWLINE(g_shift,endl);
    SHOWLINE(b_mask," "); SHOWLINE(b_bits," "); SHOWLINE(b_shift,endl);

    if (dest_width) { cout << "new size: " << dest_width << "x" << dest_height << endl; }
    if (upscale_factor) { cout << "upscale-factor: " << upscale_factor << endl; }
    if (downscale_factor) { cout << "downscale-factor: " << downscale_factor << endl; }
    if (greyscale_only) cout << "greyscale only\n";
  }


  // --------------------------------------------------------------------------------------------


  Image2RawRGB::Image2RawRGB()
    : d_last_rgb_transform(NULL)
  {
  }


  Image2RawRGB::~Image2RawRGB()
  {
    if (d_last_rgb_transform) delete d_last_rgb_transform;
  }


  void Image2RawRGB::Transform(const Image<Pixel>& img,uint8* mem,int firstline,int lastline)
  {
    Image2RawRGB_Transform* t=NULL;

    if (d_last_rgb_transform)
      {
	if (d_last_rgb_transform->CanConvert(img,d_spec))
	  {
	    // cout << "reusing old transform\n";
	    t = d_last_rgb_transform;
	    goto found2;
	  }
	else
	  {
	    delete d_last_rgb_transform;
	    d_last_rgb_transform=NULL;
	  }
      }

    //cout << "searching for transform to match:\n";
    //d_spec.Debug_ShowParam();

    {
      CPU_Capabilities* cpu = CPU_Capabilities::AskCapabilities();

      // --- YUV -> raw RGB

#if ENABLE_MMX
      if (cpu->HasMMX())
	{
	  if (i2r_32bit_BGR_mmx ::s_CanConvert(img,d_spec)) { t = new i2r_32bit_BGR_mmx;  goto found; }
	  if (i2r_16bit_mmx     ::s_CanConvert(img,d_spec)) { t = new i2r_16bit_mmx; goto found; }
	  if (i2r_grey_32bit_mmx::s_CanConvert(img,d_spec)) { t = new i2r_grey_32bit_mmx; goto found; }
	  if (i2r_grey_16bit_mmx::s_CanConvert(img,d_spec)) { t = new i2r_grey_16bit_mmx; goto found; }
	}
#endif

      if (i2r_yuv_16bit   ::s_CanConvert(img,d_spec)) { t = new i2r_yuv_16bit;    goto found; }
      if (i2r_yuv_32bit   ::s_CanConvert(img,d_spec)) { t = new i2r_yuv_32bit;    goto found; }
      if (i2r_yuv422_32bit::s_CanConvert(img,d_spec)) { t = new i2r_yuv422_32bit; goto found; }
      if (i2r_yuv444_32bit::s_CanConvert(img,d_spec)) { t = new i2r_yuv444_32bit; goto found; }

      // --- grey -> raw RGB

      if (i2r_grey_32bit  ::s_CanConvert(img,d_spec)) { t = new i2r_grey_32bit;   goto found; }
      if (i2r_grey_16bit  ::s_CanConvert(img,d_spec)) { t = new i2r_grey_16bit;   goto found; }
      if (i2r_grey_4bit   ::s_CanConvert(img,d_spec)) { t = new i2r_grey_4bit;    goto found; }

      // --- RGB -> raw RGB

      if (i2r_32bit::s_CanConvert(img,d_spec)) { t = new i2r_32bit; goto found; }
      if (i2r_16bit::s_CanConvert(img,d_spec)) { t = new i2r_16bit; goto found; }
    }

    throw Excpt_Text(ErrSev_Error,"no suitable colorspace transformation found!");
    assert(0); // no transformation found;
    return;

  found:
    //cout << "RGB transformation used: " << t->TransformationName() << endl;
  found2:
    t->SetSpec(d_spec);

    if (lastline==-1)
      {
	lastline=img.AskHeight()-1;
      }
    t->Transform(img,mem,firstline,lastline);

    d_last_rgb_transform = t;
  }

}


#include "libvideogfx/graphics/color/internal_rgb2rgb_scalar.hh"
#include "libvideogfx/macros.hh"

namespace videogfx {

  bool i2r_32bit::s_CanConvert(const Image<Pixel>& img,const RawRGBImageSpec& spec)
  {
    if (spec.dest_width || spec.upscale_factor || spec.downscale_factor) return false;
    if (spec.bits_per_pixel != 32 && spec.bits_per_pixel != 24) return false;
    if (spec.r_bits != 8 || spec.g_bits != 8 || spec.b_bits != 8) return false;
    if (spec.r_shift%8   || spec.g_shift%8   || spec.b_shift%8)   return false;

    if (img.AskParam().colorspace != Colorspace_RGB) return false;

    return true;
  }

  void i2r_32bit::Transform(const Image<Pixel>& img,uint8* mem,int firstline,int lastline)
  {
    int rpos,gpos,bpos;

    const int bytes_per_pixel = d_spec.bits_per_pixel/8;
    const int lastidx = bytes_per_pixel-1;

    rpos = lastidx-d_spec.r_shift/8;
    gpos = lastidx-d_spec.g_shift/8;
    bpos = lastidx-d_spec.b_shift/8;

    if (d_spec.little_endian)
      {
	rpos = lastidx-rpos;
	gpos = lastidx-gpos;
	bpos = lastidx-bpos;
      }

    ImageParam param = img.AskParam();

    const Pixel*const* r = img.AskFrameR();
    const Pixel*const* g = img.AskFrameG();
    const Pixel*const* b = img.AskFrameB();

    for (int y=firstline;y<=lastline;y++)
      {
	uint8* p = mem+y*d_spec.bytes_per_line;

	int yy = y-firstline;

	for (int x=0;x<param.width;x++)
	  {
	    p[rpos] = r[yy][x];
	    p[gpos] = g[yy][x];
	    p[bpos] = b[yy][x];
	    p+=bytes_per_pixel;
	  }
      }
  }

  // --------------------------------------------------------------------------------------------

  bool i2r_16bit::s_CanConvert(const Image<Pixel>& img,const RawRGBImageSpec& spec)
  {
    int rshift = spec.r_shift+(spec.r_bits-8);
    int gshift = spec.g_shift+(spec.g_bits-8);
    int bshift = spec.b_shift+(spec.b_bits-8);

    if (rshift<0 || gshift<0) return false;
    if (bshift>0) return false;

    if (spec.dest_width || spec.upscale_factor || spec.downscale_factor) return false;
    if (spec.bits_per_pixel != 16) return false;

    if (img.AskParam().colorspace != Colorspace_RGB) return false;

    return true;
  }

  void i2r_16bit::Transform(const Image<Pixel>& img,uint8* mem,int firstline,int lastline)
  {
    ImageParam param = img.AskParam();

    const Pixel*const* r = img.AskFrameR();
    const Pixel*const* g = img.AskFrameG();
    const Pixel*const* b = img.AskFrameB();

    int rshift = d_spec.r_shift+(d_spec.r_bits-8);
    int gshift = d_spec.g_shift+(d_spec.g_bits-8);
    int bshift = d_spec.b_shift+(d_spec.b_bits-8);

    bshift = -bshift;

    for (int y=firstline;y<=lastline;y++)
      {
	uint16* p = (uint16*)(mem+y*d_spec.bytes_per_line);

	int yy = y-firstline;

	for (int x=0;x<param.width;x++)
	  {
	    uint16 val;

	    val  = (r[yy][x]<<rshift) & d_spec.r_mask;
	    val |= (g[yy][x]<<gshift) & d_spec.g_mask;
	    val |= (b[yy][x]>>bshift); // & d_spec.b_mask;

	    *p++ = (d_spec.little_endian ? ToLittleEndian((uint16)val) : ToBigEndian((uint16)val));
	  }
      }
  }

}

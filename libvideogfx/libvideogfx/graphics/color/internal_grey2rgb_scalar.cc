
#include "libvideogfx/graphics/color/internal_grey2rgb_scalar.hh"
#include "libvideogfx/macros.hh"


bool i2r_grey_32bit::s_CanConvert(const Image<Pixel>& img,const RawRGBImageSpec& spec)
{
  if (spec.dest_width || spec.upscale_factor || spec.downscale_factor) return false;
  if (spec.bits_per_pixel != 32) return false;
  if (spec.r_bits != 8 || spec.g_bits != 8 || spec.b_bits != 8) return false;
  if (spec.r_shift%8   || spec.g_shift%8   || spec.b_shift%8)   return false;

  ImageParam param = img.AskParam();

  if (param.colorspace == Colorspace_Greyscale) return true;
  if (param.colorspace == Colorspace_YUV && spec.greyscale_only) return true;

  return false;
}


void i2r_grey_32bit::Transform(const Image<Pixel>& img,uint8* mem,int firstline,int lastline)
{
  ImageParam param = img.AskParam();

  const Pixel*const * pix_y = img.AskFrameY();

  for (int y=firstline;y<=lastline;y++)
    {
      uint32* membuf = (uint32*)(mem + d_spec.bytes_per_line*(y-firstline));

      for (int x=0;x<param.width;x++)
        {
	  uint32 val = pix_y[y][x];
	  val |= val<<16;
	  val |= val<< 8;

          *membuf++ = val;
        }
    }
}


// --------------------------------------------------------------------------------------------


bool i2r_grey_16bit::s_CanConvert(const Image<Pixel>& img,const RawRGBImageSpec& spec)
{
  if (spec.dest_width || spec.upscale_factor || spec.downscale_factor) return false;
  if (spec.bits_per_pixel != 16) return false;

  ImageParam param = img.AskParam();

  if (param.colorspace == Colorspace_Greyscale) return true;
  if (param.colorspace == Colorspace_YUV && spec.greyscale_only) return true;

  return false;
}


void i2r_grey_16bit::Transform(const Image<Pixel>& img,uint8* mem,int firstline,int lastline)
{
  ImageParam param = img.AskParam();

  const Pixel*const * pix_y = img.AskFrameY();

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

	  val  = (pix_y[yy][x]<<rshift) & d_spec.r_mask;
	  val |= (pix_y[yy][x]<<gshift) & d_spec.g_mask;
	  val |= (pix_y[yy][x]>>bshift); // & d_spec.b_mask;

	  *p++ = (d_spec.little_endian ? ToLittleEndian((uint16)val) : ToBigEndian((uint16)val));
	}
    }
}


// --------------------------------------------------------------------------------------------


bool i2r_grey_4bit::s_CanConvert(const Image<Pixel>& img,const RawRGBImageSpec& spec)
{
  if (spec.dest_width || spec.upscale_factor || spec.downscale_factor) return false;
  if (spec.bits_per_pixel != 4) return false;

  ImageParam param = img.AskParam();

  if (param.colorspace == Colorspace_Greyscale) return true;
  if (param.colorspace == Colorspace_YUV && spec.greyscale_only) return true;

  return false;
}


void i2r_grey_4bit::Transform(const Image<Pixel>& img,uint8* mem,int firstline,int lastline)
{
  ImageParam param = img.AskParam();

  const Pixel*const * pix_y = img.AskFrameY();

  for (int y=firstline;y<=lastline;y++)
    {
      uint8* membuf = (uint8*)(mem + d_spec.bytes_per_line*(y-firstline));

      for (int x=0;x<param.width;x+=2)
        {
          uint8 val = pix_y[y][x] & 0xF0;
          val |= pix_y[y][x+1] >> 4;

          *membuf++ = val;
        }
    }
}


/*
 *  internal_grey2rgb_mmx.cc
 */

#include "libvideogfx/graphics/color/internal_grey2rgb_mmx.hh"


bool i2r_grey_16bit_mmx::s_CanConvert(const Image<Pixel>& img,const RawRGBImageSpec& spec)
{
  if (spec.dest_width || spec.upscale_factor || spec.downscale_factor) return false;
  if (spec.bits_per_pixel != 16) return false;
  if (!spec.little_endian) return false;

  ImageParam param = img.AskParam();

  if ((param.width % 8)!=0) return false;

  int w = (param.width+7) & ~7;
  if (spec.bytes_per_line < w) return false;

  if (param.colorspace == Colorspace_Greyscale) return true;
  if (param.colorspace == Colorspace_YUV && spec.greyscale_only) return true;

  return false;
}

void i2r_grey_16bit_mmx::Transform(const Image<Pixel>& img,uint8* mem,int firstline,int lastline)
{
  uint64 constants[6];

  constants[0] = 16-d_spec.r_bits-d_spec.r_shift;
  constants[1] = 16-d_spec.g_bits-d_spec.g_shift;
  constants[2] = 16-d_spec.b_bits-d_spec.b_shift;

  assert(constants[0]==0); // Assume that R is aligned to the very left.

  uint64 mask;
  mask = d_spec.r_mask; constants[3] = mask | (mask<<16) | (mask<<32) | (mask<<48);
  mask = d_spec.g_mask; constants[4] = mask | (mask<<16) | (mask<<32) | (mask<<48);
  mask = d_spec.b_mask; constants[5] = mask | (mask<<16) | (mask<<32) | (mask<<48);


  ImageParam param = img.AskParam();

  //assert(param.nocolor);

  const Pixel*const* pix_y  = img.AskFrameY();

  const int h = param.height;
  const int w = param.width;

        uint8* dp;
  const uint8* sp;

  uint8* dpstart=mem;

  assert(w%8 == 0);

  for (int y=firstline;y<=lastline;y++)
    {
      sp = pix_y[y];

      dp=dpstart;
      dpstart+=d_spec.bytes_per_line;

      for (int x=0;x<w;x+=8)
        {
          __asm__ __volatile__
            (
	     "movq        (%0),%%mm1\n\t"   // 8 Pixel nach mm1  (ABCDEFGH)
	     " pxor       %%mm2,%%mm2\n\t"
	     "movq        %%mm1,%%mm4\n\t"  //         und nach mm4
	     " punpckhbw  %%mm1,%%mm2\n\t"  // A0B0C0D0 in mm2,mm4,mm6
	     "pxor        %%mm3,%%mm3\n\t"
	     " punpcklbw  %%mm4,%%mm3\n\t"  // E0F0G0H0 in mm3,mm5,mm7
	     "movq        %%mm2,%%mm4\n\t"
	     " movq       %%mm3,%%mm5\n\t"
             "psrlq       1*8(%2),%%mm5\n\t" // G nach rechts schieben
	     " movq       %%mm2,%%mm6\n\t"
             "psrlq       1*8(%2),%%mm4\n\t" // G nach rechts schieben
	     " movq       %%mm3,%%mm7\n\t"
             "psrlq       2*8(%2),%%mm6\n\t" // B nach rechts schieben
	     " pand       3*8(%2),%%mm2\n\t"
             "psrlq       2*8(%2),%%mm7\n\t" // B nach rechts schieben
	     " pand       3*8(%2),%%mm3\n\t"
	     "pand        4*8(%2),%%mm4\n\t"
	     " pand       4*8(%2),%%mm5\n\t"
	     "por         %%mm4,%%mm2\n\t"
	     " pand       5*8(%2),%%mm6\n\t"
	     "por         %%mm5,%%mm3\n\t"
	     " pand       5*8(%2),%%mm7\n\t"
	     "por         %%mm6,%%mm2\n\t"
	     " movq       %%mm2,8(%1)\n\t"
	     "por         %%mm7,%%mm3\n\t"
	     " movq       %%mm3,(%1)\n\t"
             : : "r" (sp), "r" (dp), "r" (&constants[0])
             );

	  sp += 8;
	  dp += 16;
        }
    }

  __asm__
    (
     "emms\n\t"
     );
}


// ----------------------


bool i2r_grey_32bit_mmx::s_CanConvert(const Image<Pixel>& img,const RawRGBImageSpec& spec)
{
  if (spec.dest_width || spec.upscale_factor || spec.downscale_factor) return false;
  if (spec.bits_per_pixel != 32) return false;
  if (spec.r_bits != 8 || spec.g_bits != 8 || spec.b_bits != 8) return false;
  if ((spec.r_shift%8) || (spec.g_shift%8) || (spec.b_shift%8)) return false;

  ImageParam param = img.AskParam();

  int w = (param.width+7) & ~7;

  if (spec.bytes_per_line < 4*w) return false;

  if (param.colorspace == Colorspace_Greyscale) return true;
  if (param.colorspace == Colorspace_YUV && spec.greyscale_only) return true;

  return false;
}


void i2r_grey_32bit_mmx::Transform(const Image<Pixel>& img,uint8* mem,int firstline,int lastline)
{
  ImageParam param = img.AskParam();

  const Pixel*const* pix_y  = img.AskFrameY();

  const int h = param.height;
  const int w = param.width;

        uint8* dp;
  const uint8* sp;

  uint8* dpstart=mem;

  //assert(w%8 == 0);

  for (int y=firstline;y<=lastline;y++)
    {
      sp = pix_y[y];

      dp=dpstart;
      dpstart+=d_spec.bytes_per_line;

      for (int x=0;x<w;x+=8)
        {
          __asm__ __volatile__
            (
	     "movq        (%0),%%mm0\n\t"   // 8 Pixel nach mm0  (ABCDEFGH)
	     "movq        %%mm0,%%mm4\n\t"  //         und nach mm4
	     "punpckhbw   %%mm0,%%mm0\n\t"  // AABBCCDD in mm0,mm2
	     " punpcklbw  %%mm4,%%mm4\n\t"  // EEFFGGHH in mm4,mm6
	     "movq        %%mm0,%%mm2\n\t"  //
             " movq       %%mm4,%%mm6\n\t"  //
	     "punpckhbw   %%mm0,%%mm0\n\t"  // AAAABBBB in mm0
	     " punpckhbw  %%mm4,%%mm4\n\t"  // EEEEFFFF in mm4
	     "movq        %%mm4,8(%1)\n\t"  // EF->mem
             " punpcklbw  %%mm2,%%mm2\n\t"  // CCCCDDDD in mm2
	     "movq        %%mm0,24(%1)\n\t" // AB->mem
	     " punpcklbw  %%mm6,%%mm6\n\t"  // GGGGHHHH in mm6
	     "movq        %%mm2,16(%1)\n\t" // CD->mem
	     "movq        %%mm6,(%1)\n\t"   // GH->mem
             : : "r" (sp), "r" (dp)
             );

	  sp += 8;
	  dp += 32;
        }
    }

  __asm__
    (
     "emms\n\t"
     );
}

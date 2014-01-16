/********************************************************************************
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

#include "libvideogfx/macros.hh"
#include "libvideogfx/graphics/color/internal_yuv2rgb_scalar.hh"


namespace videogfx {

  bool i2r_yuv_16bit::s_CanConvert(const Image<Pixel>& img,const RawRGBImageSpec& spec)
  {
    if (spec.dest_width || spec.upscale_factor || spec.downscale_factor) return false;
    if (spec.bits_per_pixel != 16) return false;
    // if (!spec.little_endian) return false;     // removed without being sure ...

    ImageParam param = img.AskParam();

    if (param.colorspace != Colorspace_YUV) return false;
    if (param.chroma !=Chroma_420) return false;

    return true;
  }

  void i2r_yuv_16bit::Transform(const Image<Pixel>& img,uint8* mem,int firstline,int lastline)
  {
    const uint32 rmask=d_spec.r_mask;
    const uint32 gmask=d_spec.g_mask;
    const uint32 bmask=d_spec.b_mask;

    uint32 rshift,gshift,bshift;

    rshift = d_spec.r_shift;  rshift -= 8-d_spec.r_bits;  rshift -= 8;  rshift = -rshift;
    gshift = d_spec.g_shift;  gshift -= 8-d_spec.g_bits;  gshift -= 8;  gshift = -gshift;
    bshift = d_spec.b_shift;  bshift -= 8-d_spec.b_bits;  bshift -= 8;  bshift = -bshift;

    ImageParam param = img.AskParam();

    assert(param.chroma==Chroma_420);
    assert(firstline%2 == 0);

    const Pixel*const* yp = img.AskFrameY();
    const Pixel*const* up = img.AskFrameU();
    const Pixel*const* vp = img.AskFrameV();

    int chr_w, chr_h;
    param.AskChromaSizes(chr_w,chr_h);

    bool fastversion = (rshift==0 &&  // red does not need shifting
			bshift>=0);   // blue will be shifted right -> no need to mask it out

    if (fastversion)
      {
	for (int cy=firstline/2;cy<=lastline/2;cy++)
	  {
	    const Pixel*  yptr1 = yp[2*cy  ];
	    const Pixel*  yptr2 = yp[2*cy+1];
	    const Pixel*  uptr  = up[cy];
	    const Pixel*  vptr  = vp[cy];
	    uint16* membuf16a = ((uint16*)(mem+ 2*cy   *d_spec.bytes_per_line));
	    uint16* membuf16b = ((uint16*)(mem+(2*cy+1)*d_spec.bytes_per_line));

	    for (int cx=0;cx<chr_w;cx++)
	      {
		int u=((int)*uptr++) -128;
		int v=((int)*vptr++) -128;
              
		int r0 = (int)(         + 409*v);
		int g0 = (int)( - 100*u - 208*v);
		int b0 = (int)( + 516*u        );

		int val;
		int yy=(((int)*yptr1++) -16)*298;

		int red   = r0+yy;
		if (red<=0) { val=0; } else if (red>0xff00) { val = rmask; } else { val = (red)&rmask; }
		int green = g0+yy;
		if (green<=0) { } else if (green>0xff00) { val|=gmask; } else { val |= (green>>gshift)&gmask; }
		int blue  = b0+yy;
		if (blue<=0) { } else if (blue>=0xff00) { val |= bmask; } else { val |= (blue>>bshift); }
		*membuf16a++ = (d_spec.little_endian ? ToLittleEndian((uint16)val) : ToBigEndian((uint16)val));

		yy=(((int)*yptr1++) -16)*298;
		red   = r0+yy;
		if (red<=0) { val=0; } else if (red>0xff00) { val = rmask; } else { val = (red)&rmask; }
		green = g0+yy;
		if (green<=0) { } else if (green>0xff00) { val|=gmask; } else { val |= (green>>gshift)&gmask; }
		blue  = b0+yy;
		if (blue<=0) { } else if (blue>=0xff00) { val |= bmask; } else { val |= (blue>>bshift); }
		*membuf16a++ = (d_spec.little_endian ? ToLittleEndian((uint16)val) : ToBigEndian((uint16)val));
              
		yy=(((int)*yptr2++) -16)*298;
		red   = r0+yy;
		if (red<=0) { val=0; } else if (red>0xff00) { val = rmask; } else { val = (red)&rmask; }
		green = g0+yy;
		if (green<=0) { } else if (green>0xff00) { val|=gmask; } else { val |= (green>>gshift)&gmask; }
		blue  = b0+yy;
		if (blue<=0) { } else if (blue>=0xff00) { val |= bmask; } else { val |= (blue>>bshift); }
		*membuf16b++ = (d_spec.little_endian ? ToLittleEndian((uint16)val) : ToBigEndian((uint16)val));
              
		yy=(((int)*yptr2++) -16)*298;
		red   = r0+yy;
		if (red<=0) { val=0; } else if (red>0xff00) { val = rmask; } else { val = (red)&rmask; }
		green = g0+yy;
		if (green<=0) { } else if (green>0xff00) { val|=gmask; } else { val |= (green>>gshift)&gmask; }
		blue  = b0+yy;
		if (blue<=0) { } else if (blue>=0xff00) { val |= bmask; } else { val |= (blue>>bshift); }
		*membuf16b++ = (d_spec.little_endian ? ToLittleEndian((uint16)val) : ToBigEndian((uint16)val));
	      }
	  }
      }
    else
      {
	for (int cy=firstline/2;cy<=lastline/2;cy++)
	  {
	    const Pixel*  yptr1 = yp[2*cy  ];
	    const Pixel*  yptr2 = yp[2*cy+1];
	    const Pixel*  uptr  = up[cy];
	    const Pixel*  vptr  = vp[cy];
	    uint16* membuf16a = ((uint16*)(mem+ 2*cy   *d_spec.bytes_per_line));
	    uint16* membuf16b = ((uint16*)(mem+(2*cy+1)*d_spec.bytes_per_line));

	    for (int cx=0;cx<chr_w;cx++)
	      {
		int u=((int)*uptr++) -128;
		int v=((int)*vptr++) -128;
              
		int r0 = (int)(         + 409*v);
		int g0 = (int)( - 100*u - 208*v);
		int b0 = (int)( + 516*u        );
              
		int val;
		int yy=(((int)*yptr1++) -16)*298;
		int red   = r0+yy;
		if (red<=0) { val=0; } else if (red>0xff00) { val=rmask; } else { val = (red>>rshift)&rmask; }
		int green = g0+yy;
		if (green<=0) { } else if (green>0xff00) { val|=gmask; } else { val |= (green>>gshift)&gmask; }
		int blue  = b0+yy;
		if (blue<=0) { } else if (blue>=0xff00) { val |= bmask; } else { val |= (blue>>bshift)&bmask; }
		*membuf16a++ = (d_spec.little_endian ? ToLittleEndian((uint16)val) : ToBigEndian((uint16)val));
              
		yy=(((int)*yptr1++) -16)*298;
		red   = r0+yy;
		if (red<=0) { val=0; } else if (red>0xff00) { val=rmask; } else { val = (red>>rshift)&rmask; }
		green = g0+yy;
		if (green<=0) { } else if (green>0xff00) { val|=gmask; } else { val |= (green>>gshift)&gmask; }
		blue  = b0+yy;
		if (blue<=0) { } else if (blue>=0xff00) { val |= bmask; } else { val |= (blue>>bshift)&bmask; }
		*membuf16a++ = (d_spec.little_endian ? ToLittleEndian((uint16)val) : ToBigEndian((uint16)val));
              
		yy=(((int)*yptr2++) -16)*298;
		red   = r0+yy;
		if (red<=0) { val=0; } else if (red>0xff00) { val=rmask; } else { val = (red>>rshift)&rmask; }
		green = g0+yy;
		if (green<=0) { } else if (green>0xff00) { val|=gmask; } else { val |= (green>>gshift)&gmask; }
		blue  = b0+yy;
		if (blue<=0) { } else if (blue>=0xff00) { val |= bmask; } else { val |= (blue>>bshift)&bmask; }
		*membuf16b++ = (d_spec.little_endian ? ToLittleEndian((uint16)val) : ToBigEndian((uint16)val));
              
		yy=(((int)*yptr2++) -16)*298;
		red   = r0+yy;
		if (red<=0) { val=0; } else if (red>0xff00) { val=rmask; } else { val = (red>>rshift)&rmask; }
		green = g0+yy;
		if (green<=0) { } else if (green>0xff00) { val|=gmask; } else { val |= (green>>gshift)&gmask; }
		blue  = b0+yy;
		if (blue<=0) { } else if (blue>=0xff00) { val |= bmask; } else { val |= (blue>>bshift)&bmask; }
		*membuf16b++ = (d_spec.little_endian ? ToLittleEndian((uint16)val) : ToBigEndian((uint16)val));
	      }
	  }
      }
  }



  // --------------------------------------------------------------------------------------------



  bool i2r_yuv444_16bit::s_CanConvert(const Image<Pixel>& img,const RawRGBImageSpec& spec)
  {
    if (spec.dest_width || spec.upscale_factor || spec.downscale_factor) return false;
    if (spec.bits_per_pixel != 16) return false;

    ImageParam param = img.AskParam();

    if (param.colorspace != Colorspace_YUV) return false;
    if (param.chroma !=Chroma_444) return false;

    return true;
  }

  void i2r_yuv444_16bit::Transform(const Image<Pixel>& img,uint8* mem,int firstline,int lastline)
  {
    const uint32 rmask=d_spec.r_mask;
    const uint32 gmask=d_spec.g_mask;
    const uint32 bmask=d_spec.b_mask;

    uint32 rshift,gshift,bshift;

    rshift = d_spec.r_shift;  rshift -= 8-d_spec.r_bits;  rshift -= 8;  rshift = -rshift;
    gshift = d_spec.g_shift;  gshift -= 8-d_spec.g_bits;  gshift -= 8;  gshift = -gshift;
    bshift = d_spec.b_shift;  bshift -= 8-d_spec.b_bits;  bshift -= 8;  bshift = -bshift;

    ImageParam param = img.AskParam();

    assert(param.chroma==Chroma_444);

    const Pixel*const* yp = img.AskFrameY();
    const Pixel*const* up = img.AskFrameU();
    const Pixel*const* vp = img.AskFrameV();

    const int w = img.AskWidth();

    bool fastversion = (rshift==0 &&  // red does not need shifting
			bshift>=0);   // blue will be shifted right -> no need to mask it out

    if (fastversion)
      {
	for (int cy=firstline;cy<=lastline;cy++)
	  {
	    const Pixel*  yptr = yp[cy];
	    const Pixel*  uptr = up[cy];
	    const Pixel*  vptr = vp[cy];
	    uint16* membuf16a = ((uint16*)(mem+ cy*d_spec.bytes_per_line));

	    for (int cx=0;cx<w;cx++)
	      {
		int u=((int)*uptr++) -128;
		int v=((int)*vptr++) -128;
              
		int r0 = (int)(         + 409*v);
		int g0 = (int)( - 100*u - 208*v);
		int b0 = (int)( + 516*u        );

		int val;
		int yy=(((int)*yptr++) -16)*298;

		int red   = r0+yy;
		if (red<=0) { val=0; } else if (red>0xff00) { val = rmask; } else { val = (red)&rmask; }
		int green = g0+yy;
		if (green<=0) { } else if (green>0xff00) { val|=gmask; } else { val |= (green>>gshift)&gmask; }
		int blue  = b0+yy;
		if (blue<=0) { } else if (blue>=0xff00) { val |= bmask; } else { val |= (blue>>bshift); }
		*membuf16a++ = (d_spec.little_endian ? ToLittleEndian((uint16)val) : ToBigEndian((uint16)val));
	      }
	  }
      }
    else
      {
	for (int cy=firstline;cy<=lastline;cy++)
	  {
	    const Pixel*  yptr = yp[cy];
	    const Pixel*  uptr = up[cy];
	    const Pixel*  vptr = vp[cy];
	    uint16* membuf16a = ((uint16*)(mem+ 2*cy   *d_spec.bytes_per_line));

	    for (int cx=0;cx<w;cx++)
	      {
		int u=((int)*uptr++) -128;
		int v=((int)*vptr++) -128;
              
		int r0 = (int)(         + 409*v);
		int g0 = (int)( - 100*u - 208*v);
		int b0 = (int)( + 516*u        );
              
		int val;
		int yy=(((int)*yptr++) -16)*298;
		int red   = r0+yy;
		if (red<=0) { val=0; } else if (red>0xff00) { val=rmask; } else { val = (red>>rshift)&rmask; }
		int green = g0+yy;
		if (green<=0) { } else if (green>0xff00) { val|=gmask; } else { val |= (green>>gshift)&gmask; }
		int blue  = b0+yy;
		if (blue<=0) { } else if (blue>=0xff00) { val |= bmask; } else { val |= (blue>>bshift)&bmask; }
		*membuf16a++ = (d_spec.little_endian ? ToLittleEndian((uint16)val) : ToBigEndian((uint16)val));
	      }
	  }
      }
  }



  // --------------------------------------------------------------------------------------------


  bool i2r_yuv_32bit::s_CanConvert(const Image<Pixel>& img,const RawRGBImageSpec& spec)
  {
    if (spec.dest_width || spec.upscale_factor || spec.downscale_factor) return false;
    if (spec.bits_per_pixel != 32 && spec.bits_per_pixel != 24) return false;
    if (spec.r_bits != 8 || spec.g_bits != 8 || spec.b_bits != 8) return false;
    if (spec.r_shift%8   || spec.g_shift%8   || spec.b_shift%8)   return false;

    ImageParam param = img.AskParam();

    if (param.colorspace != Colorspace_YUV) return false;
    if (param.chroma !=Chroma_420) return false;

    return true;
  }

  static int* clip_0_255=NULL;
  static int s_clip[1024];


  static bool clipping_initialized=false;

  void InitClip()
  {
    if (clipping_initialized)
      return;

    clip_0_255 = &s_clip[512];

    for (int i=-512;i<512;i++)
      {
	if (i<0)
	  {
	    clip_0_255[i]=0;
	  }
	else if (i>255)
	  {
	    clip_0_255[i]=255;
	  }
	else
	  {
	    clip_0_255[i]=i;
	  }
      }

    clipping_initialized=true;
  }


  void i2r_yuv_32bit::Transform(const Image<Pixel>& img,uint8* mem,int firstline,int lastline)
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

    assert(param.chroma==Chroma_420);

    const Pixel*const * pix_y = img.AskFrameY();
    const Pixel*const * pix_u = img.AskFrameU();
    const Pixel*const * pix_v = img.AskFrameV();

    InitClip();

    for (int y=firstline;y<=lastline;y+=2)
      {
	uint8* membuf8a = mem + d_spec.bytes_per_line*(y-firstline);
	uint8* membuf8b = mem + d_spec.bytes_per_line*y +d_spec.bytes_per_line;

	const Pixel* cbp = pix_u[y/2];
	const Pixel* crp = pix_v[y/2];
	const Pixel* y0p = pix_y[y  ];
	const Pixel* y1p = pix_y[y+1];

	for (int x=0;x<param.width;x+=2)
	  {
	    int cb=((int)*cbp++) -128;
	    int cr=((int)*crp++) -128;

	    int r0 = (int)(          + 409*cr);
	    int g0 = (int)( - 100*cb - 208*cr);
	    int b0 = (int)( + 516*cb         );
          
	    int yy=(((int)*y0p++) -16)*298;
	    int red   = (r0+yy)>>8; red  = clip_0_255[red];
	    int green = (g0+yy)>>8; green= clip_0_255[green];
	    int blue  = (b0+yy)>>8; blue = clip_0_255[blue];
	    membuf8a[rpos] = red;
	    membuf8a[gpos] = green;
	    membuf8a[bpos] = blue;
	    membuf8a+=bytes_per_pixel;

	    yy=(((int)*y0p++) -16)*298;
	    red   = (r0+yy)>>8; red  = clip_0_255[red];
	    green = (g0+yy)>>8; green= clip_0_255[green];
	    blue  = (b0+yy)>>8; blue = clip_0_255[blue];
	    membuf8a[rpos] = red;
	    membuf8a[gpos] = green;
	    membuf8a[bpos] = blue;
	    membuf8a+=bytes_per_pixel;

	    if (y != lastline)
	      {
		yy=(((int)*y1p++) -16)*298;
		red   = (r0+yy)>>8; red  = clip_0_255[red];
		green = (g0+yy)>>8; green= clip_0_255[green];
		blue  = (b0+yy)>>8; blue = clip_0_255[blue];
		membuf8b[rpos] = red;
		membuf8b[gpos] = green;
		membuf8b[bpos] = blue;
		membuf8b+=bytes_per_pixel;
          
		yy=(((int)*y1p++) -16)*298;
		red   = (r0+yy)>>8; red  = clip_0_255[red];
		green = (g0+yy)>>8; green= clip_0_255[green];
		blue  = (b0+yy)>>8; blue = clip_0_255[blue];
		membuf8b[rpos] = red;
		membuf8b[gpos] = green;
		membuf8b[bpos] = blue;
		membuf8b+=bytes_per_pixel;
	      }
	  }
      }
  }


  // --------------------------------------------------------------------------------------------

  bool i2r_yuv422_32bit::s_CanConvert(const Image<Pixel>& img,const RawRGBImageSpec& spec)
  {
    if (spec.dest_width || spec.upscale_factor || spec.downscale_factor) return false;
    if (spec.bits_per_pixel != 32 && spec.bits_per_pixel != 24) return false;
    if (spec.r_bits != 8 || spec.g_bits != 8 || spec.b_bits != 8) return false;
    if (spec.r_shift%8   || spec.g_shift%8   || spec.b_shift%8)   return false;

    ImageParam param = img.AskParam();

    if (param.colorspace != Colorspace_YUV) return false;
    if (param.chroma !=Chroma_422) return false;

    return true;
  }

  void i2r_yuv422_32bit::Transform(const Image<Pixel>& img,uint8* mem,int firstline,int lastline)
  {
    int rpos,gpos,bpos;

    int bytes_per_pixel = d_spec.bits_per_pixel/8;

    rpos = bytes_per_pixel-d_spec.r_shift/8;
    gpos = bytes_per_pixel-d_spec.g_shift/8;
    bpos = bytes_per_pixel-d_spec.b_shift/8;

    if (d_spec.little_endian)
      {
	rpos = bytes_per_pixel-rpos;
	gpos = bytes_per_pixel-gpos;
	bpos = bytes_per_pixel-bpos;
      }


    ImageParam param = img.AskParam();

    assert(param.chroma==Chroma_422);

    const Pixel*const * pix_y = img.AskFrameY();
    const Pixel*const * pix_u = img.AskFrameU();
    const Pixel*const * pix_v = img.AskFrameV();

    InitClip();

    for (int y=firstline;y<=lastline;y++)
      {
	uint8* membuf8a = mem + d_spec.bytes_per_line*(y-firstline);

	const Pixel* cbp = pix_u[y];
	const Pixel* crp = pix_v[y];
	const Pixel* y0p = pix_y[y];

	for (int x=0;x<param.width;x+=2)
	  {
	    int cb=((int)*cbp++) -128;
	    int cr=((int)*crp++) -128;

	    int r0 = (int)(          + 409*cr);
	    int g0 = (int)( - 100*cb - 208*cr);
	    int b0 = (int)( + 516*cb         );
          
	    int yy=(((int)*y0p++) -16)*298;
	    int red   = (r0+yy)>>8; red  = clip_0_255[red];
	    int green = (g0+yy)>>8; green= clip_0_255[green];
	    int blue  = (b0+yy)>>8; blue = clip_0_255[blue];
	    membuf8a[rpos] = red;
	    membuf8a[gpos] = green;
	    membuf8a[bpos] = blue;
	    membuf8a+=bytes_per_pixel;

	    yy=(((int)*y0p++) -16)*298;
	    red   = (r0+yy)>>8; red  = clip_0_255[red];
	    green = (g0+yy)>>8; green= clip_0_255[green];
	    blue  = (b0+yy)>>8; blue = clip_0_255[blue];
	    membuf8a[rpos] = red;
	    membuf8a[gpos] = green;
	    membuf8a[bpos] = blue;
	    membuf8a+=bytes_per_pixel;
	  }
      }
  }

  // --------------------------------------------------------------------------------------------

  bool i2r_yuv444_32bit::s_CanConvert(const Image<Pixel>& img,const RawRGBImageSpec& spec)
  {
    if (spec.dest_width || spec.upscale_factor || spec.downscale_factor) return false;
    if (spec.bits_per_pixel != 32) return false;
    if (spec.r_bits != 8 || spec.g_bits != 8 || spec.b_bits != 8) return false;
    if (spec.r_shift%8   || spec.g_shift%8   || spec.b_shift%8)   return false;

    ImageParam param = img.AskParam();

    if (param.colorspace != Colorspace_YUV) return false;
    if (param.chroma !=Chroma_444) return false;

    return true;
  }

  void i2r_yuv444_32bit::Transform(const Image<Pixel>& img,uint8* mem,int firstline,int lastline)
  {
    int rpos,gpos,bpos;

    rpos = 3-d_spec.r_shift/8;
    gpos = 3-d_spec.g_shift/8;
    bpos = 3-d_spec.b_shift/8;

    if (d_spec.little_endian)
      {
	rpos = 3-rpos;
	gpos = 3-gpos;
	bpos = 3-bpos;
      }


    ImageParam param = img.AskParam();

    assert(param.chroma==Chroma_444);

    const Pixel*const * pix_y = img.AskFrameY();
    const Pixel*const * pix_u = img.AskFrameU();
    const Pixel*const * pix_v = img.AskFrameV();

    InitClip();

    for (int y=firstline;y<=lastline;y++)
      {
	uint8* membuf8a = mem + d_spec.bytes_per_line*(y-firstline);

	const Pixel* cbp = pix_u[y];
	const Pixel* crp = pix_v[y];
	const Pixel* y0p = pix_y[y];

	for (int x=0;x<param.width;x++)
	  {
	    int cb=((int)*cbp++) -128;
	    int cr=((int)*crp++) -128;

	    int r0 = (int)(          + 409*cr);
	    int g0 = (int)( - 100*cb - 208*cr);
	    int b0 = (int)( + 516*cb         );
          
	    int yy=(((int)*y0p++) -16)*298;
	    int red   = (r0+yy)>>8; red  = clip_0_255[red];
	    int green = (g0+yy)>>8; green= clip_0_255[green];
	    int blue  = (b0+yy)>>8; blue = clip_0_255[blue];
	    membuf8a[rpos] = red;
	    membuf8a[gpos] = green;
	    membuf8a[bpos] = blue;
	    membuf8a+=4;
	  }
      }
  }

}

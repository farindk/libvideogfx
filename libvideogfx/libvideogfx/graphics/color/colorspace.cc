/*
 *  colorspace.cc
 */

#include "libvideogfx/graphics/color/colorspace.hh"
#include "libvideogfx/graphics/draw/scale.hh"
#include "libvideogfx/graphics/draw/blit.hh"
#include "libvideogfx/graphics/draw/draw.hh"
#include "libvideogfx/error.hh"


namespace videogfx {

  void ChangeChroma(Image<Pixel>& dst, const Image<Pixel>& src, ChromaFormat chroma)
  {
    ImageParam param = dst.AskParam();

    Assert(src.AskParam().colorspace == Colorspace_YUV);

    param.width      = src.AskWidth();
    param.height     = src.AskHeight();
    param.colorspace = src.AskParam().colorspace;
    param.chroma     = chroma;
    dst.Create(param);

    const ChromaFormat oldchroma = src.AskParam().chroma;

    CopyToNew(dst.AskBitmap(Bitmap_Y)     , src.AskBitmap(Bitmap_Y));
    CopyToNew(dst.AskBitmap(Bitmap_Alpha) , src.AskBitmap(Bitmap_Alpha));

    if (oldchroma == Chroma_420 && chroma == Chroma_422)
      {
	DoubleSize_Dup_V(dst.AskBitmap(Bitmap_U) , src.AskBitmap(Bitmap_U));
	DoubleSize_Dup_V(dst.AskBitmap(Bitmap_V) , src.AskBitmap(Bitmap_V));
      }
    else if (oldchroma == Chroma_420 && chroma == Chroma_444)
      {
	DoubleSize_Dup(dst.AskBitmap(Bitmap_U) , src.AskBitmap(Bitmap_U));
	DoubleSize_Dup(dst.AskBitmap(Bitmap_V) , src.AskBitmap(Bitmap_V));
      }
    else if (oldchroma == Chroma_422 && chroma == Chroma_444)
      {
	DoubleSize_Dup_H(dst.AskBitmap(Bitmap_U) , src.AskBitmap(Bitmap_U));
	DoubleSize_Dup_H(dst.AskBitmap(Bitmap_V) , src.AskBitmap(Bitmap_V));
      }
    else if (oldchroma == Chroma_444 && chroma == Chroma_420)
      {
	HalfSize_Avg(dst.AskBitmap(Bitmap_U) , src.AskBitmap(Bitmap_U));
	HalfSize_Avg(dst.AskBitmap(Bitmap_V) , src.AskBitmap(Bitmap_V));
      }
    else if (oldchroma == Chroma_444 && chroma == Chroma_422)
      {
	HalfSize_Avg_H(dst.AskBitmap(Bitmap_U) , src.AskBitmap(Bitmap_U));
	HalfSize_Avg_H(dst.AskBitmap(Bitmap_V) , src.AskBitmap(Bitmap_V));
      }
    else if (oldchroma == Chroma_422 && chroma == Chroma_420)
      {
	HalfSize_Avg_V(dst.AskBitmap(Bitmap_U) , src.AskBitmap(Bitmap_U));
	HalfSize_Avg_V(dst.AskBitmap(Bitmap_V) , src.AskBitmap(Bitmap_V));
      }
    else
      {
	CopyToNew(dst.AskBitmap(Bitmap_U) , src.AskBitmap(Bitmap_U));
	CopyToNew(dst.AskBitmap(Bitmap_V) , src.AskBitmap(Bitmap_V));
      }
  }



  void ChangeColorspace(Image<Pixel>& dst, const Image<Pixel>& src,
			Colorspace colorspace, ChromaFormat chroma)
  {
    ImageParam param = dst.AskParam();

    /* If source and target colorspace are the same, do nothing.
       Exception: if both are YUV and the chroma format differs, do chroma format conversion. */

    if (src.AskParam().colorspace == colorspace)
      {
	if (param.colorspace == Colorspace_YUV)
	  {
	    if (chroma == src.AskParam().chroma)
	      CopyToNew(dst,src);
	    else
	      ChangeChroma(dst, src, chroma);
	  }
	else
	  {
	    CopyToNew(dst,src);
	  }

	return;
      }


    // Find appropriate conversion routine

    Colorspace src_colorspace = src.AskParam().colorspace;
    bool did_convert=false;

    if (src_colorspace==Colorspace_YUV)   // YUV -> *
      {
	Image<Pixel> mysrc;  /* Input image for chroma conversion. If it is in YUV colorspace,
				chroma conversion to Chroma_444 is applied. */

	if (src.AskParam().chroma == Chroma_444)
	  mysrc=src;
	else
	  ChangeChroma(mysrc,src, Chroma_444);


	if (colorspace==Colorspace_RGB)
	  {
	    YUV2RGB(dst, mysrc);
	    did_convert=true;
	  }
	else if (colorspace==Colorspace_Greyscale)
	  {
	    YUV2Greyscale(dst, mysrc);
	    did_convert=true;
	  }
      }
    else if (src_colorspace==Colorspace_RGB) // RGB -> *
      {
	if (colorspace==Colorspace_YUV)
	  {
	    RGB2YUV(dst, src, chroma);
	    did_convert=true;
	  }
	else if (colorspace==Colorspace_HSV)
	  {
	    RGB2HSV(dst, src);
	    did_convert=true;
	  }
	else if (colorspace==Colorspace_Greyscale)
	  {
	    RGB2Greyscale(dst, src);
	    did_convert=true;
	  }
      }
    else if (src_colorspace==Colorspace_HSV) // HSV -> *
      {
	if (colorspace==Colorspace_RGB)
	  {
	    HSV2RGB(dst, src);
	    did_convert=true;
	  }
      }
    else if (src_colorspace==Colorspace_Greyscale) // Greyscale -> *
      {
	if (colorspace==Colorspace_RGB)
	  {
	    Greyscale2RGB(dst, src);
	    did_convert=true;
	  }
	else if (colorspace==Colorspace_YUV)
	  {
	    Greyscale2RGB(dst, src);
	    did_convert=true;
	  }
      }


    /* If conversion was not possible, do it the hard way:
       Convert to RGB and then back to the destination colorspace. */

    if (!did_convert)
      {
	Image<Pixel> tmp;
	ChangeColorspace(tmp, src, Colorspace_RGB);
	ChangeColorspace(dst, tmp, colorspace);
      }
  }


  inline int Clip(int x) { if (x<0) return 0; if (x>255) return 255; return x; }


  void RGB2YUV(Pixel& y,Pixel& u,Pixel& v, Pixel r,Pixel g,Pixel b)
  {
    // clipping is not needed here

    y = (( 65*r +  129*g +  24*b)>>8)+16;
    u = ((-37*r +  -74*g + 112*b)>>8)+128;
    v = ((112*r +  -93*g + -18*b)>>8)+128;
  }


  void YUV2RGB(Pixel& r,Pixel& g,Pixel& b, Pixel y,Pixel u,Pixel v)
  {
    int yy = y -16;
    int uu = u -128;
    int vv = v -128;

    r = Clip((          409*vv + 298*yy)>>8);
    g = Clip((-100*uu - 208*vv + 298*yy)>>8);
    b = Clip(( 516*uu          + 298*yy)>>8);
  }



  void RGB2YUV(Image<Pixel>& dst, const Image<Pixel>& src, ChromaFormat chroma)
  {
    Image<Pixel>* out;
    Image<Pixel>  tmp444;

    if (chroma == Chroma_Invalid)
      chroma = dst.AskParam().chroma;

    if (chroma==Chroma_444)  out = &dst;
    else                     out = &tmp444;


    // Create destination image

    ImageParam param = dst.AskParam();
    param.colorspace = Colorspace_YUV;
    param.width  = src.AskWidth();
    param.height = src.AskHeight();
    param.chroma = Chroma_444;
    out->Create(param);


    // main conversion loop

    const Pixel*const* rs = src.AskFrameR();
    const Pixel*const* gs = src.AskFrameG();
    const Pixel*const* bs = src.AskFrameB();

    Pixel*const* yd = out->AskFrameY();
    Pixel*const* ud = out->AskFrameU();
    Pixel*const* vd = out->AskFrameV();

    for (int y=0;y<param.height;y++)
      for (int x=0;x<param.width;x++)
	{
	  int r = ((int)rs[y][x]);
	  int g = ((int)gs[y][x]);
	  int b = ((int)bs[y][x]);

	  yd[y][x] = (( 65*r +  129*g +  24*b)>>8)+16;
	  ud[y][x] = ((-37*r +  -74*g + 112*b)>>8)+128;
	  vd[y][x] = ((112*r +  -93*g + -18*b)>>8)+128;
	}


    // change chroma format, if other than Chroma_444 is selected

    if (chroma!=Chroma_444)
      ChangeChroma(dst,*out, chroma);
  }


  void YUV2RGB(Image<Pixel>& dst, const Image<Pixel>& src)
  {
    ImageParam param = dst.AskParam();
    param.colorspace = Colorspace_RGB;
    param.width  = src.AskWidth();
    param.height = src.AskHeight();
    dst.Create(param);


    Pixel*const* rd = dst.AskFrameR();
    Pixel*const* gd = dst.AskFrameG();
    Pixel*const* bd = dst.AskFrameB();

    const Pixel*const* ys = src.AskFrameY();
    const Pixel*const* us = src.AskFrameU();
    const Pixel*const* vs = src.AskFrameV();


    switch (src.AskParam().chroma)
      {
      case Chroma_444:
	for (int y=0;y<param.height;y++)
	  for (int x=0;x<param.width;x++)
	    YUV2RGB(rd[y][x],gd[y][x],bd[y][x], ys[y][x],us[y][x],vs[y][x]);
	break;

      case Chroma_422:
	for (int y=0;y<param.height;y++)
	  for (int x=0;x<param.width;x++)
	    YUV2RGB(rd[y][x],gd[y][x],bd[y][x], ys[y][x>>1],us[y][x>>1],vs[y][x>>1]);
	break;

      case Chroma_420:
	for (int y=0;y<param.height;y++)
	  for (int x=0;x<param.width;x++)
	    YUV2RGB(rd[y][x],gd[y][x],bd[y][x], ys[y>>1][x>>1],us[y>>1][x>>1],vs[y>>1][x>>1]);
	break;
      }
  }


  void HSV2RGB(double& red,double& green,double& blue,  double h,double s,double br)
  {
    double hue = h;
    double saturation = s;
    double brightness = br;

    if (brightness == 0.0) { red   = green = blue  = 0.0;        return; }
    if (saturation == 0.0) { red   = green = blue  = brightness; return; }

    double offs;   // hue mod 1/6
    if (hue < 1.0/6)
      {   // red domain; green ascends
	offs = hue;
	red   = brightness;
	blue  = brightness * (1.0 - saturation);
	green = blue + (brightness - blue) * offs * 6;
      }
    else if (hue < 2.0/6)
      { // yellow domain; red descends
	offs = hue - 1.0/6;
	green = brightness;
	blue  = brightness * (1.0 - saturation);
	red   = green - (brightness - blue) * offs * 6;
      }
    else if (hue < 3.0/6)
      { // green domain; blue ascends
	offs = hue - 2.0/6;
	green = brightness;
	red   = brightness * (1.0 - saturation);
	blue  = red + (brightness - red) * offs * 6;
      }
    else if (hue < 4.0/6)
      { // cyan domain; green descends
	offs = hue - 3.0/6;
	blue  = brightness;
	red   = brightness * (1.0 - saturation);
	green = blue - (brightness - red) * offs * 6;
      }
    else if (hue < 5.0/6)
      { // blue domain; red ascends
	offs = hue - 4.0/6;
	blue  = brightness;
	green = brightness * (1.0 - saturation);
	red   = green + (brightness - green) * offs * 6;
      }
    else
      { // magenta domain; blue descends
	offs = hue - 5.0/6;
	red   = brightness;
	green = brightness * (1.0 - saturation);
	blue  = red - (brightness - green) * offs * 6;
      }
  }


  void RGB2HSV(double& h,double& s,double& v, double red,double green,double blue)
  {
    double mini, maxi;
    double delta;
  
    h = 0.0;
  
    if (red > green)
      {
	if (red > blue)
	  maxi = red;
	else
	  maxi = blue;
      
	if (green < blue)
	  mini = green;
	else
	  mini = blue;
      }
    else
      {
	if (green > blue)
	  maxi = green;
	else
	  maxi = blue;
      
	if (red < blue)
	  mini = red;
	else
	  mini = blue;
      }
  
    v = maxi;
  
    if (maxi != 0.0)
      s = (maxi - mini) / maxi;
    else
      s = 0.0;
  
    if (s == 0.0)
      h = 0.0;
    else
      {
	delta = maxi - mini;
      
	if (red == maxi)
	  h = (green - blue) / delta;
	else if (green == maxi)
	  h = 2 + (blue - red) / delta;
	else if (blue == maxi)
	  h = 4 + (red - green) / delta;
      
	h /= 6.0;
      
	if (h < 0.0)
	  h += 1.0;
	else if (h > 1.0)
	  h -= 1.0;
      }
  }


  void RGB2HSV(Image<double>& dst, const Image<Pixel>& src)
  {
    ImageParam param = dst.AskParam();
    param.colorspace = Colorspace_HSV;
    param.width  = src.AskWidth();
    param.height = src.AskHeight();
    dst.Create(param);


    const Pixel*const* rp = src.AskFrameR();
    const Pixel*const* gp = src.AskFrameG();
    const Pixel*const* bp = src.AskFrameB();

    double*const* hp = dst.AskFrame(Bitmap_Hue);
    double*const* sp = dst.AskFrame(Bitmap_Saturation);
    double*const* vp = dst.AskFrame(Bitmap_Value);

    for (int y=0;y<param.height;y++)
      for (int x=0;x<param.width;x++)
	{
	  double r = ((double)rp[y][x])/255.0;
	  double g = ((double)rp[y][x])/255.0;
	  double b = ((double)rp[y][x])/255.0;

	  RGB2HSV(hp[y][x],sp[y][x],vp[y][x], r,g,b);
	}
  }


  void HSV2RGB(Image<Pixel>& dst, const Image<double>& src)
  {
    ImageParam param = dst.AskParam();
    param.colorspace = Colorspace_RGB;
    param.width  = src.AskWidth();
    param.height = src.AskHeight();
    dst.Create(param);


    Pixel*const* rp = dst.AskFrameR();
    Pixel*const* gp = dst.AskFrameG();
    Pixel*const* bp = dst.AskFrameB();

    const double*const* hp = src.AskFrame(Bitmap_Hue);
    const double*const* sp = src.AskFrame(Bitmap_Saturation);
    const double*const* vp = src.AskFrame(Bitmap_Value);

    for (int y=0;y<param.height;y++)
      for (int x=0;x<param.width;x++)
	{
	  double r,g,b;

	  HSV2RGB(r,g,b, hp[y][x],sp[y][x],vp[y][x]);

	  rp[y][x] = (int)(r*255.0);
	  gp[y][x] = (int)(g*255.0);
	  bp[y][x] = (int)(b*255.0);
	}
  }


  void RGB2HSV(Image<Pixel>& dst, const Image<Pixel>& src)
  {
    ImageParam param = dst.AskParam();
    param.colorspace = Colorspace_HSV;
    param.width  = src.AskWidth();
    param.height = src.AskHeight();
    dst.Create(param);

    const Pixel*const* rp = src.AskFrameR();
    const Pixel*const* gp = src.AskFrameG();
    const Pixel*const* bp = src.AskFrameB();

    Pixel*const* hp = dst.AskFrame(Bitmap_Hue);
    Pixel*const* sp = dst.AskFrame(Bitmap_Saturation);
    Pixel*const* vp = dst.AskFrame(Bitmap_Value);

    for (int y=0;y<param.height;y++)
      for (int x=0;x<param.width;x++)
	{
	  double r = ((double)rp[y][x])/255.0;
	  double g = ((double)gp[y][x])/255.0;
	  double b = ((double)bp[y][x])/255.0;

	  double h,s,v;

	  RGB2HSV(h,s,v, r,g,b);

	  hp[y][x] = (Pixel)(h*255);
	  sp[y][x] = (Pixel)(s*255);
	  vp[y][x] = (Pixel)(v*255);
	}
  }


  void HSV2RGB(Image<Pixel>&  dst, const Image<Pixel>& src)
  {
    ImageParam param = dst.AskParam();
    param.colorspace = Colorspace_RGB;
    param.width  = src.AskWidth();
    param.height = src.AskHeight();
    dst.Create(param);

    Pixel*const* rp = dst.AskFrameR();
    Pixel*const* gp = dst.AskFrameG();
    Pixel*const* bp = dst.AskFrameB();

    const Pixel*const* hp = src.AskFrame(Bitmap_Hue);
    const Pixel*const* sp = src.AskFrame(Bitmap_Saturation);
    const Pixel*const* vp = src.AskFrame(Bitmap_Value);

    for (int y=0;y<param.height;y++)
      for (int x=0;x<param.width;x++)
	{
	  double h = ((double)hp[y][x])/255.0;
	  double s = ((double)sp[y][x])/255.0;
	  double v = ((double)vp[y][x])/255.0;

	  double r,g,b;

	  HSV2RGB(r,g,b, h,s,v);

	  rp[y][x] = (Pixel)(r*255.0);
	  gp[y][x] = (Pixel)(g*255.0);
	  bp[y][x] = (Pixel)(b*255.0);
	}
  }


  void RGB2Greyscale(Image<Pixel>& dst, const Image<Pixel>& src)
  {
    // Create destination image

    ImageParam param = dst.AskParam();
    param.colorspace = Colorspace_Greyscale;
    param.width  = src.AskWidth();
    param.height = src.AskHeight();
    dst.Create(param);


    // main conversion loop

    const Pixel*const* rs = src.AskFrameR();
    const Pixel*const* gs = src.AskFrameG();
    const Pixel*const* bs = src.AskFrameB();

    Pixel*const* yd = dst.AskFrameY();

    for (int y=0;y<param.height;y++)
      for (int x=0;x<param.width;x++)
	{
	  int r = ((int)rs[y][x]);
	  int g = ((int)gs[y][x]);
	  int b = ((int)bs[y][x]);

	  yd[y][x] = Clip((( 65*r +  129*g +  24*b)>>8)+16);
	}
  }


  void Greyscale2RGB(Image<Pixel>& dst, const Image<Pixel>& src)
  {
    // Create destination image

    ImageParam param = dst.AskParam();
    param.colorspace = Colorspace_RGB;
    param.width  = src.AskWidth();
    param.height = src.AskHeight();
    dst.Create(param);


    // main conversion is really simple here

    CopyToNew(dst.AskBitmapR(), src.AskBitmapY());
    CopyToNew(dst.AskBitmapG(), src.AskBitmapY());
    CopyToNew(dst.AskBitmapB(), src.AskBitmapY());
  }


  void YUV2Greyscale(Image<Pixel>& dst, const Image<Pixel>& src)
  {
    // Create destination image

    ImageParam param = dst.AskParam();
    param.colorspace = Colorspace_Greyscale;
    param.width  = src.AskWidth();
    param.height = src.AskHeight();
    dst.Create(param);


    // main conversion is really simple here

    CopyToNew(dst.AskBitmapY(), src.AskBitmapY());
  }


  void Greyscale2YUV(Image<Pixel>& dst, const Image<Pixel>& src)
  {
    // Create destination image

    ImageParam param = dst.AskParam();
    param.colorspace = Colorspace_YUV;
    param.width  = src.AskWidth();
    param.height = src.AskHeight();
    dst.Create(param);


    // main conversion is really simple here

    CopyToNew(dst.AskBitmapY(), src.AskBitmapY());
    Clear(dst.AskBitmapU(), (Pixel)128);
    Clear(dst.AskBitmapV(), (Pixel)128);
  }

}

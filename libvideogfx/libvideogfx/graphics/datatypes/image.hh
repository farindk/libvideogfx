/********************************************************************************
  $Header$

  purpose:

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
    24/Jan/2002 - Dirk Farin - Complete reimplementation based on old Image type.
    02/Jun/1999 - Dirk Farin - first implementation
 ********************************************************************************
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

#ifndef LIBVIDEOGFX_GRAPHICS_BASIC_IMAGE_HH
#define LIBVIDEOGFX_GRAPHICS_BASIC_IMAGE_HH

#include <assert.h>

#include <libvideogfx/types.hh>
#include <libvideogfx/graphics/datatypes/bitmap.hh>
#include <algorithm>

namespace videogfx {

  enum Colorspace
  {
    Colorspace_RGB, Colorspace_YUV, Colorspace_Greyscale, Colorspace_HSV,
    Colorspace_Invalid
  };

  enum ChromaFormat
  {
    /** subsampling h:2 v:2 */ Chroma_420,
    /** subsampling h:2 v:1 */ Chroma_422,
    /** No subsampling      */ Chroma_444,
    Chroma_Invalid
  };

  enum BitmapChannel
  {
    Bitmap_Red = 0, Bitmap_Green = 1, Bitmap_Blue = 2,
    Bitmap_Y   = 0, Bitmap_Cb    = 1, Bitmap_Cr   = 2,
                    Bitmap_U     = 1, Bitmap_V    = 2,
    Bitmap_Hue = 0, Bitmap_Saturation = 1, Bitmap_Value = 2,
    Bitmap_Alpha=3
  };


  /** Check if chroma is horizontally subsampled. Usage of the more general #ChromaSubH()# is recommended. */
  inline bool IsSubH(ChromaFormat cf) { return cf != Chroma_444; }
  /** Check if chroma is vertically subsampled. Usage of the more general #ChromaSubV()# is recommended. */
  inline bool IsSubV(ChromaFormat cf) { return cf == Chroma_420; }
  /** Get horizontal subsampling factor. */
  inline int  ChromaSubH(ChromaFormat cf) { return (cf != Chroma_444) ? 2 : 1; }
  /** Get vertical subsampling factor. */
  inline int  ChromaSubV(ChromaFormat cf) { return (cf == Chroma_420) ? 2 : 1; }


  struct ImageParam
  {
    ImageParam() : width(0), height(0), halign(1), valign(1),
		   colorspace(Colorspace_Invalid), has_alpha(false),
		   chroma(Chroma_444), reduced_chroma_resolution(true),
		   chroma_border(-1), chroma_halign(-1), chroma_valign(-1)
    {
      border=0;
    }

    int width,height;
    int halign,valign;
    int border;

    Colorspace colorspace;
    bool has_alpha;

    ChromaFormat chroma;
    bool reduced_chroma_resolution;
    int  chroma_border;
    int  chroma_halign;
    int  chroma_valign;


    int  AskChromaWidth()  const
    {
      if (colorspace==Colorspace_YUV)
	return (width +ChromaSubH(chroma)-1)/ChromaSubH(chroma);
      else
	return width;
    }
    int  AskChromaHeight() const
    {
      if (colorspace==Colorspace_YUV)
	return (height+ChromaSubV(chroma)-1)/ChromaSubV(chroma);
      else
	return height;
    }
    void AskChromaSizes(int& w,int &h) const;
    int  AskChromaBorder() const;
    int  AskChromaHAlign() const;
    int  AskChromaVAlign() const;

    int  BitmapWidth (BitmapChannel b) const { if (b==1||b==2) return AskChromaWidth();  else return width; }
    int  BitmapHeight(BitmapChannel b) const { if (b==1||b==2) return AskChromaHeight(); else return height; }
    int  ChromaScaleH(BitmapChannel b,int x) const
    { if ((b==1||b==2) && colorspace==Colorspace_YUV) return x/ChromaSubH(chroma); else return x; }
    int  ChromaScaleV(BitmapChannel b,int y) const
    { if ((b==1||b==2) && colorspace==Colorspace_YUV) return y/ChromaSubV(chroma); else return y; }
  };




  template <class Pel> class Image
  {
  public:
    virtual ~Image() { }

    void Create(const ImageParam&);
    void Release();

    /// Get colorspace independent image parameters.
    ImageParam AskParam() const { return d_param; }
    int  AskWidth()  const { return d_param.width; }
    int  AskHeight() const { return d_param.height; }
    int  AskBorder() const { return d_param.border; }
    void GetSize(int& w,int& h) const { w=d_param.width; h=d_param.height; }


    Bitmap<Pel>&       AskBitmap(BitmapChannel pm_id)       { return d_pm[pm_id]; }
    const Bitmap<Pel>& AskBitmap(BitmapChannel pm_id) const { return d_pm[pm_id]; }

    Pel*const*       AskFrame(BitmapChannel pm_id)       { return d_pm[pm_id].AskFrame(); }
    const Pel*const* AskFrame(BitmapChannel pm_id) const { return d_pm[pm_id].AskFrame(); }


    /** Replace a complete bitmap. Note that the new bitmap either has to be empty or has to
	be exactly the same size as the old one.
	Furthermore you are responsible that all alignments and the border size is sufficient
	for your application. This is not checked!
      
	If you insert or remove (by replacing a bitmap by an empty one) an alpha bitmap,
	the alphamask-flag in ImageParam will be set accordingly.
    */
    void ReplaceBitmap(BitmapChannel id,const Bitmap<Pel>&);
    /// Set new image parameters.
    void SetParam(const ImageParam& param) { d_param=param; }

    Image<Pel> Clone() const;

    Image<Pel> CreateSubView  (int x0,int y0,int w,int h) const;
    Image<Pel> CreateFieldView(bool top) const;

    bool IsEmpty() const { return d_pm[0].IsEmpty(); }

    Pel*const* AskFrameR()        { return d_pm[Bitmap_Red].AskFrame(); }
    const Pel*const* AskFrameR()  const { return d_pm[Bitmap_Red].AskFrame(); }
    Pel*const* AskFrameG()        { return d_pm[Bitmap_Green].AskFrame(); }
    const Pel*const* AskFrameG()  const { return d_pm[Bitmap_Green].AskFrame(); }
    Pel*const* AskFrameB()        { return d_pm[Bitmap_Blue].AskFrame(); }
    const Pel*const* AskFrameB()  const { return d_pm[Bitmap_Blue].AskFrame(); }
    Pel*const* AskFrameY()        { return d_pm[Bitmap_Y].AskFrame(); }
    const Pel*const* AskFrameY()  const { return d_pm[Bitmap_Y].AskFrame(); }
    Pel*const* AskFrameU()        { return d_pm[Bitmap_U].AskFrame(); }
    const Pel*const* AskFrameU()  const { return d_pm[Bitmap_U].AskFrame(); }
    Pel*const* AskFrameV()        { return d_pm[Bitmap_V].AskFrame(); }
    const Pel*const* AskFrameV()  const { return d_pm[Bitmap_V].AskFrame(); }
    Pel*const* AskFrameCb()       { return d_pm[Bitmap_Cb].AskFrame(); }
    const Pel*const* AskFrameCb() const { return d_pm[Bitmap_Cb].AskFrame(); }
    Pel*const* AskFrameCr()       { return d_pm[Bitmap_Cr].AskFrame(); }
    const Pel*const* AskFrameCr() const { return d_pm[Bitmap_Cr].AskFrame(); }
    Pel*const* AskFrameA()        { return d_pm[Bitmap_Alpha].AskFrame(); }
    const Pel*const* AskFrameA()  const { return d_pm[Bitmap_Alpha].AskFrame(); }

    Bitmap<Pel>& AskBitmapR()        { return d_pm[Bitmap_Red]; }
    const Bitmap<Pel>& AskBitmapR()  const { return d_pm[Bitmap_Red]; }
    Bitmap<Pel>& AskBitmapG()        { return d_pm[Bitmap_Green]; }
    const Bitmap<Pel>& AskBitmapG()  const { return d_pm[Bitmap_Green]; }
    Bitmap<Pel>& AskBitmapB()        { return d_pm[Bitmap_Blue]; }
    const Bitmap<Pel>& AskBitmapB()  const { return d_pm[Bitmap_Blue]; }
    Bitmap<Pel>& AskBitmapY()        { return d_pm[Bitmap_Y]; }
    const Bitmap<Pel>& AskBitmapY()  const { return d_pm[Bitmap_Y]; }
    Bitmap<Pel>& AskBitmapU()        { return d_pm[Bitmap_U]; }
    const Bitmap<Pel>& AskBitmapU()  const { return d_pm[Bitmap_U]; }
    Bitmap<Pel>& AskBitmapV()        { return d_pm[Bitmap_V]; }
    const Bitmap<Pel>& AskBitmapV()  const { return d_pm[Bitmap_V]; }
    Bitmap<Pel>& AskBitmapCb()       { return d_pm[Bitmap_Cb]; }
    const Bitmap<Pel>& AskBitmapCb() const { return d_pm[Bitmap_Cb]; }
    Bitmap<Pel>& AskBitmapCr()       { return d_pm[Bitmap_Cr]; }
    const Bitmap<Pel>& AskBitmapCr() const { return d_pm[Bitmap_Cr]; }
    Bitmap<Pel>& AskBitmapA()        { return d_pm[Bitmap_Alpha]; }
    const Bitmap<Pel>& AskBitmapA()  const { return d_pm[Bitmap_Alpha]; }

    bool IsShared() const
    {
      for (int i=0;i<4;i++)
	if (d_pm[i].IsShared())
	  return true;

      return false;
    }

  private:
    Bitmap<Pel> d_pm[4];
    ImageParam  d_param;
  };



  template <class Pel> void Image<Pel>::Create(const ImageParam& param)
  {
    d_pm[0].Create(param.width, param.height, param.border,param.halign,param.valign);

    switch (param.colorspace)
      {
      case Colorspace_RGB:
      case Colorspace_HSV:
	d_pm[1].Create(param.width, param.height, param.border,param.halign,param.valign);
	d_pm[2].Create(param.width, param.height, param.border,param.halign,param.valign);
	break;

      case Colorspace_YUV:
	if (param.reduced_chroma_resolution)
	  {
	    d_pm[1].Create(param.AskChromaWidth(), param.AskChromaHeight(), param.AskChromaBorder(),
			   param.AskChromaHAlign(),param.AskChromaVAlign());
	    d_pm[2].Create(param.AskChromaWidth(), param.AskChromaHeight(), param.AskChromaBorder(),
			   param.AskChromaHAlign(),param.AskChromaVAlign());
	  }
	else
	  {
	    d_pm[1].Create(param.width, param.height, param.border,param.halign,param.valign);
	    d_pm[2].Create(param.width, param.height, param.border,param.halign,param.valign);
	  }
	break;

      case Colorspace_Greyscale:
	d_pm[1].Release();
	d_pm[2].Release();
	break;

      case Colorspace_Invalid:
	Assert(0);
	break;
      }

    if (param.has_alpha)
      d_pm[Bitmap_Alpha].Create(param.width, param.height, param.border,param.halign,param.valign);
    else
      d_pm[Bitmap_Alpha].Release();

    d_param = param;
  }


  template <class Pel> void Image<Pel>::Release()
  {
    for (int i=0;i<4;i++)
      d_pm[i].Release();

    d_param = ImageParam();
  }


  template <class Pel> Image<Pel> Image<Pel>::Clone() const
  {
    Image<Pel> img;
    for (int i=0;i<4;i++)
      img.d_pm[i] = d_pm[i].Clone();

    img.d_param = d_param;

    return img;
  }

  template <class Pel> Image<Pel> Image<Pel>::CreateSubView(int x0,int y0,int w,int h) const
  {
    Image<Pel> newimg;
    newimg.d_param = d_param;

    newimg.d_param.width  = w;
    newimg.d_param.height = h;
    newimg.d_param.halign = 1;
    newimg.d_param.valign = 1;
    newimg.d_param.border = 0;
    newimg.d_param.chroma_border = -1;
    newimg.d_param.chroma_halign = -1;
    newimg.d_param.chroma_valign = -1;

    if (d_param.colorspace == Colorspace_YUV)
      {
	newimg.d_pm[0] = d_pm[0].CreateSubView(x0,y0,w,h);
	newimg.d_pm[3] = d_pm[3].CreateSubView(x0,y0,w,h);

	int subh = ChromaSubH(d_param.chroma);
	int subv = ChromaSubV(d_param.chroma);

	newimg.d_pm[1] = d_pm[1].CreateSubView(x0/subh,y0/subv,(w+subh-1)/subh,(h+subv-1)/subv);
	newimg.d_pm[2] = d_pm[2].CreateSubView(x0/subh,y0/subv,(w+subh-1)/subh,(h+subv-1)/subv);
      }
    else
      {
	for (int i=0;i<4;i++)
	  newimg.d_pm[i] = d_pm[i].CreateSubView(x0,y0,w,h);
      }

    return newimg;
  }

  template <class Pel> Image<Pel> Image<Pel>::CreateFieldView(bool top) const
  {
    if (!top && d_param.colorspace==Colorspace_YUV && d_param.chroma==Chroma_420 &&
	(d_pm[0].AskHeight()%2)==0 && (d_pm[1].AskHeight()%2)==1)
      {
	AssertDescr(false,"not enough chroma information for bottom field");
      }

    Image<Pel> newimg;
    newimg.d_param = d_param;

    for (int i=0;i<4;i++)
      newimg.d_pm[i] = d_pm[i].CreateFieldView(top);

    newimg.d_param.width  = newimg.d_pm[0].AskWidth();
    newimg.d_param.height = newimg.d_pm[0].AskHeight();
    newimg.d_param.halign = 1;
    newimg.d_param.valign = 1;
    newimg.d_param.border = 0;
    newimg.d_param.chroma_border = -1;
    newimg.d_param.chroma_halign = -1;
    newimg.d_param.chroma_valign = -1;

    return newimg;
  }

}

#endif

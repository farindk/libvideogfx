/*********************************************************************
  color/colorspace.hh

  purpose:
    Routines for chroma format and colorspace convertions.

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
    22/May/2001 - Dirk Farin - added HSV colorspace conversion
    30/Nov/1999 - Dirk Farin - implemented RGB2YUV_444()
    03/Aug/1999 - Dirk Farin - new functions: - TrTo???_DupSub()
                                              - Grey2RGB_Inplace()
    20/Jul/1999 - Dirk Farin - complete rewrite
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

#ifndef LIBVIDEOGFX_GRAPHICS_COLOR_COLORSPACE_HH
#define LIBVIDEOGFX_GRAPHICS_COLOR_COLORSPACE_HH

#include <libvideogfx/graphics/datatypes/image.hh>
#include <libvideogfx/error.hh>

namespace videogfx {

  void ChangeChroma    (Image<Pixel>& dst, const Image<Pixel>& src, ChromaFormat dstformat);

  /* Copy the image contents of image "src" to image "dst" with a conversion of colorspace.
     If target colorspace is YUV, the destination image's chroma value is used to select
     the chroma format. If source and target image are in YUV colorspace, but their chroma
     differs, chroma conversion is done just like ChangeChroma().
  */
  void ChangeColorspace(Image<Pixel>& dst, const Image<Pixel>& src,
			Colorspace colorspace, ChromaFormat chroma=Chroma_420);

  /* Same as above, but does not copy the content to a fresh image, if the colorspace
     is already correct. This is faster for cases in which you will not modify the image data.
  */
  const Image<Pixel> ChangeColorspace_NoCopy(const Image<Pixel>& src,
					     Colorspace colorspace, ChromaFormat chroma=Chroma_420);

  /* Get a bitmap of the Y-plane. If 'src' is already greyscale or YUV, the bitmap of this
     image is returned without copying it. If it has a different colorspace, it is converted
     to greyscale.
     Hence, you should not modify the generated destination image.
  */
  const Bitmap<Pixel> AskYPlane(const Image<Pixel>& src);

  // -------- single pixel colorspace conversion routines -------

  // RGB/YUV

  void RGB2YUV(Pixel& y,Pixel& u,Pixel& v, Pixel r,Pixel g,Pixel b);
  void YUV2RGB(Pixel& r,Pixel& g,Pixel& b, Pixel y,Pixel u,Pixel v);

  void RGB2YUV(Image<Pixel>& dst, const Image<Pixel>& src, ChromaFormat chroma = Chroma_Invalid);
  void YUV2RGB(Image<Pixel>& dst, const Image<Pixel>& src);

  // RGB/HSV

  void HSV2RGB(double& r,double& g,double& b, double h,double s,double v);  // input/output range: 0..1 
  void RGB2HSV(double& h,double& s,double& v, double r,double g,double b);  // input/output range: 0..1 

  void RGB2HSV(Image<double>& dst, const Image<Pixel>&  src);
  void HSV2RGB(Image<Pixel>&  dst, const Image<double>& src);
					   
  void RGB2HSV(Image<Pixel>& dst, const Image<Pixel>& src);
  void HSV2RGB(Image<Pixel>& dst, const Image<Pixel>& src);

  // RGB/Greyscale

  void RGB2Greyscale(Image<Pixel>& dst, const Image<Pixel>& src);
  void Greyscale2RGB(Image<Pixel>& dst, const Image<Pixel>& src);

  // YUV/Greyscale

  void YUV2Greyscale(Image<Pixel>& dst, const Image<Pixel>& src);
  void Greyscale2YUV(Image<Pixel>& dst, const Image<Pixel>& src);

}

#endif

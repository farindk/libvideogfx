/*********************************************************************
  color/colorspace.hh

  purpose:
    Routines for chroma format and colorspace convertions.

  notes:

  to do:

  author(s):
   - Dirk Farin, farin@ti.uni-mannheim.de
     University Mannheim, Dept. Circuitry and Simulation
     L 15,16 room 410 / D-68131 Mannheim / Germany

  modifications:
    22/May/2001 - Dirk Farin - added HSV colorspace conversion
    30/Nov/1999 - Dirk Farin - implemented RGB2YUV_444()
    03/Aug/1999 - Dirk Farin - new functions: - TrTo???_DupSub()
                                              - Grey2RGB_Inplace()
    20/Jul/1999 - Dirk Farin - complete rewrite
 *********************************************************************/

#ifndef LIBVIDEOGFX_GRAPHICS_COLOR_COLORSPACE_HH
#define LIBVIDEOGFX_GRAPHICS_COLOR_COLORSPACE_HH

#include "libvideogfx/graphics/basic/image.hh"
#include "libvideogfx/error.hh"


void ChangeChroma    (Image<Pixel>& dst, const Image<Pixel>& src, ChromaFormat dstformat);

/* Copy the image contents of image "src" to image "dst" with a conversion of colorspace.
   If target colorspace is YUV, the destination image's chroma value is used to select
   the chroma format. If source and target image are in YUV colorspace, but their chroma
   differs, chroma conversion is done just like ChangeChroma().
 */
void ChangeColorspace(Image<Pixel>& dst, const Image<Pixel>& src, Colorspace   colorspace);


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

#endif

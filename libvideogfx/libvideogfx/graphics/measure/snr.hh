/*********************************************************************
  libvideogfx/graphics/measure/snr.hh

  purpose:

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
    13/Jul/2001 - Dirk Farin - first implementation
 *********************************************************************/

#ifndef LIBVIDEOGFX_GRAPHICS_MEASURE_SNR_HH
#define LIBVIDEOGFX_GRAPHICS_MEASURE_SNR_HH

#include <libvideogfx/graphics/datatypes/image.hh>

namespace videogfx {

  double CalcMSE(const Bitmap<Pixel>& img1,
		 const Bitmap<Pixel>& img2,
		 int x0= 0,int y0=0,      // rectangle to consider for the calculation
		 int x1=-1,int y1=-1);

  double CalcSNR(const Bitmap<Pixel>& img1,
		 const Bitmap<Pixel>& img2,
		 int x0= 0,int y0=0,      // rectangle to consider for the calculation
		 int x1=-1,int y1=-1);

  double CalcPSNR(const Bitmap<Pixel>& img1,
		  const Bitmap<Pixel>& img2,
		  int x0= 0,int y0=0,     // rectangle to consider for the calculation
		  int x1=-1,int y1=-1);

}


#endif

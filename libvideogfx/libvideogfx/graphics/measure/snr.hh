/*********************************************************************
  libvideogfx/graphics/measure/snr.hh

  purpose:

  notes:

  to do:

  author(s):
   - Dirk Farin, farin@ti.uni-mannheim.de
     University Mannheim, Dept. Circuitry and Simulation
     L 15,16, room 410 / D-68131 Mannheim / Germany

  modifications:
    13/Jul/2001 - Dirk Farin - first implementation
 *********************************************************************/

#ifndef LIBVIDEOGFX_GRAPHICS_MEASURE_SNR_HH
#define LIBVIDEOGFX_GRAPHICS_MEASURE_SNR_HH

#include "libvideogfx/graphics/basic/image.hh"

double CalcMSE(const Bitmap<Pixel>& img1,
	       const Bitmap<Pixel>& img2,
	       int x0= 0,int y0=0,      // rectangle to consider in the image
	       int x1=-1,int y1=-1);

double CalcSNR(const Bitmap<Pixel>& img1,
	       const Bitmap<Pixel>& img2,
	       int x0= 0,int y0=0,      // rectangle to consider in the image
	       int x1=-1,int y1=-1);

double CalcPSNR(const Bitmap<Pixel>& img1,
		const Bitmap<Pixel>& img2,
		int x0= 0,int y0=0,     // rectangle to consider in the image
		int x1=-1,int y1=-1);


#endif

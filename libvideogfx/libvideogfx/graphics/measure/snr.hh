/*********************************************************************
  libvideogfx/graphics/measure/snr.hh

  purpose:

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
    13/Jul/2001 - Dirk Farin - first implementation
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

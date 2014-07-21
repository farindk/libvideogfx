/*********************************************************************
  libvideogfx/graphics/measure/ssim.hh

  purpose:

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmail.com

  modifications:
    21/Jul/2014 - Dirk Farin - first implementation
 ********************************************************************************
    LibVideoGfx - video processing library
    Copyright (C) 2002-2014 Dirk Farin

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

#ifndef LIBVIDEOGFX_GRAPHICS_MEASURE_SSIM_HH
#define LIBVIDEOGFX_GRAPHICS_MEASURE_SSIM_HH

#include <libvideogfx/graphics/datatypes/image.hh>

namespace videogfx {

  class SSIM
  {
  public:
    SSIM() :
      C1(0.01 * 256),
      C2(0.03 * 256),
      r (5), // 11x11 patches
      sigma(1.5)
    { }

    float C1, C2;
    int   r;
    float sigma;

    Bitmap<float> calcSSIM(const Bitmap<Pixel>& img1,
                           const Bitmap<Pixel>& img2);

    float calcMSSIM(const Bitmap<Pixel>& img1,
                    const Bitmap<Pixel>& img2);
  };
}


#endif

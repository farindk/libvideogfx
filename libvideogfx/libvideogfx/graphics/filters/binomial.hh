/*********************************************************************
  libvideogfx/graphics/filters/binomial.hh

  purpose:
    binomial-filters: 1/4 ( 1 2 1 ) inclusive rounding

  notes:

  to do:

  author(s):
   - Dirk Farin and Gerald Kuehne, d.s.farin@tue.nl

  modifications:
    05/Jun/2004 - Dirk Farin - bugfix: rounding
                             - non-decimating lowpass filter
                             - architecture independent fallback
    17/Jul/2001 - Dirk Farin, Gerald Kühne - first implementation
                                             of 3x3-Gauss
 ********************************************************************************
    LibVideoGfx - video processing library
    Copyright (C) 2002  Dirk Farin, Gerald Kuehne

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

#ifndef LIBVIDEOGFX_GRAPHICS_FILTERS_BINOMIAL_HH
#define LIBVIDEOGFX_GRAPHICS_FILTERS_BINOMIAL_HH

#include <libvideogfx/graphics/datatypes/image.hh>

namespace videogfx {

  /* Low-pass filtering is done using this kernel:

     1   /  1  2  1  \
     -- * |  2  4  2  |
     16   \  1  2  1  /

     Processing is 3-4 times faster than scalar C code.

     NOTE: this algorithm expectes an alignment of 8 pixels
     plus a border of 8 pixels.
  */

  void LowPass_Binomial           (Bitmap<Pixel>& dest, const Bitmap<Pixel>& img);
  void LowPass_Binomial_Downsample(Bitmap<Pixel>& dest, const Bitmap<Pixel>& img);

  void LowPass_Binomial           (Bitmap<short>& dest, const Bitmap<short>& img);
}

#endif

/*********************************************************************
  libvideogfx/graphics/filters/gauss_mmx.hh

  purpose:
    gauss low-pass filters, optimized for MMX instructions

  notes:

  to do:

  author(s):
   - Dirk Farin and Gerald Kuehne, dirk.farin@gmx.de
     University of Mannheim, Dept. of Computer Science IV
     L 15,16 room 410 / D-68131 Mannheim / Germany

  modifications:
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

#ifndef LIBVIDEOGFX_GRAPHICS_FILTERS_GAUSS_MMX_HH
#define LIBVIDEOGFX_GRAPHICS_FILTERS_GAUSS_MMX_HH

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
  void LowPass_3x3Gauss_MMX(Bitmap<Pixel>& dest, const Bitmap<Pixel>& img);

}

#endif

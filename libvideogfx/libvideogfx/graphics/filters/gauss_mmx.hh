/*********************************************************************
  libvideogfx/graphics/filters/gauss_mmx.hh

  purpose:
    gauss low-pass filters, optimized for MMX instructions

  notes:

  to do:

  author(s):
   - Dirk Farin and Gerald Kuehne, farin@informatik.uni-mannheim.de
     University Mannheim, Dept. Circuitry and Simulation
     L 15,16 room 410 / D-68131 Mannheim / Germany

  modifications:
    17/Jul/2001 - Dirk Farin, Gerald Kühne - first implementation
                                             of 3x3-Gauss
 *********************************************************************/

#ifndef LIBVIDEOGFX_GRAPHICS_FILTERS_GAUSS_MMX_HH
#define LIBVIDEOGFX_GRAPHICS_FILTERS_GAUSS_MMX_HH

#include <libvideogfx/graphics/datatypes/image.hh>


/* Low-pass filtering is done using this kernel:

    1   /  1  2  1  \
   -- * |  2  4  2  |
   16   \  1  2  1  /

   Processing is 3-4 times faster than scalar C code.

   NOTE: this algorithm expectes an alignment of 8 pixels
         plus a border of 8 pixels.
*/
void LowPass_3x3Gauss_MMX(const Bitmap<Pixel>& img,Bitmap<Pixel>& dest);

#endif

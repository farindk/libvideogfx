/*********************************************************************
  libvideogfx/graphics/visualize/regions.hh

  purpose:
    Visualize image regions.

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
    1999/Nov/02 - Dirk Farin - rewritten because original source
                               was lost :(((
 *********************************************************************/

#ifndef LIBVIDEOGFX_GRAPHICS_VISUALIZE_REGIONS_HH
#define LIBVIDEOGFX_GRAPHICS_VISUALIZE_REGIONS_HH

#include <libvideogfx/types.hh>
#include <libvideogfx/graphics/basic/image.hh>

void DrawRegionBoundaries(Bitmap<Pixel>& bm,const Bitmap<int>& regionmap);
void OverlayRegionBoundaries(Bitmap<Pixel>& bm,const Bitmap<int>& regionmap, Pixel value);
void OverlayAlphaMask(Bitmap<Pixel>& bm,const Bitmap<Pixel>& alphamap, Pixel val, int threshold=128);

#endif


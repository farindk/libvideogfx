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

#ifndef LIBVIDEOGFX_GRAPHICS_VISUALIZE_REGIONS_HH
#define LIBVIDEOGFX_GRAPHICS_VISUALIZE_REGIONS_HH

#include <libvideogfx/types.hh>
#include <libvideogfx/graphics/datatypes/image.hh>

namespace videogfx {

  void DrawRegionBoundaries(Bitmap<Pixel>& bm,const Bitmap<int>& regionmap, bool draw_border=true);
  void OverlayRegionBoundaries(Bitmap<Pixel>& bm,const Bitmap<int>& regionmap, Pixel value);
  void OverlayAlphaMask(Bitmap<Pixel>& bm,const Bitmap<Pixel>& alphamap, Pixel val, int threshold=128);

}

#endif


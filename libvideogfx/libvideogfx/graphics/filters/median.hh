/*********************************************************************
  libvideogfx/graphics/filters/median.hh

  purpose:
   - fast median filter

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
    21/Jul/1999 - Dirk Farin - first implementation
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

#ifndef LIBVIDEOGFX_GRAPHICS_FILTERS_MEDIAN_HH
#define LIBVIDEOGFX_GRAPHICS_FILTERS_MEDIAN_HH

#include <libvideogfx/graphics/datatypes/image.hh>
#include <libvideogfx/graphics/draw/blit.hh>
#include <libvideogfx/containers/array.hh>


namespace videogfx {

  /* Radius is 1 for 3x3 and 2 for 5x5, ...
   */
  template <class Pel> void Median(Bitmap<Pel>& dst,const Bitmap<Pel>& src,int radius=1);
}

#endif

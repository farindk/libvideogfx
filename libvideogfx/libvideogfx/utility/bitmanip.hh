/*********************************************************************
  libvideogfx/utility/bitmanip.hh

  purpose:
    Low-level bit manipulation functions.

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
    27/Jan/2002 - Dirk Farin - first implementation
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

#ifndef LIBVIDEOGFX_UTILITY_BITMANIP_HH
#define LIBVIDEOGFX_UTILITY_BITMANIP_HH

#include "libvideogfx/types.hh"

namespace videogfx {

  void CalcBitsShift(uint32 mask,int& bits,int& shift);   /* mask=000011111000 -> bits=5; shift=3 */
}

#endif

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
 *********************************************************************/

#ifndef LIBVIDEOGFX_UTILITY_BITMANIP_HH
#define LIBVIDEOGFX_UTILITY_BITMANIP_HH

#include "libvideogfx/types.hh"

namespace videogfx {

  void CalcBitsShift(uint32 mask,int& bits,int& shift);   /* mask=000011111000 -> bits=5; shift=3 */
}

#endif

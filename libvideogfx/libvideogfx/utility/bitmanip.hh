/*********************************************************************
  libvideogfx/utility/bitmanip.hh

  purpose:
    Low-level bit manipulation functions.

  notes:

  to do:

  author(s):
   - Dirk Farin, farin@ti.uni-mannheim.de
     University Mannheim, Dept. Circuitry and Simulation
     B 6,26 EG, room 0.10 / D-68131 Mannheim / Germany

  modifications:
    27/Jan/2002 - Dirk Farin - first implementation
 *********************************************************************/

#ifndef LIBVIDEOGFX_UTILITY_BITMANIP_HH
#define LIBVIDEOGFX_UTILITY_BITMANIP_HH

#include "libvideogfx/types.hh"

void CalcBitsShift(uint32 mask,int& bits,int& shift);   /* mask=000011111000 -> bits=5; shift=3 */

#endif

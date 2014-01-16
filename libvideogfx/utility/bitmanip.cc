/********************************************************************************
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

#include "libvideogfx/utility/bitmanip.hh"
#include <assert.h>

namespace videogfx {

void CalcBitsShift(uint32 mask,int& bits,int& shift)
{
  if (mask==0)
    { shift=0; bits=0; return; }

  shift=0;
  while ((mask&1)==0) { shift++; mask>>=1; }
  bits=0;
  while (mask&1) { bits++; mask>>=1; }

  assert(mask==0); /* This may fail if there are more than one continuous
		      sequences of ones (like 000111000011100). */
}

}


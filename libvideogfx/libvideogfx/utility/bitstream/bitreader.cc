/********************************************************************************
    Copyright (C) 1999  Dirk Farin

    This program is distributed under GNU Public License (GPL) as
    outlined in the COPYING file that comes with the source distribution.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ********************************************************************************/

#include "config.h"

#include <iostream>

#include "libvideogfx/types.hh"

#include "bitreader.hh"


BitReader::BitReader(const uint8* buffer,uint32 len)
  : d_buffer(0),
    d_bitsleft(0),
    d_start(buffer),
    d_ptr(buffer),
    d_endptr(buffer+len)
{
}

void BitReader::Refill()
{
#if WORDS_BIGENDIAN
  uint32 val = *((uint32*)d_ptr)++;

  uint64 val64 = val;
  val64 <<= 64-32-d_bitsleft;
  d_buffer |= val64;
  d_bitsleft += 32;
#else

#if CPU_x86
  uint32 val = *((uint32*)d_ptr);
  d_ptr+=4;

  __asm__("bswap %0" : "=r" (val) : "0" (val));

  uint64 val64 = val;
  val64 <<= 64-32-d_bitsleft;
  d_buffer |= val64;
  d_bitsleft += 32;
#else
  int shiftval = 64-8-d_bitsleft;

  while (shiftval>=0)
    {
      uint64 newval = *d_ptr++;
      newval <<= shiftval;
      d_buffer |= newval;
      shiftval  -=8;
   }

  d_bitsleft = 64-8 -shiftval;
#endif
#endif
}

/********************************************************************************
  libvideogfx/utility/bitstream/bitreader.hh

  purpose:
   Implements bit-access to an externally provided memory buffer.

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
   03/Jul/2000 - Dirk Farin
     - new method: SkipBitsFast()
   20/Jun/2000 - Dirk Farin
     - completely new implementation
   30/Sep/1999 - Dirk Farin
     - integrated from old DVDview into ULib
   26/Dec/1998 - Dirk Farin
     - made most methods inline
     - new method: "AskBitsLeft()"
   23/Dec/1998 - Dirk Farin
     - first implementation, not working yet
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

#ifndef LIBVIDEOGFX_UTILITY_BITSTREAM_BITREADER_HH
#define LIBVIDEOGFX_UTILITY_BITSTREAM_BITREADER_HH

#include "libvideogfx/types.hh"

namespace videogfx {

  // #define MemBitstreamReader BitReader

  class BitReader
  {
  public:
    BitReader(const uint8* buffer,uint32 len);

    inline uint32 GetBits (int nbits);
    inline uint32 PeekBits(int nbits);
    inline void   SkipBits(int nbits);
    inline void   SkipBitsFast(int nbits); /* Use ONLY when you called PeekBits() with at
					      least as many bits before! */
    inline void   SkipToByteBoundary();

    inline int32  AskBitsLeft() const; // Return number of bits that have still not been read.

    inline bool   IsEOF() const;       // True iff current cursor position at or behind file end
    inline int    AskPosition() const { return (d_ptr-d_start)*8 -d_bitsleft; }

  private:
    uint64 d_buffer;
    uint32 d_bitsleft;

    const uint8* d_start;
    const uint8* d_ptr;
    const uint8* d_endptr;

    void Refill();
  };

#include "bitreader.icc"

}

#endif

/********************************************************************************
  $Header$

  purpose:
    Memory buffer that can be filled with bit-strings.

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
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

#ifndef LIBVIDEOGFX_UTILITY_BITSTREAM_BITBUFFER_HH
#define LIBVIDEOGFX_UTILITY_BITSTREAM_BITBUFFER_HH

#include <libvideogfx/types.hh>

namespace videogfx {

  class BitBuffer
  {
  public:
    BitBuffer();
    ~BitBuffer();

    void Reset();

    void WriteBits(uint32 bits,int nBits);       // input has to be right aligned
    void WriteBool(bool b) { WriteBits(b ? 1 : 0 , 1); }
    void WriteBitsMasked(uint32 bits,int nBits);
    void AlignToByte0(); // Fill 0-bits until a byte boundary is reached. 0-7 bits are inserted.

    void Flush(); // Fill 0-bits to next byte boundary and make all data available at the output buffer.

    uint8* AskBuffer() const { return d_buffer; }
    uint32 AskBitsInBuffer() const { return d_bufferidx*8 + (32-d_freebits); }
    int    AskLengthInBytes() const { return (AskBitsInBuffer()+7)/8; }

  private:
    uint8* d_buffer;
    int    d_bufferidx;

    int    d_buffersize;

    uint32 d_tmpbuf;
    int    d_freebits;

    void TmpToBuffer();
    void EnlargeIfFull();
  };

}

#endif

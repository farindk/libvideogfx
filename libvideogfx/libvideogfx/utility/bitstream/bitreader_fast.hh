/********************************************************************************
  libvideogfx/utility/bitstream/bitreader_fast.hh

  purpose:
   Implements very fast bit-access to an externally provided memory buffer.

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
   27/Sep/2000 - Dirk Farin
     - first implementation, based on ideas seen in "mpeg2dec"
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

#ifndef LIBVIDEOGFX_UTILITY_BITSTREAM_FASTBITBUF_HH
#define LIBVIDEOGFX_UTILITY_BITSTREAM_FASTBITBUF_HH

#include "libvideogfx/types.hh"

namespace videogfx {

  /* This buffer can work in two modes: a slow "internal" mode, where
     you can directly call PeekBits() & co. for easy access; and a
     fast "external" mode. In external mode, you have to provide
     your own buffer for the next bits to be read (a uint32, and
     a counter of free bits in your buffer). 
     External mode is much faster than internal mode because in
     internal mode, each access to the buffer is made through a
     pointer to the object. Hence, the compiler cannot easily
     move the buffer into a register.
     In external mode, the maximum length of bits that can be
     read in a single call are 16 bits. Before each read operation,
     you have to call NEEDBITS on your buffer. 

     DUMPBITS removes a specified number of bits from the buffer
     (max. 16 bits). The actual bit-data can be accessed
     left-aligned by direct access to you buffer-integer or
     right-aligned by using UBITS or SBITS.

     The external buffer is organized as follows:
     The top 16 bits of the 32 bit buffer should contain valid
     data. "freebits" is equivalent to the number of unused bits
     in the top 16 bits of your buffer.
     Each NEEDBITS fill in 16 bits of new data when freebits>=0
     i.e. at least 16 bits are free.
  */

#define NEEDBITS(bs,bit_buf,freebits) bs.LocalFill16Bits(bit_buf,freebits)

  // remove num valid bits from bit_buf
#define DUMPBITS(bit_buf,bits,num)      \
do {                                    \
    bit_buf <<= (num);                  \
    bits += (num);                      \
} while (0)

  // take num bits from the high part of bit_buf and zero extend them
#define UBITS(bit_buf,num) (((uint32)(bit_buf)) >> (32 - (num)))

  // take num bits from the high part of bit_buf and sign extend them
#define SBITS(bit_buf,num) (((int32)(bit_buf)) >> (32 - (num)))



#define FastBitBuf BitReader_Fast

  class BitReader_Fast
  {
  public:
    BitReader_Fast(const uint8* buffer,uint32 len)
      : d_ptr(buffer), d_endptr(buffer+len), d_start(buffer),
	d_buffer(0), d_freebits(16)
    {
      Fill16Bits();
    }

    // Call this when you want to start reading in "external mode".
    inline void MakeLocalCopy(uint32& buf,int& bits) { buf=d_buffer; bits=d_freebits; }

    // Call this after you finished reading in "external mode" and want to switch
    // to "internal mode" again.
    inline void RestoreFromLocal(uint32 buf,int bits) { d_buffer=buf; d_freebits=bits; Fill16Bits(); }

    // Optionally refill buffer so that there are at least 16 bits available.
    inline void LocalFill16Bits(uint32& buf,int& bits)
    {
      if (bits >= 0)
	{
	  buf |= ((((int)d_ptr[0])<<8)|d_ptr[1])<<bits;
	  d_ptr+=2;
	  bits-=16;
	}
    }

    // Get next 16 bits from input stream.
    inline uint32 GetNextWord()
    {
      uint32 val = (((int)d_ptr[0])<<8) | d_ptr[1];
      d_ptr+=2;
      return val;
    }


    // ---- internal mode only ----

    // Return a 32 bit integer. Next bits are left-aligned.
    inline uint32 Peek16BitsMSB()
    {
      return d_buffer;
    }

    // Peek up to 16 bits.
    inline uint32 PeekBitsFast(int nbits)
    {
      return UBITS(d_buffer,nbits);
    }

    // Skip up to 16 bits.
    inline void   SkipBitsFast(int nbits)
    {
      //assert(nbits<=16);
      //assert(16-d_freebits >= nbits);

      DUMPBITS(d_buffer,d_freebits,nbits);

      Fill16Bits();
    }

    // Assure that there are at least 16 bits available in the buffer.
    inline void   Fill16Bits()
    {
      if (d_freebits >= 0)
	{
	  d_buffer |= ((((int)d_ptr[0])<<8)|d_ptr[1])<<d_freebits;
	  d_ptr+=2;
	  d_freebits-=16;
	}
    }


    // ---------------- Slow methods for compatibility and non-time critical parts ---------------

    // on entry: buffer contains at least 16 bits
    // on exit:  buffer has to contain at least 16 bits

    inline uint32 PeekBits(int nbits)
    {
      //assert(nbits>16 || nbits <= 16-d_freebits);
      //assert(d_freebits <= 0);

      if(nbits>16)
	{
	  // remember old position

	  uint32 copy_buffer = d_buffer;
	  int copy_freebits = d_freebits;
	  const uint8 * copy_ptr = d_ptr;

	  // get top 16 bits

	  uint32 bits = GetBits(16)<<(nbits-16);

	  // combine with lower bits

	  bits |= PeekBits(nbits-16);


	  // restore state

	  d_buffer=copy_buffer;
	  d_freebits = copy_freebits;
	  d_ptr = copy_ptr;

	  return bits;
	}

      uint32 bits = d_buffer>>(32-nbits);
      return bits;
    }

    inline uint32 GetBits(int nbits)
    {
      //assert(nbits>16 || nbits <= 16-d_freebits);
      //assert(d_freebits <= 0);

      if (nbits>16)
	{
	  uint32 bits = GetBits(16)<<(nbits-16);
	  bits |= GetBits(nbits-16);
	  return bits;
	}

      uint32 bits = d_buffer>>(32-nbits);
      d_buffer <<= nbits;
      d_freebits += nbits;

      Fill16Bits();

      return bits;
    }

    inline void   SkipBits(int nbits)
    {
      //assert(nbits>16 || nbits <= 16-d_freebits);
      //assert(d_freebits <= 0);

      if (nbits>16)
	{
	  SkipBits(16);
	  SkipBits(nbits-16);
	  return;
	}

      d_buffer<<=nbits;
      d_freebits += nbits;

      Fill16Bits();
    }
  
    inline int32 AskBitsLeft() const // Return number of bits that have still not been read.
    {
      return (d_endptr-d_ptr)*8 + 16-d_freebits;
    }

    inline bool   IsEOF() const       // True iff current cursor position is at or behind file end
    {
      return AskBitsLeft() <= 0;
    }

    inline int   AskPosition() const { return (d_ptr-d_start)*8 -(16-d_freebits); }
    inline const uint8* AskData() const { return d_start; }

  public:
    uint32 d_buffer;
    int    d_freebits; // number of invalid bits in top 16 bits of buffer

  private:
    const uint8* d_ptr;
    const uint8* d_endptr;
    const uint8* d_start;
  };

}

#endif

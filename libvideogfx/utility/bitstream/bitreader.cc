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

#include "config.h"

#include "libvideogfx/types.hh"
#include "libvideogfx/utility/bitstream/bitreader.hh"


namespace videogfx {

// FILEBUF_SIZE must be a multiple of uint32
#define FILEBUF_SIZE 4096*sizeof(uint32)


  BitReader::BitReader(const uint8* buffer,uint32 len)
    : d_buffer(0),
      d_bitsleft(0),
      d_start(buffer),
      d_ptr(buffer),
      d_endptr(buffer+len),
      d_istr(NULL),
      d_filebuf(NULL)
  {
  }

  BitReader::BitReader(InputStream& istr)
    : d_buffer(0),
      d_bitsleft(0),
      d_istr(&istr)
  {
    d_filebuf = new uint8[FILEBUF_SIZE];
    uint32 len = istr.FillBuffer(d_filebuf,FILEBUF_SIZE);

    d_start =d_filebuf;
    d_ptr   =d_filebuf;
    d_endptr=d_filebuf+len;
  }

  BitReader::~BitReader()
  {
    if (d_filebuf) delete[] d_filebuf;
  }


  void BitReader::Refill32bits()
  {
#if WORDS_BIGENDIAN
    assert(sizeof(uint32)==4);
    uint32 val = *((uint32*)d_ptr)++;

    uint64 val64 = val;
    val64 <<= (64-d_bitsleft)-32;
    d_buffer |= val64;
    d_bitsleft += 32;
#else

#if 0 && CPU_x86
    // disabled this code, because I feel unsure if this is portable between 32bit / 64bit systems
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
	if (d_ptr==d_endptr) break;

	uint64 newval = *d_ptr++;
	newval <<= shiftval;
	d_buffer |= newval;
	shiftval  -=8;
      }

    d_bitsleft = 64-8 -shiftval;
#endif
#endif

    // memory buffer has been read, go on with reading the file

    if (d_ptr == d_endptr && d_istr != NULL)
      {
	uint32 len = d_istr->FillBuffer(d_filebuf,FILEBUF_SIZE);
	d_ptr = d_start;
	d_endptr = d_start+len;
      }
  }


  int32 BitReader::AskBitsLeft() const
  {
    /* If we are reading from a stream, there might be the
       situation that we are reading at the end of the memory
       buffer, but there is still enough data in the file.
       AskBitsLeft(), however, would return too few available bits.
       We solve this problem such that when there are only a few
       bytes left in the buffer, the buffer is filled with new
       data. This allows us to see if there is more data in the
       file. Note that this works also on non-finite streams.
    */
    const int fewbytes=4;

    if (d_istr && (d_endptr-d_ptr)<fewbytes)
      {
	// move remaining bytes to the front

	const uint8* s = d_ptr;
	for (uint8* p = d_filebuf; s<d_endptr; )
	  *p++ = *s++;

	const int remaining = d_endptr-d_ptr;
	d_endptr = d_start+remaining;
	d_ptr    = d_start;

	// now fill in more bytes from the input
	
	uint32 len = d_istr->FillBuffer(d_filebuf+remaining,
					FILEBUF_SIZE - remaining);
	d_endptr += len;
      }
    
    return ((d_endptr-d_ptr)*8) + d_bitsleft;
  }

}

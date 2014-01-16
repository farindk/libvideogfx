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

#include "libvideogfx/utility/bitstream/bitbuffer.hh"
#include "config.h"

#include <assert.h>
#include <string.h>

namespace videogfx {

  const int BytesPerEntry = sizeof(unsigned long);
  const int BitsPerEntry = BytesPerEntry*8;
  const int InitialBufferSize = 256*BytesPerEntry;  // has to be an integer multiple of d_tmpbuf size.

  BitBuffer::BitBuffer()
  {
    d_buffer = new uint8[InitialBufferSize];
    d_buffersize = InitialBufferSize;
    d_bufferidx  = 0;
    d_freebits   = BitsPerEntry;
    d_tmpbuf     = 0;

    assert(BitsPerEntry%8 == 0);
    assert(InitialBufferSize%sizeof(d_tmpbuf)==0);
  }


  BitBuffer::~BitBuffer()
  {
    delete[] d_buffer;
  }


  void BitBuffer::Reset()
  {
    d_bufferidx  = 0;
    d_freebits   = BitsPerEntry;
    d_tmpbuf     = 0;
  }


  void BitBuffer::WriteBitsMasked(uint32 bits,int nBits)
  {
    uint32 long mask=1;
    mask<<=nBits;
    mask--;

    WriteBits(bits & mask,nBits);
  }


  void BitBuffer::WriteBits(uint32 bits,int nBits)
  {
    // cerr << "WriteBits: " << bits << " " << nBits << endl;

#ifndef NDEBUG
    {
      uint32 mask=1;
      mask<<=nBits;
      mask--;

      assert(bits == (bits & mask));
    }
#endif

    /*
      +------+----------------------+
      |XXXXXX|0000000000000000000000|
      +------+----------------------+
      d_freebits

      +----+------------------------+
      |0000|XXXXXXXXXXXXXXXXXXXXXXXX|
      +----+------------------------+
      nBits         
    */

    if (d_freebits>=nBits)
      {
	// new bits fit completely into d_tempbuf

	unsigned long aligned = bits;

	int nShiftLeft = d_freebits - nBits;

	if (nShiftLeft>0)
	  {
	    aligned <<= nShiftLeft;
	    d_tmpbuf |= aligned;
	    d_freebits = nShiftLeft;
	  }
	else
	  {
	    d_tmpbuf |= aligned;

	    TmpToBuffer();

	    d_tmpbuf   = 0;
	    d_freebits = BitsPerEntry;
	  }
      }
    else
      {
	// new bits have to be split in two parts, because they exceed the remaining
	// space in d_tmpbuf

	unsigned long rightpart = bits;   // the part that still fits
	unsigned long leftpart  = bits;   // the part that will be filled into a new buffer

	int nShiftRight = nBits - d_freebits;
	rightpart >>= nShiftRight;

	d_tmpbuf |= rightpart;

	TmpToBuffer();

	int nShiftLeft = BitsPerEntry-nShiftRight;
	leftpart <<= nShiftLeft;
	d_tmpbuf = leftpart;

	d_freebits = nShiftLeft;
      }
  }


  void BitBuffer::AlignToByte0()
  {
    d_freebits -= d_freebits%8;

    if (d_freebits==0)
      {
	TmpToBuffer();
	d_tmpbuf = 0;
	d_freebits = BitsPerEntry;
      }
  }


  void BitBuffer::Flush()
  {
    AlignToByte0();

    EnlargeIfFull();

    assert(d_bufferidx+BytesPerEntry-d_freebits/8 <= d_buffersize);

    while (d_freebits<BitsPerEntry)
      {
	d_buffer[d_bufferidx++] = (uint8)(d_tmpbuf>>(BitsPerEntry-8));
	d_tmpbuf<<=8;
	d_freebits+=8;
      }

    assert(d_freebits == BitsPerEntry);
  }


  void BitBuffer::TmpToBuffer()
  {
    EnlargeIfFull();

    assert(d_bufferidx+BytesPerEntry <= d_buffersize);
  
#ifdef WORDS_BIGENDIAN
    unsigned char* p = (unsigned char*)(&d_tmpbuf);
    for (int j=0;j<BytesPerEntry;j++)
      d_buffer[d_bufferidx++] = p[j];
#else
    unsigned char* p = (unsigned char*)(&d_tmpbuf);
    for (int j=BytesPerEntry-1;j>=0;j--)
      d_buffer[d_bufferidx++] = p[j];
#endif
  }


  void BitBuffer::EnlargeIfFull()
  {
    assert(d_bufferidx <= d_buffersize);

    if (d_bufferidx+BytesPerEntry >= d_buffersize)
      {
	uint8* newbuf = new uint8[d_buffersize*2];
	memcpy(newbuf, d_buffer, d_buffersize);
      
	d_buffersize *= 2;

	delete[] d_buffer;
	d_buffer = newbuf;
      }
  }

}

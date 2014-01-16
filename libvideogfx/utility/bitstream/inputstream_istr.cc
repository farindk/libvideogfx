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

#include "libvideogfx/utility/bitstream/inputstream_istr.hh"

#include <iomanip>
#include <assert.h>

namespace videogfx {

  InputStream_IStream::InputStream_IStream()
    : d_istr(NULL)
  {
  }


  InputStream_IStream::InputStream_IStream(std::istream& istr)
    : d_istr(&istr)
  {
  }

  InputStream_IStream::~InputStream_IStream()
  {
  }


  uint32 InputStream_IStream::MyFillBuffer(uint8* mem,uint32 maxlen)
  {
    assert(d_istr);

    d_istr->read((char*)mem,maxlen);
    return d_istr->gcount();
  }


  bool InputStream_IStream::IsEOF() const
  {
    assert(d_istr);

    if (BytesInPushbackQueue()) return false;

    // Try to read one byte to be sure the EOF flag is correct.

    InputStream_IStream* nonconst = const_cast<InputStream_IStream*>(this);

    uint8 c;
    int cnt = nonconst->MyFillBuffer(&c,1);
    if (cnt==0) return true;
    else nonconst->Pushback(&c,1);

    return d_istr->eof();
  }


  uint64 InputStream_IStream::AskStreamLength() const
  {
    assert(d_istr);

    int64 currentpos; currentpos = d_istr->tellg();
    d_istr->seekg(0,std::ios::end);
    int64 length; length = d_istr->tellg();
    d_istr->seekg(currentpos);

    return length;
  }


  uint64 InputStream_IStream::AskPosition() const
  {
    return uint64(d_istr->tellg()) - BytesInPushbackQueue();
  }


  uint64 InputStream_IStream::SetPosition(uint64 pos)
  {
    uint64  current_pos = AskPosition();

    if (pos >= current_pos &&                        // move forward in the stream
	pos-current_pos <= BytesInPushbackQueue())   // we are still in pushback queue
      {
	RemoveBytesFromPushbackQueue(pos-current_pos);
	return pos;
      }

    ClearPushbackQueue();

    d_istr->seekg((unsigned long)pos);
    return pos;
  }

}

/********************************************************************************
    LibVideoGfx
    Copyright (C) 2002  Dirk Farin

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 ********************************************************************************/

#include "libvideogfx/utility/bitstream/inputstream.hh"
#include <libvideogfx/error.hh>

#include <algorithm>
#include <string.h>

namespace videogfx {
  using namespace std;


  const int c_InitialPushbackStackSize = 100;


  InputStream::InputStream()
  {
    d_pushback_stack = new uint8[c_InitialPushbackStackSize];
    d_pushback_length = 0;
    d_pushback_size = c_InitialPushbackStackSize;
  }


  InputStream::~InputStream()
  {
    delete[] d_pushback_stack;
  }


  uint32 InputStream::FillBuffer(uint8* mem,uint32 maxlen,uint32 minlen)
  {
    uint8* p = mem;
    int    n_from_stack = 0;  // amount of data read from stack
    int    n_new = 0;         // amount of data read from input

    if (d_pushback_length>0)
      {
	n_from_stack = min(maxlen,d_pushback_length);

	for (int i=0;i<n_from_stack;i++)
	  *p++ = d_pushback_stack[i];

	for (uint32 i=0; i < d_pushback_length-n_from_stack ; i++)
	  d_pushback_stack[i] = d_pushback_stack[i+n_from_stack];

	d_pushback_length -= n_from_stack;

	if (n_from_stack == (int)maxlen)
	  return maxlen;
      }

    n_new = MyFillBuffer(p,maxlen-n_from_stack);
    p += n_new;

    int n_total_read = n_from_stack + n_new;

    if (n_total_read < (int)minlen)
      {
	/* If we read not the full amount of data, read more data until
	   the input ends. */

	while (n_total_read < (int)minlen && !IsEOF())
	  {
	    int size = MyFillBuffer(p, maxlen-n_total_read);
	    p += size;
	    n_total_read += size;
	  }
      }

    return n_total_read;
  }


  void   InputStream::Pushback(uint8* mem,int n_bytes)
  {
    if (d_pushback_length + n_bytes > d_pushback_size)
      {
	int newsize = max(d_pushback_size*2, d_pushback_length+n_bytes);

	uint8* newstack = new uint8[newsize];
	memcpy(newstack, d_pushback_stack, d_pushback_length);

	delete[] d_pushback_stack;
	d_pushback_stack = newstack;
	d_pushback_size  = newsize;
      }

    // move stack content to end to make space for new data

    Assert(d_pushback_length + n_bytes <= d_pushback_size);

    for (int i=d_pushback_length-1;i>=0;i--)
      d_pushback_stack[i+n_bytes] = d_pushback_stack[i];

    // copy new data to beginning of stack

    memcpy(&d_pushback_stack[0], mem, n_bytes);
    d_pushback_length += n_bytes;
  }


  void    InputStream::RemoveBytesFromPushbackQueue(int n)
  {
    Assert(n <= (int)d_pushback_length);
    d_pushback_length -= n;

    for (uint32 i=0;i<d_pushback_length;i++)
      d_pushback_stack[i] = d_pushback_stack[i+n];
  }

}

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


#include "libvideogfx/utility/bitstream/bytebuffer.hh"
#include "libvideogfx/utility/refcntr.hh"
#include "libvideogfx/error.hh"

#include <string.h>

namespace videogfx {

  struct ByteBufferParams : public ReferenceCntr
  {
    ByteBufferParams(int InitialSize_Hint=10000,
		     int PoolSize=10);
    ~ByteBufferParams() { }

  private:
    uint32          initialsize_hint;
    MemoryAllocator memalloc;

    // Collect information about how large these buffers usually get.
    // This helps to allocate buffers that are most of the time big enough
    // to hold all the data in them.
    unsigned int EstimatedSize;
    unsigned int LargestSize;

    // Initial buffer size if we did not collect a significant amount of
    // statistics yet.
    static const unsigned int c_InitialBufferSize;

    friend class ByteBuffer;
  };



  const unsigned int ByteBufferParams::c_InitialBufferSize = 10*1024-64;


  ByteBufferParams::ByteBufferParams(int InitialSize_Hint,int poolsize)
    : initialsize_hint(InitialSize_Hint),
      memalloc(InitialSize_Hint,poolsize),
      EstimatedSize(c_InitialBufferSize),
      LargestSize(0)
  {
  }


  ByteBufferPool::ByteBufferPool()
  {
    d_param = new ByteBufferParams;
    d_param->IncrRef();
  }

  ByteBufferPool::~ByteBufferPool()
  {
    d_param->DecrRef();
    if (d_param->RefCntr()==0)
      delete d_param;
  }


  // This factor defines what fraction of the new estimation will be taken from the
  // old estimation.
#define AgingFactor 15:16


  ByteBuffer::ByteBuffer(ByteBufferPool* p)
    : d_buf_from_pool(false)
  {
    if (p) d_param = p->d_param;
    else   d_param = NULL;

    d_buf  = NULL;
    d_len  = 0;
    d_size = 0;

    if (d_param)
      {
	d_param->IncrRef();

	if (d_param->initialsize_hint!=0)
	  {
	    d_buf  = (unsigned char*)(d_param->memalloc.Alloc((int)d_param->initialsize_hint, &d_size));
	    Assert(d_buf);
	    d_len  = 0;
	    d_buf_from_pool = true;
	  }
      }
  }


  ByteBuffer::~ByteBuffer()
  {
    if (d_buf)
      {
	if (d_buf_from_pool)
	  d_param->memalloc.Free(d_buf);
	else
	  delete[] d_buf;

	// Update statistics.

	if (d_param)
	  {
	    d_param->EstimatedSize =
	      // old fraction
	      (d_param->EstimatedSize * (1?AgingFactor) +
	       // new fraction
	       d_len * ((0?AgingFactor)-(1?AgingFactor)))
	      / (0?AgingFactor);

	    if (d_len > d_param->LargestSize) d_param->LargestSize=d_len;
	  }
      }

    if (d_param)
      {
	d_param->DecrRef();
	if (d_param->RefCntr()==0)
	  delete d_param;
      }
  }


  void ByteBuffer::AttachToPool(ByteBufferPool* pool)
  {
    assert(pool);

    pool->d_param->IncrRef();
    if (d_param)
      {
	d_param->DecrRef();
	if (d_param->RefCntr()==0)
	  delete d_param;
      }

    d_param = pool->d_param;
  }


  unsigned char* ByteBuffer::GetPtrToAppendToBuffer(unsigned int len)
  {
    Assert(len>0);

    // If contents will not fit into the buffer, enlarge buffer.
    if (d_len+len > (uint32)d_size)
      {
	unsigned int newsize=d_size;

	// No buffer allocated so far, use estimated size.
	if (newsize==0 && d_param)       newsize = d_param->EstimatedSize;

	// If this is not enough, use larger buffer.
	if (newsize<d_len+len && d_param && d_param->LargestSize>0)  newsize = d_param->LargestSize;

	if (newsize==0) newsize=1000;

	// If still not enough, double size until big enough.
	// This will allocate buffers that are far too big in some cases.
	// But allocating just the size that is needed will result in poor
	// performance when appending many small portions of data.
	while (newsize<d_len+len) newsize *= 2;


	// Allocate new buffer and copy old buffer contents to new buffer.

	unsigned char* newbuf;
	int newlen;
	bool new_from_pool = false;
	if (d_param)
	  {
	    newbuf = (unsigned char*)(d_param->memalloc.Alloc(newsize,&newlen));
	    new_from_pool = true;
	  }
	else
	  {
	    newbuf = new unsigned char[newsize];
	    newlen = newsize;
	  }

	Assert(newbuf); // Memory error handler should have caught error.
	if (d_len>0) { memcpy(newbuf,d_buf,d_len); }

	if (d_buf_from_pool)
	  {
	    // The old buffer can now be replaced with the new one and be deleted.
	    if (d_buf) { d_param->memalloc.Free(d_buf); }
	  }
	else
	  {
	    // The old buffer can now be replaced with the new one and be deleted.
	    if (d_buf) { delete[] d_buf; }
	  }

	d_buf=newbuf;
	d_size=newlen;
	d_buf_from_pool = new_from_pool;
      }

    // There has to be enough memory left now.
    Assert(d_len+len <= (uint32)d_size);

    // Return pointer, pointing behind the last already filled byte in the buffer and
    // enlarge the buffer contents size variable.
    unsigned char* newdatastart = &d_buf[d_len];
    d_len += len;

    return newdatastart;
  }


  void ByteBuffer::TruncateBuffer(unsigned int nBytes)
  {
    Assert(nBytes <= d_len);
    d_len -= nBytes;
  }


  void ByteBuffer::TruncateBufferAtFront(unsigned int nBytes)
  {
    if (nBytes==0)
      return;

    Assert(nBytes <= d_len);

    for (int i=0;i<d_len-nBytes;i++)
      d_buf[i] = d_buf[i+nBytes];

    d_len -= nBytes;
  }


  void ByteBuffer::AppendBytes(unsigned char* mem,unsigned int len)
  {
    if (len>0)
      {
	unsigned char* buffermem = GetPtrToAppendToBuffer(len);

	// Append new contents behind current buffer data.

	memcpy(buffermem,mem,len);
      }
  }

}

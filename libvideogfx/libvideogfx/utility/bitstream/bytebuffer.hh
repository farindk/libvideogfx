/********************************************************************************
  $Header$

  purpose:
   Each ByteBuffer encapsulates a dynamically allocated memory area
   to which an arbitrary amount of bytes may be appended. It
   automatically enlarges if more data is appended than the current
   buffer can hold.

  notes:
  1) You must not acquire several pointers using GetPtrToAppendToBuffer() to
     fill the associated memory area later. Strictly speaking you have to get
     the pointer and fill the memory area before you call any non-const method
     with this ByteBuffer-object again.

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
   28/Sep/1999 - Dirk Farin
     - Memory is now allocated via a MemoryAllocator object for better performance.
     - ByteBuffer statistics are now held externally in a ByteBufferParams
       structure that can be shared across multiple ByteBuffer objects.
   18/Dec/1998 - Dirk Farin
     - Changed several int-types to unsigned to eliminate compiler warnings.
   20/Nov/1998 - Dirk Farin
     - introduced TruncateBuffer()
   15/Nov/1998 - Dirk Farin
     - first implementation
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

#ifndef LIBVIDEOGFX_UTILITY_BYTEBUFFER_HH
#define LIBVIDEOGFX_UTILITY_BYTEBUFFER_HH

#include <libvideogfx/types.hh>
#include <libvideogfx/utility/fastalloc.hh>

namespace videogfx {

  class ByteBufferParams;

  /* Each ByteBufferPool manages a set of pre-allocated memory segments. */
  class ByteBufferPool
  {
  public:
    ByteBufferPool();
    ~ByteBufferPool();

    friend class ByteBuffer;

  private:
    ByteBufferParams* d_param;
  };


  class ByteBuffer
  {
  public:
    ByteBuffer(ByteBufferPool* pool = NULL);
    ~ByteBuffer();

    /* Take memory from the specified memory pool.
       Try to use different pools when requested buffer sizes vary heavily. */
    void AttachToPool(ByteBufferPool*);

    /* Append additional data to the buffer. */
    void AppendBytes(unsigned char* mem,unsigned int len);

    /* Get a pointer to a memory area where new bytes can be filled in.
       The advantage of using this method is that no memcpy is required. */
    unsigned char* GetPtrToAppendToBuffer(unsigned int len); // see note 1)

    void TruncateBuffer(unsigned int nBytes); // Throw away last 'nBytes' bytes of buffer.
    void Clear() { d_len=0; }  // Clear buffer contents but do not free memory.

    unsigned char* AskData() const { return d_buf; }
    int            AskLength() const { return d_len; }

  private:
    unsigned char* d_buf;
    unsigned int d_len;  // Amount of data in the buffer.
    int d_size; // Total size of allocated buffer memory.
    bool d_buf_from_pool;

    ByteBufferParams* d_param;
  };

}

#endif

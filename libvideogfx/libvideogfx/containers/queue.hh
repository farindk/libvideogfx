/********************************************************************************
  utility/queue.hh

  purpose:
    Queue container class template.

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
   29/Sep/1999 - Dirk Farin
     - integrated into libvideogfx CVS
   20/Nov/1998 - Dirk Farin
     - new method: AskAndRemoveHead()
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

#ifndef LIBVIDEOGFX_UTILITY_QUEUE_HH
#define LIBVIDEOGFX_UTILITY_QUEUE_HH

#include "libvideogfx/types.hh"
#include <assert.h>
#include <algorithm>

namespace videogfx {

  template <class T> class Queue
  {
  public:
    Queue();
    ~Queue();

    void Append(const T&);
    void AppendAtHead(const T&);
    T&   AskHead() const { assert(!IsEmpty()); assert(d_first<d_size); return d_queue[d_first]; }
    T&   AskTail() const;
    T&   AskFromHead(uint32 idx) const;  // 0 is first one, 1 is second one
    T&   AskFromTail(uint32 idx) const;  // 0 is last one,  1 is last but one
    void RemoveHead();
    void RemoveFromHead(uint32 idx);
    T    AskAndRemoveHead() { T& x=AskHead(); RemoveHead(); return x; }

    bool   IsEmpty() const { return d_entries==0; }
    uint32 AskSize() const { return d_entries;    }

  private:
    T*  d_queue;
    int d_size;

    int d_first;
    int d_entries;

    void Enlarge();
  };

#include "libvideogfx/containers/queue.icc"

}

#endif

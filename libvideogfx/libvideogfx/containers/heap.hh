/********************************************************************************
  utility/heap.hh

  purpose:
    Heap container class template.

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
   05/Jan/2000 - Dirk Farin
     - first revision
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

#ifndef LIBVIDEOGFX_UTILITY_HEAP_HH
#define LIBVIDEOGFX_UTILITY_HEAP_HH

#include <assert.h>

#include "libvideogfx/types.hh"

namespace videogfx {

#if 0
  class HeapElement
  {
  public:
    virtual ~HeapElement() { }

    /* The value is used as the head ordering criterion. The smallest value
       will be at the top of the head. */
    //virtual int AskValue() const = 0;
    virtual bool SmallerThan(const HeapElement& e) const = 0;
  };
#endif

  /* class T must have "operator<".
   */
  template <class T> class Heap
  {
  public:
    Heap();
    ~Heap();

    void Insert(const T&);
    T&   AskRoot() const { assert(d_entries>0); return d_heap[1]; }
    // T&   AskTop() const { assert(d_entries>0); return d_heap[1]; }   Deprecated: use AskRoot()
    void RemoveRoot(); // Remove minimum element.
    //void RemoveTop(); // Remove minimum element.    Deprecated: use RemoveRoot()
    void Reset() { d_entries=0; } // Remove all elements.

    bool   IsEmpty() const { return d_entries==0; }
    uint32 AskSize() const { return d_entries;    }

    void SortHeap();  // TODO: without a function like FillHeap(), this function does not make sense

  private:
    T*  d_heap;
    int d_size; // Size of heap excluding the first (dummy) element

    int d_entries;

    void UpHeap(int i);
    void DownHeap(int i);
  };

#include "libvideogfx/containers/heap.icc"
}

#endif

/********************************************************************************
  libvideogfx/containers/symmatrix.hh

  purpose:
    Memory efficient symmetric matrix.

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
   25/Feb/2004 - Dirk Farin
     - first implementation
 ********************************************************************************
    LibVideoGfx - video processing library
    Copyright (C) 2004  Dirk Farin

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

#ifndef LIBVIDEOGFX_CONTAINERS_SYMMATRIX_HH
#define LIBVIDEOGFX_CONTAINERS_SYMMATRIX_HH

#include "libvideogfx/types.hh"
#include <assert.h>
#include <algorithm>

namespace videogfx {

  template <class T> class SymMatrix
  {
  public:
    SymMatrix() : d_array(NULL), d_size(0) { }
    SymMatrix(int size) : d_array(NULL) { Create(size); }
    ~SymMatrix() { if (d_array) delete[] d_array; }

    void Create(int size)
    {
      if (d_array) delete[] d_array;

      d_array = new T[ size*(size+1)/2 ];
      d_size  = size;
    }

    bool IsInitialized() const { return d_array != NULL; }

    int AskSize()  const { return d_size; }

    T& Ask(int y,int x)
    {
      if (x<y) return Ask(x,y);
      return d_array[y*d_size - y*(y-1)/2 + x];
    }

    const T& Ask(int y,int x) const
    {
      if (x<y) return Ask(x,y);
      return d_array[y*d_size - y*(y-1)/2 + x];
    }

    const SymMatrix& operator=(const SymMatrix& a)
    {
      int s = a.AskSize();
      Create(s);

      int len = s*(s+1)/2;

      for (int i=0;i<s;i++)
	d_array[i] = a.d_array[i];

      return *this;
    }

  private:
    int  d_size;
    T*   d_array;
  };
}

#endif

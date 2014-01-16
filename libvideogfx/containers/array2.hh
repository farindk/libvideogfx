/********************************************************************************
  libvideogfx/containers/array2.hh

  purpose:
    Simple 2dim array class template.

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
   30/Sep/1999 - Dirk Farin
     - integrated from old DVDview into ULib
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

#ifndef LIBVIDEOGFX_CONTAINERS_ARRAY2_HH
#define LIBVIDEOGFX_CONTAINERS_ARRAY2_HH

#include "libvideogfx/types.hh"
#include <assert.h>
#include <algorithm>

namespace videogfx {

  template <class T> class Array2
  {
  public:
    Array2();
    Array2(int width,int height,int xbase=0,int ybase=0);
    ~Array2();

    void Create(int width,int height,int xbase=0,int ybase=0);
    void Resize(int newwidth,int newheight);

    bool IsInitialized() const { return d_array != NULL; }

    int AskWidth()  const { return d_width;  }
    int AskHeight() const { return d_height; }
    int AskXBase()  const { return d_xbase;  }
    int AskYBase()  const { return d_ybase;  }

    T& Ask(int y,int x)
    {
      x+= d_xbase;
      y+= d_ybase;

      assert(IsInitialized());
      assert(x>=0); assert(x<d_width);
      assert(y>=0); assert(y<d_height);
      return d_array[y][x];
    }

    const T& Ask(int y,int x) const
    {
      x+= d_xbase;
      y+= d_ybase;

      assert(IsInitialized());
      assert(x>=0); assert(x<d_width);
      assert(y>=0); assert(y<d_height);
      return d_array[y][x];
    }

    T* operator[](int y)       { return &d_array[y+d_ybase][d_xbase]; }
    const T* operator[](int y) const { return &d_array[y+d_ybase][d_xbase]; }

    const Array2<T>& operator=(const Array2<T>& a)
    {
      int w = a.AskWidth();
      int h = a.AskHeight();
      int x0 = a.AskXBase();
      int y0 = a.AskYBase();
      Create(w,h, x0,y0);

      for (int y=0;y<h;y++)
	for (int x=0;x<w;x++)
	  d_array[y][x] = a.d_array[y][x];

      return *this;
    }

  private:
    int   d_width,d_height;
    int   d_xbase,d_ybase;
    T**   d_array;
  };

#include "libvideogfx/containers/array2.icc"
}

#endif

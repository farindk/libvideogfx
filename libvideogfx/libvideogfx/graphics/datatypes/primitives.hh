/********************************************************************************
  libvideogfx/graphics/datatypes/primitives.hh

  purpose:
    Very basic data types.

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
    17/Nov/1999 - Dirk Farin - first revision
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

#ifndef LIBVIDEOGFX_GRAPHICS_DATATYPES_PRIMITIVES_HH
#define LIBVIDEOGFX_GRAPHICS_DATATYPES_PRIMITIVES_HH

namespace videogfx {

  template <class T> struct Point2D
  {
    T x,y;
  };

  template <class T> struct Rect2D
  {
    Point2D<T> upperleft;
    Point2D<T> lowerright;
  };

  template <class T> struct Color
  {
    Color() { }
    Color(T v1,T v2,T v3,T alpha) { c[0]=v1; c[1]=v2; c[2]=v3; c[3]=alpha; }
    Color(T v1,T v2,T v3)         { c[0]=v1; c[1]=v2; c[2]=v3; }
    Color(int v1,int v2,int v3,int alpha) { c[0]=(T)v1; c[1]=(T)v2; c[2]=(T)v3; c[3]=(T)alpha; }
    Color(int v1,int v2,int v3)           { c[0]=(T)v1; c[1]=(T)v2; c[2]=(T)v3; }

    T c[4];  // 0,1,2 - color | 3 - alpha
  };

}

#endif

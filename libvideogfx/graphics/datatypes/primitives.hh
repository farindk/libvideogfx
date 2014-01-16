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

#include <math.h>
#include <algorithm>


namespace videogfx {

  template <class T> struct Point2D
  {
    Point2D() { }
    Point2D(const T& _x,const T& _y) { x = _x; y = _y; }

    Point2D<T> operator*(double scale) const { return Point2D<T>(x*scale,y*scale); }

    bool operator==(const Point2D<T>& p) const { return p.x==x && p.y==y; }
    bool operator!=(const Point2D<T>& p) const { return p.x!=x || p.y!=y; }

    void shiftByOffset(T ox,T oy) { x+=ox; y+=oy; }

    T x,y;
  };

  template <class T> T SquaredDistance(const Point2D<T>& p1,const Point2D<T>& p2)
  {
    return hypot(p1.x-p2.x, p1.y-p2.y);
  }

  template <class T> struct Vector2D
  {
    Vector2D() { }
    Vector2D(const T& _x,const T& _y) { x = _x; y = _y; }

    T x,y;

    T Length() const { return (T)hypot((double)x,(double)y); }
  };

  template <class T> struct Rect2D
  {
    Point2D<T> upperleft;
    Point2D<T> lowerright;

    int& left()   { return upperleft.x; }
    int& top()    { return upperleft.y; }
    int& right()  { return lowerright.x; }
    int& bottom() { return lowerright.y; }

    int left()   const { return upperleft.x; }
    int top()    const { return upperleft.y; }
    int right()  const { return lowerright.x; }
    int bottom() const { return lowerright.y; }

    int width()  const { return right()-left()+1; }
    int height() const { return bottom()-top()+1; }

    void initEmpty(T w,T h)
    {
      left()  = w;
      right() = 0;
      top()   = h;
      bottom()= 0;
    }

    bool isEmpty() const
    {
      return left()>right() || top()>bottom();
    }

    void extendToIncludePoint(T x,T y)
    {
      left() = std::min(left(),x);
      right()= std::max(right(),x);
      top()  = std::min(top() ,y);
      bottom()=std::max(bottom(),y);
    }

    void extendToIncludeRect(const Rect2D<T>& r)
    {
      left()  = std::min(left()  ,r.left());
      right() = std::max(right() ,r.right());
      top()   = std::min(top()   ,r.top());
      bottom()= std::max(bottom(),r.bottom());
    }

    void shiftByOffset(int x,int y)
    {
      left() += x;
      right() += x;
      top() += y;
      bottom() += y;
    }

    void inflate(int x,int y)
    {
      left()   -= x;
      right()  += x;
      top()    -= y;
      bottom() += y;
    }

    void inflate(int width)
    {
      inflate(width,width);
    }

    void crop(int w,int h)
    {
      if (left()<0) left()=0;
      if (top()<0)  top()=0;
      if (bottom()>=h) bottom()=h-1;
      if (right()>=w)  right()=w-1;
    }

    void crop(const Rect2D<T>& roi)
    {
      left()  = std::max(roi.left(), left());
      top()   = std::max(roi.top(),  top());
      right() = std::min(roi.right(), right());
      bottom()= std::min(roi.bottom(),bottom());
    }
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

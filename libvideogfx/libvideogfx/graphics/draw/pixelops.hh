/*********************************************************************
  libvideogfx/graphics/draw/pixelops.hh

  purpose:
    Single pixel operations that are applied to full bitmaps.

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
   30/Jan/2002 - Dirk Farin - adapted the interface
   27/Apr/2001 - Dirk Farin - first implementation
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

#ifndef LIBVIDEOGFX_GRAPHICS_DRAW_PIXELOPS_HH
#define LIBVIDEOGFX_GRAPHICS_DRAW_PIXELOPS_HH

#include <libvideogfx/graphics/datatypes/bitmap.hh>

namespace videogfx {

  template <class T> void MakeAbsolute(Bitmap<T>&);
  template <class T> void Inverse(Bitmap<T>&,T maxval);
  template <class T> void AddValues(Bitmap<T>& dest,const Bitmap<T>& a,const Bitmap<T>& b);
  template <class T> void Add(Bitmap<T>& dest,T val);
  template <class T> void HalfPlusOffset(Bitmap<T>& bm,T offset); // p = p/2+offset

  /* dest = a-b */
  template <class T> void AbsoluteDifference(Bitmap<T>& dest,const Bitmap<T>& a,const Bitmap<T>& b);
  template <class T> void ContrastBrightness(Bitmap<T>& bm,double contrast,double brightness);
  template <class T> void ClipValues(Bitmap<T>& bm,T low,T high);
  template <class T> void SearchForMinMaxValues(const Bitmap<T>& bm,T& low,T& high);
  template <class T> void StretchValues(Bitmap<T>& bm,T low,T high);
  template <class T> void BinaryThreshold(Bitmap<T>& bm,T thresh,T setlow,T sethigh); // setlow if bm[]<thresh else sethigh

  template <class Pel> void ModuloRange(Bitmap<Pel>& bm, Pel mini, Pel maxi);



  // --------------------- implementation ------------------------

  template <class T> void MakeAbsolute(Bitmap<T>& bm)
  {
    T*const* p = bm.AskFrame();

    const int w = bm.AskWidth();
    const int h = bm.AskHeight();

    for (int y=0;y<h;y++)
      for (int x=0;x<w;x++)
	p[y][x] = abs(p[y][x]);
  }

  template <class T> void Inverse(Bitmap<T>& bm,T maxval)
  {
    const int w = bm.AskWidth();
    const int h = bm.AskHeight();

    T*const* p = bm.AskFrame();

    for (int y=0;y<h;y++)
      for (int x=0;x<w;x++)
	p[y][x] = maxval-p[y][x];
  }

  template <class T> void Add(Bitmap<T>& dest,T val)
  {
    const int w = dest.AskWidth();
    const int h = dest.AskHeight();

    dest.Create(w,h);

    T*const* pd = dest.AskFrame(); // get this pointer first, in case pd==pa or pd==pb

    for (int y=0;y<h;y++)
      for (int x=0;x<w;x++)
	{
	  pd[y][x] += val;
	}
  }

  template <class T> void AddValues(Bitmap<T>& dest,const Bitmap<T>& a,const Bitmap<T>& b)
  {
    const int w = a.AskWidth();
    const int h = a.AskHeight();

    dest.Create(w,h);

    T*const* pd = dest.AskFrame(); // get this pointer first, in case pd==pa or pd==pb
    const T*const* pa = a.AskFrame();
    const T*const* pb = b.AskFrame();

    for (int y=0;y<h;y++)
      for (int x=0;x<w;x++)
	{
	  pd[y][x] = (pa[y][x]+pb[y][x]); /// TODO
	}
  }

  template <class T> void AbsoluteDifference(Bitmap<T>& dest,const Bitmap<T>& a,const Bitmap<T>& b)
  {
    const int w = a.AskWidth();
    const int h = a.AskHeight();

    dest.Create(w,h);

    T*const* pd = dest.AskFrame(); // get this pointer first, in case pd==pa or pd==pb
    const T*const* pa = a.AskFrame();
    const T*const* pb = b.AskFrame();

    for (int y=0;y<h;y++)
      for (int x=0;x<w;x++)
	{
	  pd[y][x] = abs(pa[y][x]-pb[y][x]); /// TODO
	}
  }

  template <class T> void ContrastBrightness(Bitmap<T>& bm,double contrast,double brightness)
  {
    T*const* p = bm.AskFrame();

    const int w = bm.AskWidth();
    const int h = bm.AskHeight();

    for (int y=0;y<h;y++)
      for (int x=0;x<w;x++)
	p[y][x] = (T)(contrast*p[y][x]+brightness);
  }

  template <class T> void HalfPlusOffset(Bitmap<T>& bm,T offset)
  {
    T*const* p = bm.AskFrame();

    const int w = bm.AskWidth();
    const int h = bm.AskHeight();

    for (int y=0;y<h;y++)
      for (int x=0;x<w;x++)
	p[y][x] = (T)(p[y][x]/2+offset);
  }

  template <class T> void ClipValues(Bitmap<T>& bm,T low,T high)
  {
    T*const* p = bm.AskFrame();

    const int w = bm.AskWidth();
    const int h = bm.AskHeight();

    for (int y=0;y<h;y++)
      for (int x=0;x<w;x++)
	{
	  if (p[y][x] < low) p[y][x]=low;
	  else if (p[y][x] > high) p[y][x]=high;
	}
  }


  template <class T> void SearchForMinMaxValues(const Bitmap<T>& bm,T& low,T& high)
  {
    const T*const* p = bm.AskFrame();

    const int w = bm.AskWidth();
    const int h = bm.AskHeight();

    low=high=p[0][0];

    for (int y=0;y<h;y++)
      for (int x=0;x<w;x++)
	{
	  if (p[y][x] < low)  low =p[y][x];
	  if (p[y][x] > high) high=p[y][x];
	}
  }


  template <class T> void StretchValues(Bitmap<T>& bm,T low,T high)
  {
    T mini,maxi;
    SearchForMinMaxValues(bm,mini,maxi);

    ContrastBrightness(bm,1.0,-mini);
    maxi -= mini;
    mini  = 0;
    ContrastBrightness(bm,((double)(high-low))/maxi,0.0);
    maxi = high-low;
    mini = 0;

    if (low!=0) ContrastBrightness(bm,1.0,low);
    maxi = high;
    mini = low;
  }

  template <class T> void BinaryThreshold(Bitmap<T>& bm,T thresh,T setlow,T sethigh)
  {
    int w=bm.AskWidth(), h=bm.AskHeight();

    Pixel*const* p = bm.AskFrame();

    for (int y=0;y<h;y++)
      for (int x=0;x<w;x++)
	{
	  if (p[y][x] < thresh) p[y][x] = setlow;
	  else                  p[y][x] = sethigh;
	}
  }

  template <class Pel>
  void ModuloRange(Bitmap<Pel>& bm, Pel mini, Pel maxi)
  {
    Pel range = maxi-mini;

    Pel*const* p = bm.AskFrame();
    int w = bm.AskWidth();
    int h = bm.AskHeight();

    for (int y=0;y<h;y++)
      for (int x=0;x<w;x++)
	{
	  while (p[y][x] <  mini) p[y][x] += range;
	  while (p[y][x] >= maxi) p[y][x] -= range;
	}
  }

}

#endif

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

#include "libvideogfx/graphics/filters/median.hh"

#include <math.h>
#include <iostream>
#include <algorithm>

using namespace std;

namespace videogfx {

  /*
   *  This Quickselect routine is based on the algorithm described in
   *  "Numerical recipes in C", Second Edition,
   *  Cambridge University Press, 1992, Section 8.5, ISBN 0-521-43108-5
   *  This code by Nicolas Devillard - 1998. Public domain.
   */
  template <class Pel> Pel QuickSelect(Pel* arr, int n)
  {
    int low, high ;
    int median;
    int middle, ll, hh;

    low = 0 ; high = n-1 ; median = (low + high) / 2;
    for (;;) {
      if (high <= low) /* One element only */
	return arr[median] ;

      if (high == low + 1) {  /* Two elements only */
	if (arr[low] > arr[high])
	  swap(arr[low], arr[high]) ;
	return arr[median] ;
      }

      /* Find median of low, middle and high items; swap into position low */
      middle = (low + high) / 2;
      if (arr[middle] > arr[high])    swap(arr[middle], arr[high]) ;
      if (arr[low] > arr[high])       swap(arr[low], arr[high]) ;
      if (arr[middle] > arr[low])     swap(arr[middle], arr[low]) ;

      /* Swap low item (now in position middle) into position (low+1) */
      swap(arr[middle], arr[low+1]) ;

      /* Nibble from each end towards middle, swapping items when stuck */
      ll = low + 1;
      hh = high;
      for (;;) {
        do ll++; while (arr[low] > arr[ll]) ;
        do hh--; while (arr[hh]  > arr[low]) ;

        if (hh < ll)
	  break;

        swap(arr[ll], arr[hh]) ;
      }

      /* Swap middle item (in position low) back into correct position */
      swap(arr[low], arr[hh]) ;

      /* Re-set active partition */
      if (hh <= median)
        low = ll;
      if (hh >= median)
        high = hh - 1;
    }
  }


  /*----------------------------------------------------------------------------
   Function :   opt_med9()
   In       :   pointer to an array of 9 pixelvalues
   Out      :   a pixelvalue
   Job      :   optimized search of the median of 9 pixelvalues
   Notice   :   in theory, cannot go faster without assumptions on the
                signal.
                Formula from:
                XILINX XCELL magazine, vol. 23 by John L. Smith

                The input array is modified in the process
                The result array is guaranteed to contain the median
                value
                in middle position, but other elements are NOT sorted.
		---------------------------------------------------------------*/

#define PIX_SORT(a,b) { if ((a)>(b)) swap((a),(b)); }

  template <class Pel> Pel OptSelect9(Pel* p)
  {
    PIX_SORT(p[1], p[2]) ; PIX_SORT(p[4], p[5]) ; PIX_SORT(p[7], p[8]) ;
    PIX_SORT(p[0], p[1]) ; PIX_SORT(p[3], p[4]) ; PIX_SORT(p[6], p[7]) ;
    PIX_SORT(p[1], p[2]) ; PIX_SORT(p[4], p[5]) ; PIX_SORT(p[7], p[8]) ;
    PIX_SORT(p[0], p[3]) ; PIX_SORT(p[5], p[8]) ; PIX_SORT(p[4], p[7]) ;
    PIX_SORT(p[3], p[6]) ; PIX_SORT(p[1], p[4]) ; PIX_SORT(p[2], p[5]) ;
    PIX_SORT(p[4], p[7]) ; PIX_SORT(p[4], p[2]) ; PIX_SORT(p[6], p[4]) ;
    PIX_SORT(p[4], p[2]) ; return(p[4]) ;
  }

  template <class Pel> Pel OptSelect25(Pel* p)
  {
    PIX_SORT(p[0], p[1]) ;   PIX_SORT(p[3], p[4]) ;   PIX_SORT(p[2], p[4]) ;
    PIX_SORT(p[2], p[3]) ;   PIX_SORT(p[6], p[7]) ;   PIX_SORT(p[5], p[7]) ;
    PIX_SORT(p[5], p[6]) ;   PIX_SORT(p[9], p[10]) ;  PIX_SORT(p[8], p[10]) ;
    PIX_SORT(p[8], p[9]) ;   PIX_SORT(p[12], p[13]) ; PIX_SORT(p[11], p[13]) ;
    PIX_SORT(p[11], p[12]) ; PIX_SORT(p[15], p[16]) ; PIX_SORT(p[14], p[16]) ;
    PIX_SORT(p[14], p[15]) ; PIX_SORT(p[18], p[19]) ; PIX_SORT(p[17], p[19]) ;
    PIX_SORT(p[17], p[18]) ; PIX_SORT(p[21], p[22]) ; PIX_SORT(p[20], p[22]) ;
    PIX_SORT(p[20], p[21]) ; PIX_SORT(p[23], p[24]) ; PIX_SORT(p[2], p[5]) ;
    PIX_SORT(p[3], p[6]) ;   PIX_SORT(p[0], p[6]) ;   PIX_SORT(p[0], p[3]) ;
    PIX_SORT(p[4], p[7]) ;   PIX_SORT(p[1], p[7]) ;   PIX_SORT(p[1], p[4]) ;
    PIX_SORT(p[11], p[14]) ; PIX_SORT(p[8], p[14]) ;  PIX_SORT(p[8], p[11]) ;
    PIX_SORT(p[12], p[15]) ; PIX_SORT(p[9], p[15]) ;  PIX_SORT(p[9], p[12]) ;
    PIX_SORT(p[13], p[16]) ; PIX_SORT(p[10], p[16]) ; PIX_SORT(p[10], p[13]) ;
    PIX_SORT(p[20], p[23]) ; PIX_SORT(p[17], p[23]) ; PIX_SORT(p[17], p[20]) ;
    PIX_SORT(p[21], p[24]) ; PIX_SORT(p[18], p[24]) ; PIX_SORT(p[18], p[21]) ;
    PIX_SORT(p[19], p[22]) ; PIX_SORT(p[8], p[17]) ;  PIX_SORT(p[9], p[18]) ;
    PIX_SORT(p[0], p[18]) ;  PIX_SORT(p[0], p[9]) ;   PIX_SORT(p[10], p[19]) ;
    PIX_SORT(p[1], p[19]) ;  PIX_SORT(p[1], p[10]) ;  PIX_SORT(p[11], p[20]) ;
    PIX_SORT(p[2], p[20]) ;  PIX_SORT(p[2], p[11]) ;  PIX_SORT(p[12], p[21]) ;
    PIX_SORT(p[3], p[21]) ;  PIX_SORT(p[3], p[12]) ;  PIX_SORT(p[13], p[22]) ;
    PIX_SORT(p[4], p[22]) ;  PIX_SORT(p[4], p[13]) ;  PIX_SORT(p[14], p[23]) ;
    PIX_SORT(p[5], p[23]) ;  PIX_SORT(p[5], p[14]) ;  PIX_SORT(p[15], p[24]) ;
    PIX_SORT(p[6], p[24]) ;  PIX_SORT(p[6], p[15]) ;  PIX_SORT(p[7], p[16]) ;
    PIX_SORT(p[7], p[19]) ;  PIX_SORT(p[13], p[21]) ; PIX_SORT(p[15], p[23]) ;
    PIX_SORT(p[7], p[13]) ;  PIX_SORT(p[7], p[15]) ;  PIX_SORT(p[1], p[9]) ;
    PIX_SORT(p[3], p[11]) ;  PIX_SORT(p[5], p[17]) ;  PIX_SORT(p[11], p[17]) ;
    PIX_SORT(p[9], p[17]) ;  PIX_SORT(p[4], p[10]) ;  PIX_SORT(p[6], p[12]) ;
    PIX_SORT(p[7], p[14]) ;  PIX_SORT(p[4], p[6]) ;   PIX_SORT(p[4], p[7]) ;
    PIX_SORT(p[12], p[14]) ; PIX_SORT(p[10], p[14]) ; PIX_SORT(p[6], p[7]) ;
    PIX_SORT(p[10], p[12]) ; PIX_SORT(p[6], p[10]) ;  PIX_SORT(p[6], p[17]) ;
    PIX_SORT(p[12], p[17]) ; PIX_SORT(p[7], p[17]) ;  PIX_SORT(p[7], p[10]) ;
    PIX_SORT(p[12], p[18]) ; PIX_SORT(p[7], p[12]) ;  PIX_SORT(p[10], p[18]) ;
    PIX_SORT(p[12], p[20]) ; PIX_SORT(p[10], p[20]) ; PIX_SORT(p[10], p[12]) ;

    return (p[12]);
  }

  template <class Pel> void Median_Border(Bitmap<Pel>& dst,const Bitmap<Pel>& src,int border)
  {
    int w=src.AskWidth(), h=src.AskHeight();
    const Pel*const* sp = src.AskFrame();
    Pel*const* dp = dst.AskFrame();

    int npel = (2*border+1)*(2*border+1);
    Pel* tmp = new Pel[npel];

    for (int y=0;y<border;y++)
      for (int x=0;x<w;x++)
	{
	  int i=0;
	  for (int dy=-border;dy<=border;dy++)
	    for (int dx=-border;dx<=border;dx++)
	      if (x+dx>0 && x+dx<w && y+dy>0 && y+dy<h)
		tmp[i++] = sp[y+dy][x+dx];

	  dp[y][x] = QuickSelect(tmp, i);

	  int yoff=h-border;
	  i=0;
	  for (int dy=-border;dy<=border;dy++)
	    for (int dx=-border;dx<=border;dx++)
	      if (x+dx>0 && x+dx<w && y+yoff+dy>0 && y+yoff+dy<h)
		tmp[i++] = sp[y+yoff+dy][x+dx];

	  dp[y+yoff][x] = QuickSelect(tmp, i);
	}

    for (int y=border;y<h-border;y++)
      for (int x=0;x<border;x++)
	{
	  int i=0;
	  for (int dy=-border;dy<=border;dy++)
	    for (int dx=-border;dx<=border;dx++)
	      if (x+dx>0 && x+dx<w && y+dy>0 && y+dy<h)
		tmp[i++] = sp[y+dy][x+dx];

	  dp[y][x] = QuickSelect(tmp, i);

	  int xoff=w-border;
	  i=0;
	  for (int dy=-border;dy<=border;dy++)
	    for (int dx=-border;dx<=border;dx++)
	      if (x+xoff+dx>0 && x+xoff+dx<w)
		tmp[i++] = sp[y+dy][x+xoff+dx];

	  dp[y][x+xoff] = QuickSelect(tmp, i);
	}

    delete[] tmp;
  }

  template <class Pel> void Median_QuickSelect(Bitmap<Pel>& dst,const Bitmap<Pel>& src,int radius)
  {
    int w=src.AskWidth(), h=src.AskHeight();
    const Pel*const* sp = src.AskFrame();
    Pel*const* dp = dst.AskFrame();

    int npel = (2*radius+1)*(2*radius+1);
    Pel* tmp = new Pel[npel];

    for (int y=radius;y<h-radius;y++)
      for (int x=radius;x<w-radius;x++)
	{
	  int i=0;
	  for (int dy=-radius;dy<=radius;dy++)
	    for (int dx=-radius;dx<=radius;dx++)
	      tmp[i++] = sp[y+dy][x+dx];

	  dp[y][x] = QuickSelect(tmp, npel);
	}

    delete[] tmp;
  }


  template <class Pel> void Median_3x3(Bitmap<Pel>& dst,const Bitmap<Pel>& src)
  {
    int w=src.AskWidth(), h=src.AskHeight();
    const Pel*const* sp = src.AskFrame();
    Pel*const* dp = dst.AskFrame();

    Pel tmp[9];

    for (int y=1;y<h-1;y++)
      for (int x=1;x<w-1;x++)
	{
	  int i=0;
	  for (int dy=-1;dy<=1;dy++)
	    for (int dx=-1;dx<=1;dx++)
	      tmp[i++] = sp[y+dy][x+dx];

	  dp[y][x] = OptSelect9(tmp);
	}
  }


  template <class Pel> void Median_5x5(Bitmap<Pel>& dst,const Bitmap<Pel>& src)
  {
    int w=src.AskWidth(), h=src.AskHeight();
    const Pel*const* sp = src.AskFrame();
    Pel*const* dp = dst.AskFrame();

    Pel tmp[25];

    for (int y=2;y<h-2;y++)
      for (int x=2;x<w-2;x++)
	{
	  int i=0;
	  for (int dy=-2;dy<=2;dy++)
	    for (int dx=-2;dx<=2;dx++)
	      tmp[i++] = sp[y+dy][x+dx];

	  dp[y][x] = OptSelect25(tmp);
	}
  }


  template <class Pel> void Median(Bitmap<Pel>& dst,const Bitmap<Pel>& src,int radius)
  {
    int w=src.AskWidth(), h=src.AskHeight();
    dst.Create(w,h);

    Median_Border(dst,src,radius);

    switch (radius)
      {
      case 0: Copy(dst,src); break;
      case 1: Median_3x3(dst,src); break;
      case 2: Median_5x5(dst,src); break;
      default:
	Median_QuickSelect(dst,src,radius);
	break;
      }
  }


  template Pixel QuickSelect(Pixel* arr, int n);
  template int QuickSelect(int* arr, int n);
  template void Median_QuickSelect(Bitmap<Pixel>& dst,const Bitmap<Pixel>& src,int radius);
  template void Median_QuickSelect(Bitmap<int>& dst,const Bitmap<int>& src,int radius);
  template void Median_3x3(Bitmap<Pixel>& dst,const Bitmap<Pixel>& src);
  template void Median_3x3(Bitmap<int>& dst,const Bitmap<int>& src);
  template void Median_5x5(Bitmap<Pixel>& dst,const Bitmap<Pixel>& src);
  template void Median_5x5(Bitmap<int>& dst,const Bitmap<int>& src);
  template void Median(Bitmap<Pixel>& dst,const Bitmap<Pixel>& src,int radius);
  template void Median(Bitmap<int>& dst,const Bitmap<int>& src,int radius);
}

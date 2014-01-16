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

#include "libvideogfx/graphics/visualize/regions.hh"
#include <stdlib.h>

namespace videogfx {

  void DrawRegionBoundaries(Bitmap<Pixel>& bm,const Bitmap<int>& regionmap, bool draw_border)
  {
    const int*const* sp = regionmap.AskFrame();

    int w = bm.AskWidth();
    int h = bm.AskHeight();

    bm.Create(w,h);

    Pixel*const* dp = bm.AskFrame();
    for (int y=0;y<h-1;y++)
      for (int x=0;x<w-1;x++)
	{
	  if      (sp[y][x] != sp[y+1][x]) dp[y][x]=BoolPixel_Set;
	  else if (sp[y][x] != sp[y][x+1]) dp[y][x]=BoolPixel_Set; else dp[y][x]=BoolPixel_Clear;
	}

    int x,y;
    x=w-1;
    for (y=0;y<h-1;y++)
      {
	if (sp[y][x] != sp[y+1][x]) dp[y][x]=BoolPixel_Set; else dp[y][x]=BoolPixel_Clear;
      }
  
    y=h-1;
    for (int x=0;x<w-1;x++)
      {
	if (sp[y][x] != sp[y][x+1]) dp[y][x]=BoolPixel_Set; else dp[y][x]=BoolPixel_Clear;
      }


    // draw image border

    if (draw_border)
      {
	for (int x=0;x<w;x++)
	  dp[0][x]=dp[h-1][x]=BoolPixel_Set;
	for (int y=0;y<h;y++)
	  dp[y][0]=dp[y][w-1]=BoolPixel_Set;
      }
  }


  void OverlayRegionBoundaries(Bitmap<Pixel>& bm,const Bitmap<int>& regionmap, Pixel value)
  {
    const int*const* sp = regionmap.AskFrame();

    int w = bm.AskWidth();
    int h = bm.AskHeight();

    bm.Create(w,h);

    Pixel*const* dp = bm.AskFrame();
    for (int y=0;y<h-1;y++)
      for (int x=0;x<w-1;x++)
	{
	  if      (sp[y][x] != sp[y+1][x]) dp[y][x]=value;
	  else if (sp[y][x] != sp[y][x+1]) dp[y][x]=value;
	}

    int x,y;
    x=w-1;
    for (y=0;y<h-1;y++)
      {
	if (sp[y][x] != sp[y+1][x]) dp[y][x]=value;
      }
  
    y=h-1;
    for (int x=0;x<w-1;x++)
      {
	if (sp[y][x] != sp[y][x+1]) dp[y][x]=value;
      }
  }


  void OverlayAlphaMask(Bitmap<Pixel>& bm,const Bitmap<Pixel>& alphamap,  Pixel val, int threshold)
  {
    Pixel*const* p = bm.AskFrame();
    const Pixel*const* ap = alphamap.AskFrame();

    assert(bm.AskWidth() ==alphamap.AskWidth());
    assert(bm.AskHeight()==alphamap.AskHeight());

    for (int y=0;y<bm.AskHeight();y++)
      for (int x=0;x<bm.AskWidth();x++)
	{
	  if (ap[y][x]<threshold)
	    {
	      if (((x+y)%4)==0)
		{ p[y][x]=val; }
	      else if (x>0 && ap[y][x] != ap[y][x-1])
		{
		  if (ap[y][x]<ap[y][x-1]) p[y][x]=val; else p[y][x-1]=val;
		}
	      else if (y>0 && ap[y][x] != ap[y-1][x])
		{
		  if (ap[y][x]<ap[y-1][x]) p[y][x]=val; else p[y-1][x]=val;
		}
	    }
	}
  }

}

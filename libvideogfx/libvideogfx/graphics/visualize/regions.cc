
#include <stdlib.h>
#include "libvideogfx/graphics/visualize/regions.hh"


void DrawRegionBoundaries(Bitmap<Pixel>& bm,const Bitmap<int>& regionmap)
{
  const int*const* sp = regionmap.AskFrame();

  int w = bm.AskWidth();
  int h = bm.AskHeight();

  bm.Create(w,h);

  Pixel*const* dp = bm.AskFrame();
  for (int y=0;y<h-1;y++)
    for (int x=0;x<w-1;x++)
      {
	if      (sp[y][x] != sp[y+1][x]) dp[y][x]=0;
	else if (sp[y][x] != sp[y][x+1]) dp[y][x]=0; else dp[y][x]=255;
      }

  int x,y;
  x=w-1;
  for (y=0;y<h-1;y++)
    {
      if (sp[y][x] != sp[y+1][x]) dp[y][x]=0; else dp[y][x]=255;
    }
  
  y=h-1;
  for (int x=0;x<w-1;x++)
    {
      if (sp[y][x] != sp[y][x+1]) dp[y][x]=0; else dp[y][x]=255;
    }


  // draw image border

  for (int x=0;x<w;x++)
    dp[0][x]=dp[h-1][x]=0;
  for (int y=0;y<h;y++)
    dp[y][0]=dp[y][w-1]=0;
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

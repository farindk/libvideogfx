
#include "libvideogfx/graphics/draw/format.hh"

void PixelDifferenceToPixel(Bitmap<Pixel>& dst,const Bitmap<int16>& src)
{
  int w=src.AskWidth();
  int h=src.AskHeight();

  dst.Create(w,h);

  const int16*const* ap = src.AskFrame();
  Pixel*const* bp = dst.AskFrame();

  for (int y=0;y<h;y++)
    for (int x=0;x<w;x++)
      bp[y][x] = (Pixel)(ap[y][x]/2+128);
}



#include "../libvideogfx/graphics/color/colorspace.hh"

#include <iostream>
#include <iomanip>
#include <algorithm>
using namespace std;
using namespace videogfx;


void Show(const Bitmap<Pixel>& pm)
{
  const Pixel*const* p = pm.AskFrame();

  for (int y=0;y<pm.AskHeight();y++,cout << endl)
    for (int x=0;x<pm.AskWidth();x++)
      cout << setw(2) << ((int)p[y][x]) << " ";
}


int main(int argc,char** argv)
{
  try
    {
      ImageParam param;
      param.width  = 5;
      param.height = 5;
      param.chroma = Chroma_444;
      param.colorspace = Colorspace_YUV;

      Image<Pixel> src,dst;
      src.Create(param);

      Pixel*const* p = src.AskFrameU();
      for (int y=0;y<param.AskChromaHeight();y++)
	for (int x=0;x<param.AskChromaWidth();x++)
	  {
	    p[y][x] = 2*x+10*y;
	  }

      Show(src.AskBitmapU()); cout << endl;

      ChangeChroma(dst,src, Chroma_422); Show(dst.AskBitmapU()); cout << endl;
      ChangeChroma(dst,src, Chroma_420); Show(dst.AskBitmapU()); cout << endl;
    }
  catch(Excpt_Base& e)
    {
      MessageDisplay::Show(e);
    }

  return 0;
}


#include "libvideogfx/types.hh"
#include "libvideogfx/graphics/datatypes/bitmap.hh"
#include "libvideogfx/graphics/filters/linear.hh"
#include "libvideogfx/graphics/draw/blit.hh"

#include <unistd.h>
#include <iostream>
#include <iomanip>
using namespace std;


void Show(Bitmap<Pixel> pm)
{
  const Pixel*const* p = pm.AskFrame();

  for (int y=0;y<pm.AskHeight();y++,cout << endl)
    for (int x=0;x<pm.AskWidth();x++)
      cout << setw(2) << ((int)p[y][x]) << " ";
}


main(int argc,char** argv)
{
  try
    {
      Bitmap<Pixel> pm(new BitmapProvider_Mem<Pixel>(20,11, 0));

      {
	Pixel*const* p = pm.AskFrame();

	for (int y=0;y<pm.AskHeight();y++)
	  for (int x=0;x<pm.AskWidth();x++)
	    p[y][x] = x+2*y;
      }

#if 1
      Bitmap<Pixel> sub = pm.CreateSubView(3,5,6,4);
      Bitmap<Pixel> top = pm.CreateFieldView(true);
      Bitmap<Pixel> bot = pm.CreateFieldView(false);
#endif

      Array<double> filter;
      filter.Create(3,-1);
      filter[-1] = 1;
      filter[ 0] = 2;
      filter[ 1] = 1;
      //CreateGaussFilter(filter,2.0);

      Bitmap<Pixel> filteredbm;

      Show(pm);
      Show(sub);

      ConvolveV(filteredbm, sub,filter);

      Copy(sub,filteredbm);

      Show(pm);
    }
  catch (Excpt_Base& e)
    {
      MessageDisplay::Show(e);
    }
}

// ./median filename filterradius

#include "../libvideogfx/x11/imgwin.hh"
#include "../libvideogfx/graphics/fileio/unified_reader.hh"
#include "../libvideogfx/graphics/filters/median.hh"

#include <unistd.h>
#include <iostream>
using namespace videogfx;
using namespace std;

int main(int argc,char** argv)
{
  try
    {
      ImageWindow_Autorefresh_X11 win;
      Image<Pixel> img;

      UnifiedImageLoader reader;
      reader.SetInput(argv[1]);
      reader.SetTargetColorspace(Colorspace_Greyscale);
      reader.ReadImage(img);

      win.Create(img.AskWidth(),img.AskHeight(),"test");
      win.Display(img);

      Image<Pixel> filtered;
      filtered = img.Clone();

      while (!reader.IsEOF())
	{
	  reader.ReadImage(img);
	  Median(filtered.AskBitmapY(), img.AskBitmapY(), atoi(argv[2]));
	  win.Display(filtered);
	}
    }
  catch (const Excpt_Base& e)
    {
      MessageDisplay::Show(e);
    }
  return 0;
}

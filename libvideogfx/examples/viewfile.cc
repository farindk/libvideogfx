// use option -xv to open image with Xv extension

#include "libvideogfx/x11/imgwin.hh"
#include "libvideogfx/graphics/fileio/unified_loader.hh"

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

      reader.ReadImage(img);

      win.Create(img.AskWidth(),img.AskHeight(),"test");
      win.Display(img);

      int idx=1;
      while (!reader.IsEOF())
	{
	  //sleep(1);
	  reader.SkipToImage(idx);
	  reader.ReadImage(img);
	  win.Display(img);

	  usleep(1000000/25);
	  cout << "idx:" << idx << endl;
	  idx++;
	}

      sleep(5);
    }
  catch (const Excpt_Base& e)
    {
      MessageDisplay::Show(e);
    }
  return 0;
}

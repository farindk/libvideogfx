
#include "libvideogfx/graphics/fileio/v4l.hh"
#include "libvideogfx/graphics/draw/pixelops.hh"
#include "libvideogfx/graphics/draw/format.hh"
#include "libvideogfx/graphics/filters/linear.hh"
#include "libvideogfx/x11/imgwin.hh"

#include <stdio.h>

#include <fstream>
#include <algorithm>
using namespace std;
using namespace videogfx;


int main(int argc,char** argv)
{
  try
    {
      V4L_Grabber grabber;
      grabber.SetDevice("/dev/video0");
      grabber.SetGreyscaleMode();

      //grabber.DoChromaAvgOn422To420();
      // void SetResolution(int w,int h);

      grabber.StartGrabbing();

      Image<Pixel> img;
      grabber.Grab(img);

      ImageWindow_Autorefresh_X11 win;
      win.Create(img.AskWidth(),img.AskHeight(),"edges");

      Bitmap<int16> edges;

      for (;;)
	{
	  grabber.Grab(img);

	  Sobel_Hor(edges,img.AskBitmapY());
	  PixelDifferenceToPixel(img.AskBitmapY(),edges);

	  win.Display(img);
	}
    }
  catch(Excpt_Base& e)
    {
      MessageDisplay::Show(e);
    }

  return 0;
}

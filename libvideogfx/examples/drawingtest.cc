
#include "libvideogfx/x11/imgwin.hh"
#include "libvideogfx/graphics/draw/draw.hh"
#include "libvideogfx/graphics/draw/pixelops.hh"
using namespace videogfx;

#include <unistd.h>
#include <time.h>
#include <stdlib.h>


main(int argc,char** argv)
{
  srand(time(NULL));

  ImageParam spec;
  spec.width   = 512;
  spec.height  = 256;
  spec.colorspace = Colorspace_Greyscale;

  Image<Pixel> img;
  img.Create(spec);

  Clear(img.AskBitmapY(), (Pixel)255);

  ImageWindow_Autorefresh_X11 win;
  win.Create(spec.width,spec.height,"test");

  for (int i=0;i<10;i++)
    {
      for (int i=0;i<1000;i++)
	{
	  int x0 = rand()%spec.width;
	  int y0 = rand()%spec.height;
	  int x1 = rand()%spec.width;
	  int y1 = rand()%spec.height;

	  DrawLine(img.AskBitmapY(), x0,y0,x1,y1, (Pixel)0);
	}

      win.Display(img);
      usleep(10000);  // slow down to prevent X11 overload
    }

  sleep(3);



  Clear(img.AskBitmapY(), (Pixel)255);

  for (int i=0;i<10;i++)
    {
      for (int i=0;i<100;i++)
	{
	  int x0 = rand()%(3*spec.width )-spec.width;
	  int y0 = rand()%(3*spec.height)-spec.height;
	  int x1 = rand()%(3*spec.width )-spec.width;
	  int y1 = rand()%(3*spec.height)-spec.height;

	  if (ClipLine(x0,y0,x1,y1, 20,20,spec.width-20,spec.height-20))
	    DrawLine(img.AskBitmapY(), x0,y0,x1,y1, (Pixel)0);
	}

      win.Display(img);
      usleep(10000);  // slow down to prevent X11 overload
    }

  sleep(1);
  Inverse(img.AskBitmapY(),(Pixel)255); win.Display(img);
  sleep(1);
  Inverse(img.AskBitmapY(),(Pixel)255); win.Display(img);
  sleep(1);


  Clear(img.AskBitmapY(), (Pixel)255);

  for (int i=0;i<10;i++)
    {
      for (int i=0;i<10;i++)
	{
	  int x0 = rand()%(spec.width +40)-20;
	  int y0 = rand()%(spec.height+40)-20;
	  int x1 = rand()%(spec.width +40)-20;
	  int y1 = rand()%(spec.height+40)-20;

	  DrawArrow(img.AskBitmapY(), x0,y0,x1,y1, (Pixel)0);
	}

      win.Display(img);
      usleep(10000);  // slow down to prevent X11 overload
    }

  sleep(3);


  for (int i=0;i<100;i++)
    {
      Clear(img.AskBitmapY(), (Pixel)255);

      int x0 = rand()%(2*spec.width )-spec.width/2;
      int y0 = rand()%(2*spec.height)-spec.height/2;
      int r  = rand()%(spec.width);

      DrawCircle(img.AskBitmapY(), x0,y0,r, (Pixel)0);

      win.Display(img);
      usleep(250000);

      DrawCircle(img.AskBitmapY(), x0,y0,r, (Pixel)0, true);

      win.Display(img);
      usleep(250000);
    }

  sleep(3);
}

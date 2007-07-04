
#include <iostream>
#include "../libvideogfx/x11/imgwin.hh"
#include "../libvideogfx/graphics/draw/draw.hh"

using namespace std;
using namespace videogfx;

int main(int argc,char** argv)
{
  try
    {
      Image<Pixel> img;
      X11Win win;

      img.Create(200,200,Colorspace_RGB);
      win.Create(200,200,"qwe");
      Clear(img, Color<Pixel>(0,0,0));

      bool on=false;
      for (;;)
	{
	  int x,y;
	  int b = win.CheckForMouseButton(x,y);
	  if (b>0)
	    {
	      DrawCircle(img,x,y,2, Color<Pixel>(0,255,0),true);
	      win.Display(img);
	      on=true;
	    }

	  if (b<0)
	    {
	      DrawCircle(img,x,y,2, Color<Pixel>(255,0,0),true);
	      win.Display(img);
	      on=false;
	    }

	  if (win.CheckForMouseMove(x,y))
	    if (on)
	      {
		DrawCircle(img,x,y,2, Color<Pixel>(0,0,255),true);
		win.Display(img);
	      }

	  if (win.CheckForKeypress()=='q')
	    break;
	}
    }
  catch(const Excpt_Base& e)
    {
      MessageDisplay::Show(e);
    }

  return 0;
}

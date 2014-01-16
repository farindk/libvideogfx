
#include "../libvideogfx/x11/imgwin.hh"
#include "../libvideogfx/graphics/fileio/jpeg.hh"
#include "../libvideogfx/graphics/draw/blit.hh"
#include "../libvideogfx/graphics/draw/format.hh"
#include "../libvideogfx/graphics/draw/scale.hh"
using namespace videogfx;

#include <unistd.h>
#include <time.h>
#include <stdlib.h>


// ARGS: give me 4 JPEG files

int main(int argc,char** argv)
{
  srand(time(NULL));

  Image<Pixel> img[5];
  for (int i=0;i<4;i++)
    {
      ReadImage_JPEG(img[i], argv[i+1]);
    }

  ImageParam spec = img[0].AskParam();
  img[4].Create(spec);

  ImageWindow_Autorefresh_X11 win;
  win.Create(spec.width,spec.height,"test");

  int w = spec.width;
  int h = spec.height;

#if 0
  Copy(img[4],0,0,
       img[0],0,0,w/2,h/2);
  Copy(img[4],w/2,0,
       img[1],w/2,0,w/2,h/2);
  Copy(img[4],0,h/2,
       img[2],0,h/2,w/2,h/2);
  Copy(img[4],w/2,h/2,
       img[3],w/2,h/2,w/2,h/2);
#else
  CopyScaled(img[4],0,0,w/2,h/2,
	     img[0],0,0,w,h);
  CopyScaled(img[4],w/2,0,w/2,h/2,
	     img[1],0,0,w,h);
  CopyScaled(img[4],0,h/2,w/2,h/2,
	     img[2],0,0,w,h);
  CopyScaled(img[4],w/2,h/2,w/2,h/2,
	     img[3],0,0,w,h);
#endif

  for (int i=0;i<5;i++)
    {
      win.Display(img[i]);
      sleep(1);
    }

  sleep(10);

  return 0;
}

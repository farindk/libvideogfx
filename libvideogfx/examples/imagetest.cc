// use option -xv to open image with Xv extension

#include "libvideogfx/x11/imgwin.hh"
#include "libvideogfx/libvideogfx.hh"
#include <unistd.h>
using namespace videogfx;

#include <iostream>
using namespace std;


void FillStep(Bitmap<Pixel>& bm, Pixel a,Pixel b, Pixel c,Pixel d)
{
  Pixel*const* p = bm.AskFrame();
  int x0 = bm.AskXOffset();
  int y0 = bm.AskYOffset();
  cout << x0 << " " << y0 << " (" << bm.AskWidth() << " " << bm.AskHeight() << ")" << endl;

  for (int y=0;y<bm.AskHeight();y++)
    for (int x=0;x<bm.AskWidth();x++)
      {
	int ox = x-x0;
	int oy = y-y0;

	if (ox<0)
	  {
	    if (oy<0) p[oy][ox] = a;
	    else      p[oy][ox] = c;
	  }
	else
	  {
	    if (oy<0) p[oy][ox] = b;
	    else      p[oy][ox] = d;
	  }
      }
}


int main(int argc,char** argv)
{
  {
    ImageWindow_Autorefresh_X11 win_yuv;

    ImageParam spec;
    spec.width   = 256;
    spec.height  = 256;
    spec.chroma  = Chroma_420;
    spec.xoffset = 128;
    spec.yoffset = 128;

    Image<Pixel> img_yuv;  spec.colorspace = Colorspace_YUV;  img_yuv.Create(spec);

    FillStep(img_yuv.AskBitmapY(), 80, 100, 140, 200);
    FillStep(img_yuv.AskBitmapU(), 80, 150,128,128);
    FillStep(img_yuv.AskBitmapV(), 140,128,40,150);

    win_yuv.SetPosition(0,0); win_yuv.Create(256,256,"yuv");

    Image<Pixel> disp = img_yuv;
    disp.MoveZero(0,0);
    win_yuv.Display(disp);

    sleep(3);
  }


  Image<Pixel> img;
  ReadImage_JPEG(img, argv[1]);
  {
    ImageWindow_Autorefresh_X11 win;
    win.Create(img.AskWidth(), img.AskHeight(), "output");
    win.Display(img);

    sleep(3);
  }

  Image<Pixel> larger;
  Enlarge(larger, img, 10,20,30,40, Color<Pixel>(200,128,128));
  larger.MoveZero(0,0);
  img = larger;
  {
    ImageWindow_Autorefresh_X11 win;
    win.Create(img.AskWidth(), img.AskHeight(), "output");
    win.Display(img);

    sleep(3);
  }

  return 0;
}

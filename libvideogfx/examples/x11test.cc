// use option -xv to open image with Xv extension

#include "libvideogfx/x11/imgwin.hh"
#include "libvideogfx/x11/draw_x11.hh"
using namespace videogfx;

#include <unistd.h>


main(int argc,char** argv)
{
  bool useXv = (argc>1 && strcmp(argv[1],"-xv")==0);

  ImageWindow_Autorefresh_X11 win(useXv);

  ImageParam spec;
  spec.width   = 256;
  spec.height  = 256;
  spec.chroma  = Chroma_420;
  spec.colorspace = Colorspace_YUV;

  Image<Pixel> img;
  img.Create(spec);

  Pixel*const* yp = img.AskFrameY();
  Pixel*const* up = img.AskFrameU();
  Pixel*const* vp = img.AskFrameV();

  for (int y=0;y<128;y++)
    for (int x=0;x<128;x++)
      {
	yp[y*2][x*2]=yp[y*2][x*2+1]=yp[y*2+1][x*2]=yp[y*2+1][x*2+1]=128;
	up[y][x] = x*2;
	vp[y][x] = y*2;
      }

  WriteText_X11(img.AskBitmapY(),"Test",100,110,
		(Pixel)255,(Pixel)0,"-adobe-helvetica-bold-r-*-*-20-*-*-*-*-*-*-*");

  win.Create(256,256,"test");
  win.Display(img);

  sleep(10);
}

// use option -xv to open image with Xv extension

#include "libvideogfx/x11/imgwin.cc"
#include "libvideogfx/graphics/color/colorspace.hh"
#include "libvideogfx/graphics/draw/draw.hh"
#include <unistd.h>
using namespace videogfx;


void FillGradient(Bitmap<Pixel>& bmx,Bitmap<Pixel>& bmy)
{
  Pixel*const* px = bmx.AskFrame();
  Pixel*const* py = bmy.AskFrame();

  for (int y=0;y<256;y++)
    for (int x=0;x<256;x++)
      {
	px[y][x] = x;
	py[y][x] = y;
      }
}


main(int argc,char** argv)
{
  ImageWindow_Autorefresh_X11 win_rgb;
  ImageWindow_Autorefresh_X11 win_yuv;
  ImageWindow_Autorefresh_X11 win_hsv;

  ImageParam spec;
  spec.width   = 256;
  spec.height  = 256;
  spec.chroma  = Chroma_444;

  Image<Pixel> img_rgb;  spec.colorspace = Colorspace_RGB;  img_rgb.Create(spec);
  Image<Pixel> img_yuv;  spec.colorspace = Colorspace_YUV;  img_yuv.Create(spec);
  Image<Pixel> img_hsv;  spec.colorspace = Colorspace_HSV;  img_hsv.Create(spec);

  Clear(img_rgb.AskBitmapB(),(Pixel)128);
  FillGradient(img_rgb.AskBitmapR(), img_rgb.AskBitmapG());

  Clear(img_yuv.AskBitmapY(),(Pixel)128);
  FillGradient(img_yuv.AskBitmapU(), img_yuv.AskBitmapV());

  Clear(img_hsv.AskBitmap(Bitmap_Value),(Pixel)255);
  FillGradient(img_hsv.AskBitmap(Bitmap_Hue), img_hsv.AskBitmap(Bitmap_Saturation));

  Image<Pixel> dispimg;

  ChangeColorspace(dispimg,img_rgb,Colorspace_RGB);
  win_rgb.SetPosition( 10,10); win_rgb.Create(256,256,"rgb"); win_rgb.Display(dispimg);

  ChangeColorspace(dispimg,img_yuv,Colorspace_RGB);
  win_yuv.SetPosition(310,10); win_yuv.Create(256,256,"yuv"); win_yuv.Display(dispimg);

  ChangeColorspace(dispimg,img_hsv,Colorspace_RGB);
  win_hsv.SetPosition(610,10); win_hsv.Create(256,256,"hsv"); win_hsv.Display(dispimg);

  sleep(10);
}

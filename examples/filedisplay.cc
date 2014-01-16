// use option -xv to open image with Xv extension

#include "../libvideogfx/x11/imgwin.hh"
#include "../libvideogfx/graphics/fileio/yuv4mpeg.hh"

#include <unistd.h>
#include <iostream>
using namespace videogfx;
using namespace std;

int main(int argc,char** argv)
{
  ImageWindow_Autorefresh_X11 win;
  Image<Pixel> img;
  FileReader_YUV4MPEG reader;

  ifstream istr(argv[1]);
  reader.SetYUVStream(istr);

  ImageParam spec = reader.AskParam();

  win.Create(spec.width,spec.height,"test");

  while (!reader.IsEOF())
    {
      reader.ReadImage(img);
      win.Display(img);
    }

  sleep(5);

  return 0;
}

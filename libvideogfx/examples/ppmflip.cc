
#include "libvideogfx/graphics/fileio/ppm.hh"

#include <fstream>
#include <algorithm>
using namespace std;
using namespace videogfx;


void main(int argc,char** argv)
{
  ifstream istr(argv[1]);
  ofstream ostr(argv[2]);

  Image<Pixel> img;
  ReadImage_PPM(img,istr);

  int w = img.AskWidth();

  Pixel*const* p = img.AskFrameY();
  for (int y=0;y<img.AskHeight();y++)
    for (int x=0;x<w/2;x++)
      {
	swap(p[y][x],p[y][w-1-x]);
      }

  WriteImage_PPM(img,ostr);
}


#include "../libvideogfx/graphics/fileio/rw_ppm.hh"
#include "../libvideogfx/graphics/fileio/avi/aviformat.hh"

#include <fstream>
using namespace std;
using namespace videogfx;

#include <stdio.h>

main(int argc,char** argv)
{
  AVIWriter writer;
  ofstream ostr("movie.avi", std::ios::out | std::ios::binary);
  writer.SetOutputstream(ostr);

  for (int i=0;i<40;i++)
    {
      char buf[100];
      sprintf(buf,argv[1],i+1);

      ifstream istr;
      istr.open(buf, std::ios::in | std::ios::binary);
      Image_RGB<Pixel> img = ReadImage_PPM(istr);

      writer.WriteImage(img);
    }

  writer.Flush();
}

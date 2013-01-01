
#include "libvideogfx/x11/imgwin.hh"
#include "libvideogfx/graphics/fileio/unified_reader.hh"
#include "libvideogfx/graphics/fileio/unified_writer.hh"

#include <unistd.h>
#include <iostream>
using namespace videogfx;
using namespace std;

int main(int argc,char** argv)
{
  Image<Pixel> img;

  UnifiedImageReader reader;
  reader.SetInput(argv[1]);

  UnifiedImageWriter writer;
  writer.SetOutput(argv[2]);

  int idx=1;
  while (!reader.IsEOF())
    {
      reader.ReadImage(img);
      writer.WriteImage(img);

      cout << "frame:" << idx << endl;
      idx++;
    }

  return 0;
}

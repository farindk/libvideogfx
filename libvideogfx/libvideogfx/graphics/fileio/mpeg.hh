/*********************************************************************
  mpeg.hh

  purpose:
    MPEG-file reader. The external program 'dvdview' is used to
    decode the bit-stream.

  notes:

  to do:

  author(s):
   - Dirk Farin, farin@ti.uni-mannheim.de
     University Mannheim, Dept. Cumputer Science IV
     L 15,16, room 410 / D-68161 Mannheim / Germany

  modifications:
   27/Apr/01 - Dirk Farin - first implementation
 *********************************************************************/

#ifndef LIBVIDEOGFX_GRAPHICS_FILEIO_READ_MPEG_HH
#define LIBVIDEOGFX_GRAPHICS_FILEIO_READ_MPEG_HH

#include <stdio.h>
#include <libvideogfx/graphics/basic/image.hh>


class FileReader_MPEG
{
public:
   FileReader_MPEG();
  ~FileReader_MPEG();

  // initialization

  void Open(const char* filename);

  // usage

  bool IsEOF() const;
  void SkipToImage(int nr);
  bool ReadImage(Image<Pixel>&);

private:
  FILE* d_fh;

  int  d_next_framenr;  // number of next frame to be returned by ReadImage()

  mutable bool d_image_cache_full;
  mutable Image<Pixel> d_next_image_cache;

  bool Preload(Image<Pixel>& dest) const;
};

#endif

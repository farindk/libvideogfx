/*********************************************************************
  jpeg.hh

  purpose:
    JPEG file access routines.

  notes:

  to do:

  author(s):
   - Dirk Farin, farin@informatik.uni-mannheim.de
     University Mannheim, Dept. Computer Science IV
     L 15,16  room 410 / D-68131 Mannheim / Germany

  modifications:
   18/May/2001 - Dirk Farin - first implementation
 *********************************************************************/

#ifndef LIBVIDEOGFX_GRAPHICS_FILEIO_JPEG_HH
#define LIBVIDEOGFX_GRAPHICS_FILEIO_JPEG_HH

#include <libvideogfx/graphics/datatypes/image.hh>

bool JPEG_Supported();

void ReadImage_JPEG(Image<Pixel>&,const char* filename);
void WriteImage_JPEG(const Image<Pixel>&,const char* filename);


#endif

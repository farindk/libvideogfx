/*********************************************************************
  png.hh

  purpose:
    PNG file access routines.

  notes:

  to do:
   - handling of 16 bit data
   - handling of YUV files

  author(s):
   - Gerald Kuehne kuehne@informatik.uni-mannheim.de

  modifications:
   03/May/2002 - Dirk Farin - added alpha channel support
   02/May/2002 - Gerald Kuehne - first implementation
 ********************************************************************************
    LibVideoGfx - video processing library
    Copyright (C) 2002  Dirk Farin

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ********************************************************************************/

#ifndef LIBVIDEOGFX_GRAPHICS_FILEIO_PNG_HH
#define LIBVIDEOGFX_GRAPHICS_FILEIO_PNG_HH

#include <libvideogfx/graphics/datatypes/image.hh>
#include <iostream>

namespace videogfx {

  bool PNG_Supported();

  void ReadImage_PNG(Image<Pixel>& img, std::istream& stream);
  void WriteImage_PNG(std::ostream& stream,const Image<Pixel>& img);
  bool ReadImage_PNG(Image<Pixel>& img, const char* filename);
  void WriteImage_PNG(const char* filename,const Image<Pixel>& img);

  // obsolete
  inline void WriteImage_PNG(const Image<Pixel>& img, std::ostream& stream)
  {
    std::cerr << "you're using the old syntax of libvideogfx::WriteImage_PNG()\n";
    WriteImage_PNG(stream,img);
  }
  inline void WriteImage_PNG(const Image<Pixel>& img, const char* filename)
  {
    std::cerr << "you're using the old syntax of libvideogfx::WriteImage_PNG()\n";
    WriteImage_PNG(filename,img);
  }
}

#endif

/*********************************************************************
  fileio/raw.hh

  purpose:
    read and write raw images

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
   11/Aug/2007 - Dirk Farin - first implementation
 ********************************************************************************
    LibVideoGfx - video processing library
    Copyright (C) 2007  Dirk Farin

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


#ifndef LIBVIDEOGFX_GRAPHICS_FILEIO_RAW_HH
#define LIBVIDEOGFX_GRAPHICS_FILEIO_RAW_HH

#include <fstream>
#include <iostream>

#include <libvideogfx/graphics/datatypes/image.hh>

namespace videogfx {

  class RawImageReader
  {
  public:
    RawImageReader();

    int  SetSize(int w,int h) { d_w=w; d_h=h; }
    int  SetBitsPerPixels(int bpp) { d_bpp=bpp; }
    int  SetHeaderSkip(int nbytes) { d_header_bytes=nbytes; }
    bool SetLittleEndian(bool flag=true) { d_little_endian=flag; }

    void ReadImage(Image<uint16>&, std::ifstream&);
    void ReadImage(Image<uint16>&, const char* name);

    void ReadImage(Image<Pixel>&, std::ifstream&);
    void ReadImage(Image<Pixel>&, const char* name);

    void ReadMedicalXRay(Image<Pixel>&,  const char* name);
    void ReadMedicalXRay(Image<uint16>&, const char* name);

  private:
    int  d_w,d_h;
    int  d_bpp;
    int  d_header_bytes;
    bool d_little_endian;

    void ReadImageData(Image<uint16>& img, std::ifstream& istr);
    void Convert16to8 (Image<Pixel>&, Image<uint16>&);
  };
}

#endif

/*********************************************************************
  yuv_vqeg.hh

  purpose:
    Read YUV-files in the format of the "Video Quality
    Expert Group" (www.vqeg.org).

  notes:

  to do:
    writer

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
   12/Nov/2001 - Dirk Farin - first implementation
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

#ifndef LIBVIDEOGFX_GRAPHICS_FILEIO_YUV_VQEG_HH
#define LIBVIDEOGFX_GRAPHICS_FILEIO_YUV_VQEG_HH

#include <fstream>
#include <iostream>

#include <libvideogfx/graphics/datatypes/image.hh>

namespace videogfx {

  class FileReader_YUV_VQEG
  {
  public:
    FileReader_YUV_VQEG();
    ~FileReader_YUV_VQEG() { }

    // initialization

    void SetYUVStream(std::istream& yuvstream)   { d_yuvstr = &yuvstream; }
    void SelectResolution625(bool flag) { d_mode625=flag; }

    // usage

    int  AskNFrames() const { return d_mode625 ? 220 : 260; }
    bool IsEOF() const { return d_nextframe>=AskNFrames(); }

    void SkipToImage(int nr);
    void ReadImage(Image<Pixel>&);

  private:
    std::istream* d_yuvstr;

    bool  d_mode625;
    int   d_nextframe;
  };

}

#endif

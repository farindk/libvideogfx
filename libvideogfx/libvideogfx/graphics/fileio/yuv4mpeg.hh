/********************************************************************************
  libvideogfx/graphics/fileio/yuv4mpeg.hh

  purpose:
    Read in YUV4MPEG2-files.

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
   27/May/2002 - Dirk Farin - first implementation, based on MJPEG-tools code
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

#ifndef LIBVIDEOGFX_GRAPHICS_FILEIO_YUV4MPEG_HH
#define LIBVIDEOGFX_GRAPHICS_FILEIO_YUV4MPEG_HH

#include <fstream>
#include <iostream>

#include <libvideogfx/graphics/datatypes/image.hh>

namespace videogfx {

  class FileReader_YUV4MPEG
  {
  public:
    FileReader_YUV4MPEG();
    ~FileReader_YUV4MPEG() { }

    // initialization

    void SetYUVStream  (std::istream& yuvstream)   { d_yuvstr = &yuvstream; d_initialized=false; }

    // usage

    bool IsEOF() const;

    void SkipToImage(int nr);
    ImageParam AskParam() { Init(); return d_spec; }

    bool ReadImage(Image<Pixel>&);  // returns false if no more images in the file/stream

  private:
    std::istream* d_yuvstr;
    int64 d_length;

    ImageParam d_spec;

    int    d_nextFrame;

    void Init();
    bool d_initialized;
  };


  class FileWriter_YUV4MPEG
  {
  public:
    FileWriter_YUV4MPEG();
    ~FileWriter_YUV4MPEG() { }

    void SetYUVStream(std::ostream& str)   { d_yuvstr   = &str; d_header_written=false; }

    void WriteImage(const Image<Pixel>&);

  private:
    std::ostream* d_yuvstr;
    bool d_header_written;
  };
}

#endif

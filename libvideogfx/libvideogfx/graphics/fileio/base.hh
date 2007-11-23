/*********************************************************************
  libvideogfx/graphics/fileio/base.hh

  purpose:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
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

#ifndef LIBVIDEOGFX_GRAPHICS_FILEIO_BASE_HH
#define LIBVIDEOGFX_GRAPHICS_FILEIO_BASE_HH

#include <fstream>
#include <iostream>

#include <libvideogfx/graphics/datatypes/image.hh>

namespace videogfx {


  class ImageReader
  {
  public:
    virtual ~ImageReader() { }

    virtual ImageParam AskImageParam() const = 0;
    virtual int  AskNFrames() const { return INT_MAX; } // endless stream
    virtual bool IsEOF() const { return false; } // endless stream

    virtual void ReadImage(Image<Pixel>&) = 0;
    virtual bool SkipToImage(int nr)
    { AssertDescr(false, "frame-skipping not implemented"); return false; }

  };



  class ImageWriter
  {
  public:
    virtual ~ImageWriter() { }

    virtual void WriteImage(const Image<Pixel>&) = 0;
  };

}

#endif

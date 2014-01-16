/*********************************************************************
  fileio/imagesink.hh

  purpose:
    Abstract image sink. Just throw in any kind of images and they
    will be displayed/stored/...

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
   05/Apr/2002 - Dirk Farin - first implementation
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

#ifndef LIBVIDEOGFX_GRAPHICS_FILEIO_IMAGESINK_HH
#define LIBVIDEOGFX_GRAPHICS_FILEIO_IMGAESINK_HH

#include <libvideogfx/graphics/datatypes/image.hh>

namespace videogfx {

  class ImageSink
  {
  public:
    virtual ~ImageSink() { }

    virtual void SendImage(const Image<Pixel>&) = 0;
  };



  enum ImageFileFormat
  {
    Format_JPEG, Format_PPM, Format_PGM, Format_PNG // , Format_PlanarYUV420
  };

  class ImageSink_Save : public ImageSink
  {
  public:
    ImageSink_Save(ImageFileFormat fmt = Format_JPEG);
    ~ImageSink_Save();

    bool IsFormatSupported(ImageFileFormat);

    void SetFileFormat(ImageFileFormat fmt) { d_format = fmt; }
    void SetFilename(const char*,bool autosuffix=false);
    void SetCurrentImageNr(int n) { d_curr_nr = n; }

    void SendImage(const Image<Pixel>&);

  private:
    ImageFileFormat d_format;
    char* d_filename_template;
    bool  d_autosuffix;
    int   d_curr_nr;
  };

}

#endif

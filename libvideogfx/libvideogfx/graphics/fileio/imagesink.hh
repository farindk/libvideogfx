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
 *********************************************************************/

#ifndef LIBVIDEOGFX_GRAPHICS_FILEIO_IMAGESINK_HH
#define LIBVIDEOGFX_GRAPHICS_FILEIO_IMGAESINK_HH

#include <fstream>
#include <iostream>

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
    Format_JPEG, Format_PPM, Format_PGM // , Format_PlanarYUV420
  };

  class ImageSink_Save : public ImageSink
  {
  public:
    ImageSink_Save(ImageFileFormat fmt = Format_JPEG);
    ~ImageSink_Save();

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

/*********************************************************************
  libvideogfx/graphics/fileio/unified_loader.hh

  purpose:
    Makes the loading of files much easier by providing a unified
    interface to loading any kind of images and sequences.
    The input can even be modified by a set of filters like
    deinterlacing or cropping. A further nice feature is to
    save parts of the input-definition pipeline as a macro,
    so that sequences can be accessed very easily.

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
   30/Jul/04 - Dirk Farin - first implementation
 ********************************************************************************
    LibVideoGfx - video processing library
    Copyright (C) 2004  Dirk Farin

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

#ifndef LIBVIDEOGFX_GRAPHICS_FILEIO_UNIFIED_LOADER_HH
#define LIBVIDEOGFX_GRAPHICS_FILEIO_UNIFIED_LOADER_HH

#include <libvideogfx/graphics/datatypes/image.hh>

namespace videogfx {

  class LoaderPlugin
  {
  public:
    virtual ~LoaderPlugin() { }

    virtual int  AskNFrames() const = 0;
    virtual bool IsEOF() const = 0;

    virtual bool SkipToImage(int nr) { return false; }
    virtual void ReadImage(Image<Pixel>&) = 0;
  };


  class FileIOFactory
  {
  public:
    virtual ~FileIOFactory() { }

    /* Parse the specification. If the loader factory can handle it, it removes
       the option from the specification and appends it to the plugin pipeline. */
    virtual LoaderPlugin* ParseSpec(char* spec, LoaderPlugin* previous);
  };


  class UnifiedImageLoader
  {
  public:
    ~UnifiedImageLoader() { }

    void SetInput(const char* input_specification);
    void SetTargetColorspace(Colorspace c = Colorspace_Invalid);   // invalid -> keep input colorspace
    void SetTargetChroma(Chroma c = Chroma_Invalid); // invalid -> input input chroma

    // usage

    int  AskNFrames() const;
    bool IsEOF() const;

    bool SkipToImage(int nr);
    void ReadImage(Image<Pixel>&);

  private:
    static const int s_max_loader_plugins;
    static FileIOFactory* s_plugins[s_max_loader_plugins];
  };

}

#endif

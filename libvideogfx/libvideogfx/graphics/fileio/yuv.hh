/*********************************************************************
  libvideogfx/graphics/fileio/yuv.hh

  purpose:
    Read in YUV-Files. The format of the files is as follows:
    First the complete Y-plane is continuously saved. The
    following U- and V-planes are either saved continuously
    one after the other (U comes first) or interleaved.
    That is the bytes are taken alternatingly from the U- and
    V-planes. The chrominance channels may be horizontally or
    vertically (or both) subsampled by a factor of two.

    Additionally a separate alpha-mask file can be read in. 
    This mask always has full resolution.


    Write an YUV image to a ostream as follows: First the complete
    Y plane is written. Then the U and V components are written
    either one after the other or interleaved.
    If you're saving a greyscale only image you can choose between
    saving dummy color information or omitting the U,V data.

    The alpha mask that may be available in the image will be saved
    into a separate ostream if specified.

  notes:
    - You may save entire YUV sequences into a single file by simply
      calling WriteImage() several times.

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
   15/Jul/99 - Dirk Farin - complete rewrite, interleaved output,
                            greyscale output, greyscale input
   25/May/99 - Dirk Farin - first implementation
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

#ifndef LIBVIDEOGFX_GRAPHICS_FILEIO_YUV_HH
#define LIBVIDEOGFX_GRAPHICS_FILEIO_YUV_HH

#include <fstream>
#include <iostream>

#include <libvideogfx/graphics/datatypes/image.hh>

namespace videogfx {

  int YUVFileFrameSize(const ImageParam& param);


  class FileReader_YUV1
  {
  public:
    FileReader_YUV1();
    ~FileReader_YUV1() { }

    // initialization

    void SetYUVStream  (std::istream& yuvstream)   { d_yuvstr = &yuvstream; d_initialized=false; }
    void SetAlphaStream(std::istream& alphastream) { d_alphastr = &alphastream; d_initialized=false; }

    void SetImageParam(const ImageParam& spec) { d_spec = spec; d_initialized=false; }
    void AskImageParam(ImageParam& param) { param = d_spec; }
    void SetInterleavedChroma(bool flag=true)  { d_interleavedUV = flag; d_initialized=false; }
    void SetInputIsGreyscale(bool flag=true) { d_greyscale_input = flag; d_initialized=false; }

    // usage

    int  AskNFrames() const;
    bool IsEOF() const;

    void SkipToImage(int nr);
    void ReadImage(Image<Pixel>&);

  private:
    std::istream* d_yuvstr;
    std::istream* d_alphastr;

    ImageParam d_spec;

    bool  d_interleavedUV;
    bool  d_greyscale_input;

    int   d_nFrames;
    int   d_Framesize;
    int   d_nextFrame;

    void Init();
    bool d_initialized;
  };



  class FileWriter_YUV1
  {
  public:
    FileWriter_YUV1();
    ~FileWriter_YUV1() { }

    void SetYUVStream(std::ostream& str)   { d_yuvstr   = &str; }
    void SetAlphaStream(std::ostream& str) { d_alphastr = &str; }

    void SetWriteGreyscaleAsColor(bool flag=true) { d_write_greyscale_as_color=flag; }
    void SetWriteColorAsGreyscale(bool flag=true) { d_write_color_as_greyscale=flag; }
    void SetWriteInterleaved(bool flag=true)      { d_write_interleaved=flag; }

    void SkipToImage(int nr, const ImageParam& param);
    void WriteImage(const Image<Pixel>&);

  private:
    std::ostream* d_yuvstr;
    std::ostream* d_alphastr;

    bool d_write_greyscale_as_color;
    bool d_write_color_as_greyscale;
    bool d_write_interleaved;
  };

}

#endif

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
   - Dirk Farin, farin@ti.uni-mannheim.de
     University Mannheim, Dept. Circuitry and Simulation
     B 6,26 EG, room 0.10 / D-68131 Mannheim / Germany

  modifications:
   15/Jul/99 - Dirk Farin - complete rewrite, interleaved output,
                            greyscale output, greyscale input
   25/May/99 - Dirk Farin - first implementation
 *********************************************************************/

#ifndef LIBVIDEOGFX_GRAPHICS_FILEIO_YUV_HH
#define LIBVIDEOGFX_GRAPHICS_FILEIO_YUV_HH

#include <fstream>
#include <iostream>
using namespace std;

#include "libvideogfx/graphics/basic/image.hh"


class FileReader_YUV1
{
public:
   FileReader_YUV1();
  ~FileReader_YUV1() { }

  // initialization

  void SetYUVStream  (istream& yuvstream)   { d_yuvstr = &yuvstream; d_initialized=false; }
  void SetAlphaStream(istream& alphastream) { d_alphastr = &alphastream; d_initialized=false; }

  void SetImageParam(const ImageParam& spec) { d_spec = spec; d_initialized=false; }
  void SetInterleavedUV(bool flag=true)    { d_interleavedUV = flag; d_initialized=false; }
  void SetInputIsGreyscale(bool flag=true) { d_greyscale_input = flag; d_initialized=false; }

  // usage

  int  AskNFrames() const;
  bool IsEOF() const;

  void SkipToImage(int nr);
  void ReadImage(Image<Pixel>&);

private:
  istream* d_yuvstr;
  istream* d_alphastr;

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

  void SetYUVStream(ostream& str)   { d_yuvstr   = &str; }
  void SetAlphaStream(ostream& str) { d_alphastr = &str; }

  void SetWriteGreyscaleAsColor(bool flag=true) { d_write_greyscale_as_color=flag; }
  void SetWriteColorAsGreyscale(bool flag=true) { d_write_color_as_greyscale=flag; }
  void SetWriteInterleaved(bool flag=true)      { d_write_interleaved=flag; }

  void WriteImage(const Image<Pixel>&);

private:
  ostream* d_yuvstr;
  ostream* d_alphastr;

  bool d_write_greyscale_as_color;
  bool d_write_color_as_greyscale;
  bool d_write_interleaved;
};

#endif

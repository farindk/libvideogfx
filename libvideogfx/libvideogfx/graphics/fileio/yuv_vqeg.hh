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
 *********************************************************************/

#ifndef LIBVIDEOGFX_GRAPHICS_FILEIO_YUV_VQEG_HH
#define LIBVIDEOGFX_GRAPHICS_FILEIO_YUV_VQEG_HH

#include <fstream>
#include <iostream>
using namespace std;

#include "libvideogfx/graphics/datatypes/image.hh"


class FileReader_YUV_VQEG
{
public:
   FileReader_YUV_VQEG();
  ~FileReader_YUV_VQEG() { }

  // initialization

  void SetYUVStream  (istream& yuvstream)   { d_yuvstr = &yuvstream; }
  void SelectResolution625(bool flag) { d_mode625=flag; }

  // usage

  int  AskNFrames() const { return d_mode625 ? 220 : 260; }
  bool IsEOF() const { return d_nextframe>=AskNFrames(); }

  void SkipToImage(int nr);
  void ReadImage(Image<Pixel>&);

private:
  istream* d_yuvstr;

  bool  d_mode625;
  int   d_nextframe;
};

#endif

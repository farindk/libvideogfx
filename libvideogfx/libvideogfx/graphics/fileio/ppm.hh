/*********************************************************************
  fileio/ppm.hh

  purpose:
    PPM-file access functions.

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
   26/Jan/2001 - Dirk Farin - reimplementation
   03/Aug/1999 - Dirk Farin - new functions: ReadImage_PPM()
                                             ReadImage_PPM5()
   29/Jul/1999 - Dirk Farin - files renamed to rw_ppm.*
                            - added ppm loading
   19/Jul/1999 - Dirk Farin - completely rewritten
   29/Jun/1999 - Dirk Farin - first implementation
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


#ifndef LIBVIDEOGFX_GRAPHICS_FILEIO_RWPPM_HH
#define LIBVIDEOGFX_GRAPHICS_FILEIO_RWPPM_HH

#include <fstream>
#include <iostream>

#include <libvideogfx/graphics/datatypes/image.hh>

namespace videogfx {

  /* Reads PPM P5 and P6 types images.
   * If the 'srcimg' parameter is filled in, this image will be used for loading the file.
   *   It is a requirement that the image size is the same as the image file size.
   * If the 'spec' parameter is filled in, a new image will be created that uses most of
   *   the fields set in 'spec'. But width and height will be overwritten with the values from the file.
   * If none of both is specified, a completely new image without border and alignment is created.
   */

  void ReadImage_PPM(Image<Pixel>& dest,std::istream& stream);
  void ReadImage_PPM(Image<Pixel>& dest,const char* filename);
  int  ReadImage_PPM(Image<uint16>& dest,std::istream& stream);  // returns the maximum value (usually 255 or 65535)
  int  ReadImage_PPM(Image<uint16>& dest,const char* filename);


  /* Write RGB image into PPM P6-type file.
   */
  /* Write luminance part of YUV image into PPM P5-type file.
   * NOTE: the chrominance will simply be ignored.
   */
  void WriteImage_PPM(const Image<Pixel>&,std::ostream& stream);
  void WriteImage_PPM(const Image<Pixel>&,const char* filename);


  void WritePPM6(const Bitmap<Pixel>& r, const Bitmap<Pixel>& g, const Bitmap<Pixel>& b, std::ostream&);
  void WritePPM5(const Bitmap<Pixel>& pm, std::ostream&);


  class FileWriter_PPM
  {
  public:
    FileWriter_PPM();

    void WriteYUVAsGreyscale(bool flag) { d_yuv_as_grey=flag; }
    void WriteRGBAsGreyscale(bool flag) { d_rgb_as_grey=flag; }
    void WriteGreyscaleAsRGB(bool flag) { d_grey_as_rgb=flag; }

    void Write(const Image<Pixel>&,const char* filename);
    void Write(const Image<Pixel>&,std::ostream&);

  private:
    bool d_yuv_as_grey;
    bool d_rgb_as_grey;
    bool d_grey_as_rgb;
  };

}

#endif

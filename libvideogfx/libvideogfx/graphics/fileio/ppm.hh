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
 *********************************************************************/

#ifndef LIBVIDEOGFX_GRAPHICS_FILEIO_RWPPM_HH
#define LIBVIDEOGFX_GRAPHICS_FILEIO_RWPPM_HH

#include <fstream>
#include <iostream>
using namespace std;

#include <libvideogfx/graphics/datatypes/image.hh>


/* Reads PPM P5 and P6 types images.
 * If the 'srcimg' parameter is filled in, this image will be used for loading the file.
 *   It is a requirement that the image size is the same as the image file size.
 * If the 'spec' parameter is filled in, a new image will be created that uses most of
 *   the fields set in 'spec'. But width and height will be overwritten with the values from the file.
 * If none of both is specified, a completely new image without border and alignment is created.
 */

void ReadImage_PPM(Image<Pixel>& dest,istream& stream);
void ReadImage_PPM(Image<Pixel>& dest,const char* filename);


/* Write RGB image into PPM P6-type file.
 */
/* Write luminance part of YUV image into PPM P5-type file.
 * NOTE: the chrominance will simply be ignored.
 */
void WriteImage_PPM(const Image<Pixel>&,ostream& stream);
void WriteImage_PPM(const Image<Pixel>&,const char* filename);


void WritePPM6(const Bitmap<Pixel>& r, const Bitmap<Pixel>& g, const Bitmap<Pixel>& b, ostream&);
void WritePPM5(const Bitmap<Pixel>& pm, ostream&);


class FileWriter_PPM
{
public:
  FileWriter_PPM();

  void WriteYUVAsGreyscale(bool flag) { d_yuv_as_grey=flag; }
  void WriteRGBAsGreyscale(bool flag) { d_rgb_as_grey=flag; }
  void WriteGreyscaleAsRGB(bool flag) { d_grey_as_rgb=flag; }

  void Write(const Image<Pixel>&,const char* filename);
  void Write(const Image<Pixel>&,ostream&);

private:
  bool d_yuv_as_grey;
  bool d_rgb_as_grey;
  bool d_grey_as_rgb;
};

#endif

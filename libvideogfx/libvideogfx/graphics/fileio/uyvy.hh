/*********************************************************************
  fileio/uyvy.hh

  purpose:
    Functions for loading and saving UYVY files of any size.

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
   26/Jan/2002 - Dirk Farin - reimplementation
   15/Jul/1999 - Dirk Farin - completely rewritten
   18/Jun/9999 - Dirk Farin - first implementation
 *********************************************************************/

#ifndef LIBVIDEOGFX_GRAPHICS_FILEIO_UYVY_HH
#define LIBVIDEOGFX_GRAPHICS_FILEIO_UYVY_HH

#include <fstream>
#include <iostream>
using namespace std;

#include <libvideogfx/graphics/basic/image.hh>


// Return true if the image file size matches the specified size.
bool CheckFileSize_UYVY(ifstream& istr,int w,int h);

void ReadImage_UYVY (Image<Pixel>&,ifstream& istr,int w,int h);
void WriteImage_UYVY(const Image<Pixel>&,ofstream& ostr);

#endif

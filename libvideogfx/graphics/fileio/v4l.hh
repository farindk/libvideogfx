/*********************************************************************
  v4l.hh

  purpose:
    Interface to Video4Linux grabbing interface.

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
   11/Jul/2000 - Dirk Farin - first implementation
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

#ifndef LIBVIDEOGFX_GRAPHICS_FILEIO_V4L_HH
#define LIBVIDEOGFX_GRAPHICS_FILEIO_V4L_HH

#include <libvideogfx/graphics/datatypes/image.hh>


namespace videogfx {

  class V4L_Grabber
  {
  public:
    V4L_Grabber();
    ~V4L_Grabber();

    // initialization

    void SetDevice(const char* device);
    void SetGreyscaleMode(bool greyscale=true) { d_greyscale=greyscale; }
    void SetChroma(ChromaFormat chroma) { d_chroma=chroma; }
    void DoChromaAvgOn422To420(bool flag=true) { d_avg_422_to_420=flag; }
    void SetResolution(int w,int h);
    void AskResolution(int& w,int& h) const { w = d_width; h = d_height; }

    void StartGrabbing();

    void Grab(Image<Pixel>&);

  private:
    char* d_device;
    int  d_fd;
    int  palette;

    int d_width,d_height;
    bool d_greyscale;
    ChromaFormat d_chroma;
    bool d_avg_422_to_420;

    struct GrabData* d_grabdata;

    int d_nextbuf;
  };

}

#endif

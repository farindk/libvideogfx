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
 *********************************************************************/

#ifndef LIBVIDEOGFX_GRAPHICS_FILEIO_V4L_HH
#define LIBVIDEOGFX_GRAPHICS_FILEIO_V4L_HH

#include <libvideogfx/graphics/datatypes/image.hh>


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
  void AskResolution(int& w,int& h) { w = d_width; h = d_height; }

  void StartGrabbing();

  void Grab(Image<Pixel>&);

private:
  char* d_device;
  int  d_fd;

  int d_width,d_height;
  bool d_greyscale;
  ChromaFormat d_chroma;
  bool d_avg_422_to_420;

  struct GrabData* d_grabdata;

  int d_nextbuf;
};

#endif

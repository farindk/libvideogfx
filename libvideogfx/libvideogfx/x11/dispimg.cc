/********************************************************************************
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

#include "config.h"

#include <sys/types.h>
#include <assert.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>

#include <iostream>
#include <iomanip>
using namespace std;

namespace X11 {
#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>
#include <X11/Xutil.h>
#include <X11/Xmd.h>
#include <X11/extensions/shmproto.h>

#if HAVE_XV
#include <X11/extensions/Xvlib.h>
#endif

  extern "C" {
    extern int XShmGetEventBase(Display*);   // TODO: where can this function declaration be found ?
  }
};

using namespace X11;

extern "C" {
#include <stdio.h>
}

#include "libvideogfx/types.hh"

#include "server.hh"
#include "dispimg.hh"


namespace videogfx {

  struct DisplayImage_Data
  {
    DisplayImage_Data()
      : mayUseMITSHM(true),
	mayUseXv(true),
	d_initialized(false),
	d_display(NULL),
	d_server(NULL),
	d_grabbed_port(false),
	d_data(NULL)
    {
    }

    ~DisplayImage_Data()
    {
      if (d_initialized)
	{
	  if (d_UseShmExt)
	    {
	      XShmDetach(d_display,&d_ShmSegInfo);
	      if (d_ximg) XDestroyImage(d_ximg);
#if HAVE_XV
	      if (d_xvimg) XFree(d_xvimg);
#endif
	      shmdt(d_ShmSegInfo.shmaddr);
	      shmctl(d_ShmSegInfo.shmid,IPC_RMID,0);
	    }
	  else
	    {
	      d_ximg->data = NULL;
	      XDestroyImage(d_ximg);
	      if (d_data) delete[] d_data;
	    }

	  XFreeGC(d_display, d_gc);
	}

#if HAVE_XV
      if (d_grabbed_port) XvUngrabPort(d_display,d_xvport,0);
#endif

      if (d_server) delete d_server;
    }

#if HAVE_XV
    bool GetXvAdaptorPort(XvPortID& port) const;
    bool XvSupportsFormat_YV12(XvPortID port) const;
#endif

    // user parameters

    bool mayUseMITSHM;
    bool mayUseXv;

    // internal parameters

    bool        d_initialized;
    Display*    d_display;
    X11ServerConnection* d_server;

    bool        d_UseShmExt;
    bool        d_UseXv;

    Window   d_win;
    GC       d_gc;

    XImage*  d_ximg;
#if HAVE_XV
    XvImage* d_xvimg;
    XvPortID d_xvport;
#endif
    bool     d_grabbed_port;

    uint8*   d_data;

    XShmSegmentInfo d_ShmSegInfo;
    bool     d_WaitForCompletion;
    int      d_CompletionType;

    int d_width,d_height;
  };


  static int shmmajor;

  static bool shmfailed;
  static int shmhandler(Display* display,XErrorEvent* err)
  {
    if (err->request_code == shmmajor &&
	err->minor_code == X_ShmAttach)
      shmfailed=true;

    return 0;
  }


  DisplayImage_X11::DisplayImage_X11()
  {
    d_data = new DisplayImage_Data;
  }


  void DisplayImage_X11::UseMITSHM(bool flag) { d_data->mayUseMITSHM=flag; }
  void DisplayImage_X11::UseXv(bool flag) { d_data->mayUseXv=flag; }

  bool DisplayImage_X11::UsesXv() const
  {
#if HAVE_XV
    return d_data->d_xvimg != NULL;
#else
    return false;
#endif
  }

  XImage&  DisplayImage_X11::AskXImage()  { assert(d_data->d_ximg);  return *d_data->d_ximg;  }
#if HAVE_XV
  void* DisplayImage_X11::AskXvImage() { assert(d_data->d_xvimg); return (void*)(d_data->d_xvimg); }
#endif

#if HAVE_XV
  bool DisplayImage_Data::XvSupportsFormat_YV12(XvPortID port) const
  {
    XvImageFormatValues * formatValues;
    int formats;

    formatValues = XvListImageFormats (d_display, port, &formats);
    for (int i = 0; i < formats; i++)
      if ((formatValues[i].id == 0x32315659) &&
	  (strcmp (formatValues[i].guid, "YV12")==0)) {
	XFree (formatValues);
	return true;
      }
    XFree (formatValues);
    return false;
  }


  bool DisplayImage_Data::GetXvAdaptorPort(XvPortID& port) const
  {
    unsigned int version,release,dummy;

    if ((XvQueryExtension(d_display, &version, &release,
			  &dummy, &dummy, &dummy) != Success) ||
	(version < 2) || ((version==2) && (release < 2)))
      { return false; }

    unsigned int nAdaptors;
    XvAdaptorInfo* adaptorInfo;
    XvQueryAdaptors(d_display, d_win, &nAdaptors, &adaptorInfo);

    for (unsigned int i = 0; i < nAdaptors; i++)
      if (adaptorInfo[i].type & XvImageMask)
	for (unsigned int j = 0; j < adaptorInfo[i].num_ports; j++)
	  if (XvSupportsFormat_YV12(adaptorInfo[i].base_id + j) &&
	      (XvGrabPort (d_display, adaptorInfo[i].base_id + j, 0) == Success))
	    {
	      port = adaptorInfo[i].base_id + j;
	      XvFreeAdaptorInfo (adaptorInfo);

	      return true;
	    }

    XvFreeAdaptorInfo (adaptorInfo);
    //cerr << "Cannot find xv port\n";
    return false;
  }
#endif


  void DisplayImage_X11::Create(int w,int h,Window win,X11Server* server)
  {
    if (d_data->d_initialized)
      return;

#if HAVE_XV
    d_data->d_xvimg = NULL;
#endif
    d_data->d_ximg  = NULL;

    d_data->d_width  = w;
    d_data->d_height = h;
    int roundedwidth  = (w+15); roundedwidth  -= roundedwidth %16;
    //int roundedheight = (h+15); roundedheight -= roundedheight%16;

    // Connect to X11 server

    if (server)
      d_data->d_server = new X11ServerConnection(server);
    else
      d_data->d_server = new X11ServerConnection;

    d_data->d_display = d_data->d_server->AskDisplay();
    d_data->d_win = win;


    // Choose VisualInfo

    XWindowAttributes winattr;
    XGetWindowAttributes(d_data->d_display,win,&winattr);

    VisualID visualid = XVisualIDFromVisual(winattr.visual);
    XVisualInfo vinfo_template;
    vinfo_template.visualid   = visualid;

    XVisualInfo* vinfo;
    int nvinfos;
    vinfo=XGetVisualInfo(d_data->d_display,VisualIDMask,&vinfo_template,&nvinfos);
    assert(vinfo != NULL);
    assert(nvinfos==1);

    //cout << "VISUAL-ID used for image: 0x" << hex << visualid << dec << endl;

    XGCValues gcvals;
    d_data->d_gc = XCreateGC(d_data->d_display,win,0,&gcvals);


    // Set Colormap

    // TODO

    // Create XImage structure

    if (d_data->mayUseMITSHM && XShmQueryExtension(d_data->d_display))
      {
	int dummy;
	int major_version,minor_version,pixmap_flag;

	if (!XShmQueryVersion(d_data->d_display, &major_version, &minor_version,&pixmap_flag)
	    || !XQueryExtension(d_data->d_display, "MIT-SHM", &dummy, &dummy, &dummy))
	  {
	    // ShowNote(ErrSev_Note,"X11 shared memory (MITSHM) extension not supported");
	    d_data->d_UseShmExt=false;
	  }
	else
	  {
	    //char buffer[1000];
	    //sprintf(buffer,"X11 shared memory (MITSHM) extensions version %d.%d detected.",major_version, minor_version);
	    //ShowNote(ErrSev_Note,buffer);
	    d_data->d_UseShmExt = true;
	  }
      }
    else
      {
	/*
	  if (mayUseMITSHM)
	  ShowNote(ErrSev_Note,"X11 shared memory (MITSHM) extension not supported");
	  else
	  ShowNote(ErrSev_Note,"X11 shared memory (MITSHM) extension disabled");
	*/

	d_data->d_UseShmExt=false;
      }

  tryagain:
    if (d_data->d_UseShmExt)
      {
	int memory_size;

	// try to use Xv-Extension

#if HAVE_XV
	if (d_data->mayUseXv && d_data->GetXvAdaptorPort(d_data->d_xvport))
	  {
	    d_data->d_xvimg = XvShmCreateImage(d_data->d_display, d_data->d_xvport, 0x32315659,
					       (char*)0,roundedwidth,h,&d_data->d_ShmSegInfo);

	    assert(d_data->d_xvimg);

	    memory_size = d_data->d_xvimg->data_size;
	  }
	else
#endif
	  {
	    // fall-back to standard X11

	    d_data->d_ximg = XShmCreateImage(d_data->d_display,winattr.visual,vinfo->depth,ZPixmap,(char*)0,
					     &d_data->d_ShmSegInfo,roundedwidth,h);
	    if (!d_data->d_ximg)
	      { 
		//TODO
		assert(0);
		//throw Excpt_Base(ErrSev_Error,"XShmCreateImage failed");
	      }

	    memory_size = d_data->d_ximg->bytes_per_line*h;
	  }

	d_data->d_ShmSegInfo.shmid    = shmget(IPC_PRIVATE,memory_size, IPC_CREAT|0604);
	if (d_data->d_ShmSegInfo.shmid==-1)
	  {
	    perror("shmget failed: ");
	    d_data->d_UseShmExt = false;
	    goto tryagain;
	    //assert(0);
	  } // throw Excpt_Base(ErrSev_Error,"shmget failed"); }

	d_data->d_ShmSegInfo.shmaddr  = (char*)shmat(d_data->d_ShmSegInfo.shmid,0,0);
	uint8* mem = (uint8*)d_data->d_ShmSegInfo.shmaddr;

	if (d_data->d_ximg) d_data->d_ximg->data = d_data->d_ShmSegInfo.shmaddr;
#if HAVE_XV
	if (d_data->d_xvimg) d_data->d_xvimg->data = d_data->d_ShmSegInfo.shmaddr;
#endif

	if (d_data->d_ShmSegInfo.shmaddr==((char *)-1))
	  { perror("shmat failed: "); assert(0); } // throw Excpt_Base(ErrSev_Error,"shmat failed"); }
	d_data->d_ShmSegInfo.readOnly = True;

	int dummy;
	XQueryExtension(d_data->d_display,"MIT-SHM",&shmmajor,&dummy,&dummy);

	shmfailed=false;
	XSetErrorHandler(shmhandler);

	Status xshma;
	xshma=XShmAttach(d_data->d_display,&d_data->d_ShmSegInfo);
	XSync(d_data->d_display,False);

	XSetErrorHandler(NULL);

	shmctl(d_data->d_ShmSegInfo.shmid, IPC_RMID, 0);

	if (!xshma)
	  { assert(0); } // throw Excpt_Base(ErrSev_Error,"XShmAttach failed");

	if (shmfailed)
	  {
	    cerr << "MIT-SHM failed, falling back to network mode.\n";
	    XDestroyImage(d_data->d_ximg);
	    shmdt(d_data->d_ShmSegInfo.shmaddr);
	    shmctl(d_data->d_ShmSegInfo.shmid,IPC_RMID,0);
	    d_data->d_UseShmExt = false;
	    goto tryagain;
	  }
      
	d_data->d_data = mem;
	d_data->d_CompletionType = XShmGetEventBase(d_data->d_display) + ShmCompletion;
      }
    else
      {
	d_data->d_ximg = XCreateImage(d_data->d_display,vinfo->visual,vinfo->depth,
				      ZPixmap,0,NULL,roundedwidth,h,32,0);
	d_data->d_data = new uint8[d_data->d_ximg->bytes_per_line*h];
	d_data->d_ximg->data = (char*)d_data->d_data;
      }

    d_data->d_WaitForCompletion=false;

    XSync(d_data->d_display,False);

    XFree(vinfo);

    d_data->d_initialized = true; 
  }


  DisplayImage_X11::~DisplayImage_X11()
  {
    delete d_data;
  }


  void DisplayImage_X11::PutImage(int srcx0,int srcy0,int w,int h, int dstx0,int dsty0)
  {
    if (w==0) w=d_data->d_width;
    if (h==0) h=d_data->d_height;

    if (0 && d_data->d_WaitForCompletion)
      while (1)
	{
	  XEvent xev;
        
	  XNextEvent(d_data->d_display, &xev);
	  if (xev.type == d_data->d_CompletionType)
	    break;
	}

#if HAVE_XV
    if (d_data->d_xvimg)
      {
	XvShmPutImage(d_data->d_display, d_data->d_xvport, d_data->d_win,
		      d_data->d_gc, d_data->d_xvimg,
		      srcx0, srcy0, w,h,
		      dstx0, dsty0, w,h,
		      True);

	XFlush(d_data->d_display);
	d_data->d_WaitForCompletion=true;
      }
    else
#endif
      if (d_data->d_UseShmExt)
	{
	  XShmPutImage(d_data->d_display, d_data->d_win, d_data->d_gc, d_data->d_ximg, srcx0, srcy0, dstx0, dsty0, w,h, True);

	  XFlush(d_data->d_display);
	  d_data->d_WaitForCompletion=true;
	}
      else
	{
	  XPutImage(d_data->d_display, d_data->d_win, d_data->d_gc, d_data->d_ximg, srcx0, srcy0, dstx0, dsty0, w,h);
	  XFlush(d_data->d_display);
	}
  }

}

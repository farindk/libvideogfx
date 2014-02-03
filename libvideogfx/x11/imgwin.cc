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
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <iostream>
#include <iomanip>
using namespace std;

#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>
#include <X11/Xutil.h>

#if HAVE_XV
#include <X11/extensions/Xvlib.h>
#endif


#include "server.hh"
#include "imgwin.hh"

namespace videogfx {

  struct X11SpecificData
  {
    Display*  d_display;
    Window    d_win;
  };


  ImageWindow_X11::ImageWindow_X11()
    : d_initialized(false),
      d_server(NULL),
      d_xpos(-1),d_ypos(-1)
  {
    d_x11data = new X11SpecificData;
  }


  ImageWindow_X11::~ImageWindow_X11()
  {
    //printf("ImageWindow_X11::~ImageWindow_X11()\n");
    Close();

    if (d_x11data) { delete d_x11data; d_x11data=NULL; }
    if (d_server) delete d_server;
  }

  void ImageWindow_X11::Close()
  {
    if (!d_initialized)
      return;

    XUnmapWindow(d_x11data->d_display , d_x11data->d_win);
    XFlush(d_x11data->d_display);
    d_initialized=false;
  }


  Window   ImageWindow_X11::AskWindow()  { assert(d_initialized); return d_x11data->d_win; }
  Display* ImageWindow_X11::AskDisplay() { return d_x11data->d_display; }


  void ImageWindow_X11::Create(int w,int h,const char* title,X11Server* server,Window parent)
  {
    assert(!d_initialized);

    // Get X11 server.

    if (d_server) delete d_server;

    if (server)
      d_server = new X11ServerConnection(server);
    else
      d_server = new X11ServerConnection;

    d_x11data->d_display = d_server->AskDisplay();

    int screen = DefaultScreen(d_x11data->d_display);
    Window rootwin = RootWindow(d_x11data->d_display,screen);


    // Choose VisualInfo

    XVisualInfo vinfo;
    //bool use_cmap8=false;

    if (XMatchVisualInfo(d_x11data->d_display, screen, 16, TrueColor, &vinfo))
      {
      }
    else
      if (XMatchVisualInfo(d_x11data->d_display, screen, 15, TrueColor, &vinfo))
	{
	}
      else
	if (XMatchVisualInfo(d_x11data->d_display, screen, 24, TrueColor, &vinfo))
	  {
	  }
	else
	  if (XMatchVisualInfo(d_x11data->d_display, screen, 32, TrueColor, &vinfo))
	    {
	    }
	  else
	    if (XMatchVisualInfo(d_x11data->d_display, screen,  8, PseudoColor, &vinfo))
	      {
		// use_cmap8=true;
	      }
	    else
	      if (XMatchVisualInfo(d_x11data->d_display, screen,  4, StaticGray, &vinfo))
		{
		}
#if 0
	      else
		if (XMatchVisualInfo(d_x11data->d_display, screen,  8, GrayScale, &vinfo))
		  {
		  }
#endif
		else
		  {
		    // TODO
		    cerr << "no matching visual found\n";
		    exit(10);
		    // throw Excpt_Base(ErrSev_Error,"I'm sorry, no matching visual info found.");
		  }

    //cout << "VISUAL-ID used for window: 0x" << hex << vinfo.visualid << dec << endl;

    // Create window

    Colormap theCmap = XCreateColormap(d_x11data->d_display, rootwin, vinfo.visual, AllocNone);

    XSetWindowAttributes attr;
    attr.colormap = theCmap;
    attr.background_pixel = 0;
    attr.border_pixel     = 1;

    Window parent_window;

#if 1
    if (parent)
      parent_window = parent;
    else
#endif
      parent_window = RootWindow(d_x11data->d_display,screen);

    //printf("WINID: %d %p\n",parent,parent);

    d_x11data->d_win = XCreateWindow(d_x11data->d_display, parent_window,
				     d_xpos,d_ypos,w,h, 2, vinfo.depth, InputOutput, vinfo.visual,
				     CWBackPixel|CWBorderPixel|CWColormap,&attr);
  

    XSizeHints* sizeh;
    if (!(sizeh = XAllocSizeHints()))
      { fprintf(stderr,"cannot allocate XSizeHints structure\n"); exit(0); }

    XWMHints* wm_hints;
    if (!(wm_hints = XAllocWMHints()))
      { fprintf(stderr,"cannot allocate XWMHints structure\n"); exit(0); }

    XClassHint* classhint;
    if (!(classhint = XAllocClassHint()))
      { fprintf(stderr,"cannot allocate XClassHint structure\n"); exit(0); }

    sizeh->flags |= PPosition|PSize|PMinSize|PMaxSize;
    sizeh->width  = w;
    sizeh->height = h;
    sizeh->min_width  = w;
    sizeh->min_height = h;
    sizeh->max_width  = w;
    sizeh->max_height = h;

    XTextProperty windowName, iconName;

    if (XStringListToTextProperty((char**)&title,1,&windowName)==0)
      { fprintf(stderr,"cannot allocate structure for windowName\n"); exit(0); }

    if (XStringListToTextProperty((char**)&title,1,&iconName)==0)
      { fprintf(stderr,"cannot allocate structure for iconName\n"); exit(0); }

    wm_hints->initial_state = NormalState;
    wm_hints->input = True;
    wm_hints->flags = StateHint | InputHint;

    classhint->res_name  = NULL;
    classhint->res_class = "Libvideogfx";

    XSetWMProperties(d_x11data->d_display, d_x11data->d_win, &windowName, &iconName,
		     NULL,0, sizeh, wm_hints, classhint);
  
    XSelectInput(d_x11data->d_display, d_x11data->d_win, ExposureMask|KeyPressMask|PointerMotionMask|
		 ButtonPressMask|ButtonReleaseMask);
    XMapWindow(d_x11data->d_display,d_x11data->d_win);
    XFlush(d_x11data->d_display);

    while (1)
      {
	XEvent xev;
	XNextEvent(d_x11data->d_display,&xev);

	if (xev.type == Expose)
	  break;
      }

    // Set Colormap

    // TODO

    d_initialized = true; 
  }


  ImageWindow_Autorefresh_X11::ImageWindow_Autorefresh_X11(bool useXv)
    : d_dispimg(NULL),
      d_rgbtransform(NULL)
  {
    d_dispimg = new DisplayImage_X11;
    d_dispimg->UseXv(useXv);

    if (!useXv)
      d_rgbtransform = new Image2RawRGB;
    else
      d_rgbtransform = NULL;
  }

  ImageWindow_Autorefresh_X11::~ImageWindow_Autorefresh_X11()
  {
    delete d_dispimg;
    if (d_rgbtransform) delete d_rgbtransform;
  }

  void ImageWindow_Autorefresh_X11::Create(int w,int h,const char*title,X11Server* server,Window win)
  {
    ImageWindow_X11::Create(w,h,title,server,win);

    d_dispimg->Create(w,h,AskWindow(),server);


    // set output specs for RGB transformation

    if (d_rgbtransform)
      {
	XImage& ximg = d_dispimg->AskXImage();

	RawRGBImageSpec spec;
	spec.bytes_per_line = ximg.bytes_per_line;
	spec.bits_per_pixel = ximg.bits_per_pixel;
	spec.little_endian  = (ximg.byte_order==LSBFirst);
	spec.SetRGBMasks(ximg.red_mask,ximg.green_mask,ximg.blue_mask);

	d_rgbtransform->SetOutputSpec(spec);
      }
  }

  void ImageWindow_Autorefresh_X11::Close()
  {
    ImageWindow_X11::Close();
  }


  void ImageWindow_Autorefresh_X11::Display(const Image<Pixel>& img)
  {
#if HAVE_XV
    if (d_dispimg->UsesXv())
      {
	assert(img.AskParam().colorspace == Colorspace_YUV);

	XvImage* xvimg = (XvImage*)d_dispimg->AskXvImage();

	const Pixel*const* yp = img.AskFrameY();
	const Pixel*const* up = img.AskFrameU();
	const Pixel*const* vp = img.AskFrameV();

	int w = img.AskWidth();
	int h = img.AskHeight();
	int rw = (w+15) & ~0xF;

	ImageParam param = img.AskParam();

	int cw,ch;
	param.AskChromaSizes(cw,ch);
	int rcw = (cw+7) & ~0x7;
      
	for (int y=0;y<h;y++)
	  memcpy(&xvimg->data[rw*y],yp[y],w);

	for (int y=0;y<ch;y++)
	  memcpy(&xvimg->data[rw*h+rcw*y],vp[y],cw);

	for (int y=0;y<ch;y++)
	  memcpy(&xvimg->data[rw*h+rcw*ch+rcw*y],up[y],cw);

	d_dispimg->PutImage();
      }
    else
#endif
      {
	d_rgbtransform->Transform(img,(uint8*)(d_dispimg->AskXImage().data));
	d_dispimg->PutImage();
      }
  }


  void ImageWindow_Autorefresh_X11::CheckForRedraw()
  {
    XEvent event;
    while (XCheckWindowEvent(AskDisplay(),AskWindow(),ExposureMask,&event))
      {
	Redraw(event.xexpose);
      }
  }


  void ImageWindow_Autorefresh_X11::RedrawForever()
  {
    XEvent event;
    for (;;)
      {
	XWindowEvent(AskDisplay(),AskWindow(),ExposureMask,&event);
	Redraw(event.xexpose);
      }
  }


  bool ImageWindow_Autorefresh_X11::CheckForMouseMove(int& x,int& y)
  {
    XEvent event;
    if (XCheckWindowEvent(AskDisplay(),AskWindow(),PointerMotionMask,&event))
      {
	x = event.xmotion.x;
	y = event.xmotion.y;
	return true;
      }
    else
      return false;
  }

  int  ImageWindow_Autorefresh_X11::CheckForMouseButton(int& x,int& y)
  {
    XEvent event;
    if (XCheckWindowEvent(AskDisplay(),AskWindow(),ButtonPressMask,&event))
      {
	x = event.xbutton.x;
	y = event.xbutton.y;
	return event.xbutton.button;
      }
    else if (XCheckWindowEvent(AskDisplay(),AskWindow(),ButtonReleaseMask,&event))
      {
	x = event.xbutton.x;
	y = event.xbutton.y;
	return -event.xbutton.button;
      }
    else
      return 0;
  }

  char ImageWindow_Autorefresh_X11::CheckForKeypress()
  {
    XEvent event;
    if (XCheckWindowEvent(AskDisplay(),AskWindow(),KeyPressMask,&event))
      {
	char buf;

	if (XLookupString(&event.xkey,&buf,1,NULL,NULL) > 0)
	  return buf;
	else
	  return 0;
      }
    else
      return 0;
  }


  char ImageWindow_Autorefresh_X11::WaitForKeypress()
  {
    XEvent event;

    for (;;)
      {
	XWindowEvent(AskDisplay(),AskWindow(),KeyPressMask|ExposureMask,&event);

	if (event.type == Expose)
	  {
	    Redraw(event.xexpose);
	  }
	else
	  {
	    char buf;

	    if (XLookupString(&event.xkey,&buf,1,NULL,NULL) > 0)
	      return buf;
	    else
	      return 0;
	  }
      }
  }


  void ImageWindow_Autorefresh_X11::Redraw(XExposeEvent& ev)
  {
    d_dispimg->PutImage(ev.x     , ev.y,
			ev.width , ev.height,
			ev.x     , ev.y);
  }


  /* All windows have to be on the same X-server.
   */
  int MultiWindowRefresh(ImageWindow_Autorefresh_X11*const* windows,int nWindows)
  {
    XEvent ev;
    bool refresh_occured=false;

    while (!refresh_occured)
      {
	XMaskEvent(windows[0]->AskDisplay(),ExposureMask|KeyPressMask|PointerMotionMask,&ev);
	for (int i=0;i<nWindows;i++)
	  {
	    if (ev.xany.window == windows[i]->AskWindow())
	      {
		if (ev.type == Expose)
		  {
		    windows[i]->Redraw(ev.xexpose);
		    refresh_occured=true;
		  }
		else
		  {
		    XPutBackEvent(windows[0]->AskDisplay(), &ev);
		  }

		return i;
	      }
	  }
      }

    return -1;
  }

  void DisplayX11(const Image<Pixel>& img, int wait_usecs)
  {
    int w=img.AskWidth(), h=img.AskHeight();
    ImageWindow_Autorefresh_X11 win;
    win.Create(w,h,"libvideogfx (c) Dirk Farin");
    win.Display(img);
    if (wait_usecs) usleep(wait_usecs); else while (win.WaitForKeypress() != 'q') { }
  }
}

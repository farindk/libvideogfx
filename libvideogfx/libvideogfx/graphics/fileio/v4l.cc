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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <linux/videodev.h>

#include "libvideogfx/graphics/fileio/v4l.hh"

namespace videogfx {
  using namespace std;

  struct GrabData
  {
    unsigned char* d_map;
    struct video_mbuf d_vidmbuf;
    struct video_mmap d_vidmmap;
  };


  V4L_Grabber::V4L_Grabber()
    : d_device(NULL),
      d_fd(-1),
      d_greyscale(false),
      d_chroma(Chroma_420),
      d_avg_422_to_420(false)
  {
    SetDevice("/dev/video0");

    d_width  = 384;
    d_height = 288;

    d_grabdata = new GrabData;
  }

  V4L_Grabber::~V4L_Grabber()
  {
    if (d_device) delete[] d_device;

    if (d_fd>=0)
      close(d_fd);

    delete d_grabdata;
  }


  void V4L_Grabber::SetDevice(const char* device)
  {
    if (d_device) delete[] d_device;

    d_device = new char[strlen(device)+1];
    strcpy(d_device,device);
  }

  void V4L_Grabber::SetResolution(int w,int h)
  {
    d_width  = w;
    d_height = h;
  }

  void V4L_Grabber::StartGrabbing()
  {
    d_fd = open(d_device,O_RDWR);
    if (d_fd==-1)
      { perror("open video-device: "); exit(10); }

    if (-1 == ioctl(d_fd,VIDIOCGMBUF,&d_grabdata->d_vidmbuf)) {
      perror("ioctl VIDIOCGMBUF");
    }


    d_grabdata->d_map = (unsigned char*)mmap(0,d_grabdata->d_vidmbuf.size,
					     PROT_READ|PROT_WRITE,MAP_SHARED,d_fd,0);
    if ((unsigned char*)-1 == d_grabdata->d_map) {
      perror("mmap on video device");
    }



    // Grabbing starten

    d_grabdata->d_vidmmap.width =d_width;
    d_grabdata->d_vidmmap.height=d_height;
    d_grabdata->d_vidmmap.format = (d_greyscale ? VIDEO_PALETTE_GREY : VIDEO_PALETTE_YUV422);

    for (int i=0;i<d_grabdata->d_vidmbuf.frames;i++)
      {
	d_grabdata->d_vidmmap.frame =i;
	if (-1 == ioctl(d_fd,VIDIOCMCAPTURE,&d_grabdata->d_vidmmap)) {
	  perror("ioctl VIDIOCMCAPTURE");
	  exit(10);
	}
      }

    d_nextbuf=0;
  }


  const uint64 mask_Y = 0x00FF00FF00FF00FFLL;

  void V4L_Grabber::Grab(Image<Pixel>& img)
  {
    ImageParam spec;

    Assert(d_greyscale || (d_chroma == Chroma_420 ||
			   d_chroma == Chroma_422));

    spec.width  = d_width;
    spec.height = d_height;
    spec.colorspace = (d_greyscale ? Colorspace_Greyscale : Colorspace_YUV);
    spec.halign  = 16;  // for MMX accelerated format conversion
    spec.chroma  = d_chroma;

    const bool do_avg_422_to_420 = (d_chroma==Chroma_420 && d_avg_422_to_420);

    img.Create(spec);

    if (-1 == ioctl(d_fd,VIDIOCSYNC,&d_nextbuf)) {
      perror("ioctl VIDIOCSYNC");
      exit(10);
    }

    if (d_greyscale)
      {
	Pixel*const* yp=img.AskFrameY();

	unsigned char* mapptr=d_grabdata->d_map + d_grabdata->d_vidmbuf.offsets[d_nextbuf];

	for (int y=0;y<d_height;y++)
	  {
	    memcpy(yp[y],mapptr,d_width);
	    mapptr += d_width;
	  }
      }
    else
      {
	Pixel*const* yp=img.AskFrameY();
	Pixel*const* vp=img.AskFrameU();
	Pixel*const* up=img.AskFrameV();

	unsigned char* mapptr=d_grabdata->d_map + d_grabdata->d_vidmbuf.offsets[d_nextbuf];

#ifndef ENABLE_MMX
	for (int y=0;y<d_height;y++)
	  {
	    cout << "- " << y << endl;

	    if (do_avg_422_to_420)
	      {
		for (int x=0;x<d_width;x++)
		  {
		    yp[y   ][x]    = *mapptr++;
		    vp[y>>1][x>>1] = (mapptr[0] + mapptr[d_width*2])>>1; mapptr++;
		    x++;
		    yp[y   ][x]    = *mapptr++;
		    up[y>>1][x>>1] = (mapptr[0] + mapptr[d_width*2])>>1; mapptr++;
		  }
	      }
	    else
	      {
		for (int x=0;x<d_width;x++)
		  {
		    yp[y   ][x]    = *mapptr++;
		    vp[y>>1][x>>1] = *mapptr++;
		    x++;
		    yp[y   ][x]    = *mapptr++;
		    up[y>>1][x>>1] = *mapptr++;
		  }
	      }


	    if (spec.chroma == Chroma_420)
	      {
		y++;
		for (int x=0;x<d_width;x++)
		  {
		    yp[y][x] = *mapptr;
		    mapptr+=2;
		  }
	      }
	  }
#else
	__asm__
	  (
	   "movq %0,%%mm7\n\t"
	   : : "m"(mask_Y)
	   );

	for (int y=0;y<d_height;y++)
	  {
	    Pixel* dest = yp[y];

	    if (do_avg_422_to_420)
	      {
		for (int x=0;x<d_width;x+=8)
		  {
		    __asm__
		      (
		       "movq  (%1),%%mm1\n\t"
		       "movq 8(%1),%%mm2\n\t"
		       "pand %%mm7,%%mm1\n\t"
		       "pand %%mm7,%%mm2\n\t"
		       "packuswb %%mm2,%%mm1\n\t"
		       "movq %%mm1,(%0)\n\t"

		       : : "r" (dest), "r"(mapptr)
		       );

		    vp[(y>>1)][(x>>1)  ] = (mapptr[ 1]+mapptr[2*d_width+ 1])/2;
		    up[(y>>1)][(x>>1)  ] = (mapptr[ 3]+mapptr[2*d_width+ 3])/2;
		    vp[(y>>1)][(x>>1)+1] = (mapptr[ 5]+mapptr[2*d_width+ 5])/2;
		    up[(y>>1)][(x>>1)+1] = (mapptr[ 7]+mapptr[2*d_width+ 7])/2;
		    vp[(y>>1)][(x>>1)+2] = (mapptr[ 9]+mapptr[2*d_width+ 9])/2;
		    up[(y>>1)][(x>>1)+2] = (mapptr[11]+mapptr[2*d_width+11])/2;
		    vp[(y>>1)][(x>>1)+3] = (mapptr[13]+mapptr[2*d_width+13])/2;
		    up[(y>>1)][(x>>1)+3] = (mapptr[15]+mapptr[2*d_width+15])/2;

		    mapptr += 16;
		    dest   += 8;
		  }
	      }
	    else
	      {
		for (int x=0;x<d_width;x+=8)
		  {
		    __asm__
		      (
		       "movq  (%1),%%mm1\n\t"
		       "movq 8(%1),%%mm2\n\t"
		       "pand %%mm7,%%mm1\n\t"
		       "pand %%mm7,%%mm2\n\t"
		       "packuswb %%mm2,%%mm1\n\t"
		       "movq %%mm1,(%0)\n\t"

		       : : "r" (dest), "r"(mapptr)
		       );

		    vp[(y>>1)][(x>>1)  ] = mapptr[ 1];
		    up[(y>>1)][(x>>1)  ] = mapptr[ 3];
		    vp[(y>>1)][(x>>1)+1] = mapptr[ 5];
		    up[(y>>1)][(x>>1)+1] = mapptr[ 7];
		    vp[(y>>1)][(x>>1)+2] = mapptr[ 9];
		    up[(y>>1)][(x>>1)+2] = mapptr[11];
		    vp[(y>>1)][(x>>1)+3] = mapptr[13];
		    up[(y>>1)][(x>>1)+3] = mapptr[15];

		    mapptr += 16;
		    dest   += 8;
		  }
	      }

	    if (spec.chroma == Chroma_420)
	      {
		y++;

		for (int x=0;x<d_width;x+=8)
		  {
		    __asm__
		      (
		       "movq  (%1),%%mm1\n\t"
		       "movq 8(%1),%%mm2\n\t"
		       "pand %%mm7,%%mm1\n\t"
		       "pand %%mm7,%%mm2\n\t"
		       "packuswb %%mm2,%%mm1\n\t"
		       "movq %%mm1,(%0)\n\t"

		       : : "r" (dest), "r"(mapptr)
		       );

		    mapptr += 16;
		    dest   += 8;
		  }
	      }
	  }

	__asm__
	  (
	   "emms\n\t"
	   );
#endif
      }

    d_grabdata->d_vidmmap.frame =d_nextbuf;
    if (-1 == ioctl(d_fd,VIDIOCMCAPTURE,&d_grabdata->d_vidmmap)) {
      perror("ioctl VIDIOCMCAPTURE");
      exit(10);
    }

    d_nextbuf=1-d_nextbuf;
  }

}

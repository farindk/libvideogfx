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

#include "libvideogfx/graphics/fileio/uyvy.hh"

namespace videogfx {
  using namespace std;

  bool CheckFileSize_UYVY(ifstream& istr,int w,int h)
  {
    long pos = istr.tellg();
    istr.seekg(0,ios::end);
    long filelength = istr.tellg();
    istr.seekg(pos,ios::beg);

    return filelength == w*h*2;
  }


  void ReadImage_UYVY(Image<Pixel>& img,ifstream& istr,int w,int h)
  {
    assert(istr.is_open());

    ImageParam param = img.AskParam();

    param.width      = w;
    param.height     = h;
    param.chroma     = Chroma_422;
    param.colorspace = Colorspace_YUV;

    img.Create(param);

    Pixel*const* yp = img.AskFrameY();
    Pixel*const* up = img.AskFrameU();
    Pixel*const* vp = img.AskFrameV();

    uint8* linebuf = new uint8[w*2];

    for (int y=0;y<h;y++)
      {
	istr.read((char*)linebuf,w*2);

	uint8* lp = linebuf;

	for (int x=0;x<w/2;x++)
	  {
	    // This ugly piece of code helps the compiler to optimize
	    // this a bit further as he doesn't have to mind the pointers
	    // pointing to the same memory locations.
	    // Thus all four assignments can be executed in parallel.

	    uint8 a,b,c,d;

	    a = *lp++;
	    b = *lp++;
	    c = *lp++;
	    d = *lp++;

	    up[y][x]     = a;
	    yp[y][2*x  ] = b;
	    vp[y][x]     = c;
	    yp[y][2*x+1] = d;
	  }
      }

    delete[] linebuf;
  }





  void WriteImage_UYVY(const Image<Pixel>& img,ofstream& ostr)
  {
    ImageParam param = img.AskParam();

    Assert(param.chroma     == Chroma_422);
    Assert(param.colorspace == Colorspace_YUV);


    // Write file

    const Pixel*const* yp = img.AskFrameY();
    const Pixel*const* up = img.AskFrameU();
    const Pixel*const* vp = img.AskFrameV();

    uint8* linebuf = new uint8[param.width*2];

    for (int y=0;y<param.height;y++)
      {
	uint8* lp;
	const uint8* p;

	// luminance

	lp = &linebuf[1];
	p = yp[y];

	for (int x=0;x<param.width;x++)
	  {
	    *lp = *p++;
	    lp+=2;
	  }

	// chrominance

	for (int x=0;x<param.width/2;x++)
	  {
	    // The same ugly code as in ReadImage_UYVY().

	    uint8 a,b;
	    a = up[y][x];
	    b = vp[y][x];
	    linebuf[4*x  ] = a;
	    linebuf[4*x+2] = b;
	  }

	ostr.write((char*)linebuf,param.width*2);
      }

    delete[] linebuf;
  }

}

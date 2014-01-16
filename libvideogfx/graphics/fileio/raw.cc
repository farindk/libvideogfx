/********************************************************************************
    LibVideoGfx - video processing library
    Copyright (C) 2007  Dirk Farin

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

#include "libvideogfx/graphics/fileio/raw.hh"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

namespace videogfx {
  using namespace std;

  RawImageReader::RawImageReader()
  {
    d_w=d_h=512;
    d_bpp=16;
    d_header_bytes=52;
    d_little_endian=true;
  }

  static void read_line(uint32* linebuf, ifstream& istr, int w, int bpp, bool little_endian)
  {
    char* lb = (char*)linebuf;
    istr.read(lb,w*((bpp+7)/8));

    /**/ if (bpp>24) { }
    else if (bpp>16) for(int i=w-1;i>=0;i--) { lb[4*i]=lb[3*i]; lb[4*i+1]=lb[3*i+1]; lb[4*i+2]=lb[3*i+2]; lb[4*i+3]=0; }
    else if (bpp> 8) for(int i=w-1;i>=0;i--) { lb[4*i]=lb[2*i]; lb[4*i+1]=lb[2*i+1]; lb[4*i+2]=lb[4*i+3]=0; }
    else             for(int i=w-1;i>=0;i--) { lb[4*i]=lb[  i]; lb[4*i+1]=lb[4*i+2]=lb[4*i+3]=0; }

    if (little_endian)
      {
	/**/ if (bpp>24) { for (int i=0;i<w;i++) { std::swap(lb[4*i],lb[4*i+3]); std::swap(lb[4*i+1],lb[4*i+2]); } }
	else if (bpp>16)   for (int i=0;i<w;i++) { std::swap(lb[4*i],lb[4*i+2]); }
	else if (bpp> 8)   for (int i=0;i<w;i++) { std::swap(lb[4*i],lb[4*i+1]); }
	else { }
      }
  }

  void RawImageReader::ReadImageData(Image<uint16>& img, ifstream& istr)
  {
    istr.seekg(d_header_bytes);
    
    img.Create(d_w,d_h, Colorspace_Greyscale);
    uint16*const* yp = img.AskFrameY();

    uint32* linebuf = new uint32[d_w];

    int shift = d_bpp%8;
    if (shift) shift=8-shift;

    for (int y=0;y<d_h;y++)
      {
	read_line(linebuf, istr, d_w, d_bpp, d_little_endian);
	uint32* p = linebuf;

	bool do_swap;

#if !WORDS_BIGENDIAN
	char* bp = (char*)linebuf;
	for (int x=0;x<d_w;x++)
	  {
	    std::swap(bp[4*x  ], bp[4*x+3]);
	    std::swap(bp[4*x+1], bp[4*x+2]);
	  }
#endif

	for (int x=0;x<d_w;x++)
	  {
	    uint32 val = *p++;
	    val >>= 16-shift;

	    yp[y][x] = val;
	  }
      }

    delete[] linebuf;
  }

  void RawImageReader::ReadImage(Image<uint16>& img, ifstream& istr)
  {
    istr.seekg(d_header_bytes);
    ReadImageData(img,istr);
  }

  void RawImageReader::ReadImage(Image<uint16>& img, const char* name)
  {
    ifstream istr(name, std::ios::in | std::ios::binary);
    ReadImage(img, istr);
  }



  void RawImageReader::Convert16to8 (Image<Pixel>& img8, Image<uint16>& img16)
  {
    img8.Create(d_w,d_h, Colorspace_Greyscale);

    const uint16*const* sp = img16.AskFrameY();
    /* */ Pixel*const*  dp = img8 .AskFrameY();

    for (int y=0;y<d_h;y++)
      for (int x=0;x<d_w;x++)
	{
	  dp[y][x] = sp[y][x] >> 8;
	}
  }

  void RawImageReader::ReadImage(Image<Pixel>& img, ifstream& istr)
  {
    Image<uint16> img16;
    ReadImage(img16, istr);
    Convert16to8(img, img16);
  }

  void RawImageReader::ReadImage(Image<Pixel>& img, const char* name)
  {
    ifstream istr(name, std::ios::in | std::ios::binary);
    ReadImage(img, istr);
  }


  static uint32 read32(istream& istr)
  {
    uint32 val=0;
    char c;

    istr.read(&c,1); val |= c;
    istr.read(&c,1); val |= c<< 8;
    istr.read(&c,1); val |= c<<16;
    istr.read(&c,1); val |= c<<24;

    return val;
  }

  void RawImageReader::ReadMedicalXRay(Image<uint16>& img, const char* name)
  {
    ifstream istr(name, std::ios::in | std::ios::binary);
    istr.seekg(40 /* header */);

    int w = read32(istr);
    int h = read32(istr);
    int b = read32(istr);

    SetSize(w,h);
    SetBitsPerPixels(b);
    SetHeaderSkip(52);

    ReadImageData(img, istr);

    // estimate endian-ness

    const uint16*const* p = img.AskFrameY();
    int msb_variance = 0;
    int lsb_variance = 0;

    for (int x=0;x<w-1;x++)
      {
	msb_variance += abs((p[h/2][x] >> 8) - (p[h/2][x+1] >> 8));
	lsb_variance += abs((p[h/2][x]&0xFF) - (p[h/2][x+1]&0xFF));
      }

    if (lsb_variance < msb_variance)
      {
	// cout << "wrong endianness, re-reading\n";
	d_little_endian = !d_little_endian;
	ReadImage(img, name);
      }
  }

  void RawImageReader::ReadMedicalXRay(Image<Pixel>& img,  const char* name)
  {
    Image<uint16> img16;
    ReadMedicalXRay(img16, name);
    Convert16to8(img, img16);
  }
}

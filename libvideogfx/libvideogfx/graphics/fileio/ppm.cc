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

#include "libvideogfx/graphics/fileio/ppm.hh"
#include "libvideogfx/error.hh"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

namespace videogfx {
  using namespace std;

  // --- low-level PPM writers

  void WritePPM5(const Bitmap<Pixel>& pm, ostream& ostr)
  {
    const int w = pm.AskWidth();
    const int h = pm.AskHeight();

    ostr << "P5\n" << w << ' ' << h << "\n255\n";

    const Pixel*const* Y = pm.AskFrame();

    for (int y=0;y<h;y++)
      ostr.write((char*)Y[y],w);
  }


  void WritePPM6(const Bitmap<Pixel>& r, const Bitmap<Pixel>& g, const Bitmap<Pixel>& b, ostream& ostr)
  {
    const int w = r.AskWidth();
    const int h = r.AskHeight();

    Assert(w==g.AskWidth() && h==g.AskHeight());
    Assert(w==b.AskWidth() && h==b.AskHeight());

    ostr << "P6\n" << w << ' ' << h << "\n255\n";

    uint8* linebuf = new uint8[w*3];

    const Pixel*const* R = r.AskFrame();
    const Pixel*const* G = g.AskFrame();
    const Pixel*const* B = b.AskFrame();

    for (int y=0;y<h;y++)
      {
	uint8* p = linebuf;
	  
	for (int x=0;x<w;x++)
	  {
	    *p++ = R[y][x];
	    *p++ = G[y][x];
	    *p++ = B[y][x];
	  }

	ostr.write((char*)linebuf,w*3);
      }

    delete[] linebuf;
  }



  // --- high-level PPM-writer class

  FileWriter_PPM::FileWriter_PPM()
    : d_yuv_as_grey(false),
      d_rgb_as_grey(false),
      d_grey_as_rgb(false)
  {
  }


  void FileWriter_PPM::Write(const Image<Pixel>& img,const char* filename)
  {
    ofstream ostr;
    ostr.open(filename, std::ios::out | std::ios::binary);
    Write(img,ostr);
  }


  void FileWriter_PPM::Write(const Image<Pixel>& img,ostream& ostr)
  {
    ImageParam param = img.AskParam();

    if (param.colorspace == Colorspace_RGB)
      {
	if (d_rgb_as_grey)
	  WritePPM5(img.AskBitmapG(),ostr);
	else
	  WritePPM6(img.AskBitmapR(),img.AskBitmapG(),img.AskBitmapB(),ostr);
      }
    else if (param.colorspace == Colorspace_Greyscale)
      {
	if (d_grey_as_rgb)
	  WritePPM6(img.AskBitmapY(),img.AskBitmapY(),img.AskBitmapY(),ostr);
	else
	  WritePPM5(img.AskBitmapY(),ostr);
      }
    else if (param.colorspace == Colorspace_YUV)
      {
	if (d_yuv_as_grey)
	  WritePPM5(img.AskBitmapY(),ostr);
	else
	  {
	    AssertDescr(false,"cannot save YUV as PPM in RGB colorspace");
	  }
      }
    else
      {
	AssertDescr(false,"cannot save this colorspace as PPM file");
      }
  }


  // --- convenience function for PPM writing

  void WriteImage_PPM(const Image<Pixel>& img,ostream& stream)
  {
    FileWriter_PPM writer;
    writer.Write(img,stream);
  }


  void WriteImage_PPM(const Image<Pixel>& img,const char* filename)
  {
    FileWriter_PPM writer;
    writer.WriteYUVAsGreyscale(true);
    writer.Write(img,filename); 
  }



  // --- PPM reading functions

  static bool is_whiteline(const char* b)
  {
    for (int i=0; b[i]; i++)
      if (!isspace(b[i]))
	return false;

    return true;
  }


  void ReadImage_PPM(Image<Pixel>& dest,const char* filename)
  {
    ifstream istr(filename);
    ReadImage_PPM(dest,istr);
  }


  void ReadImage_PPM(Image<Pixel>& dest,istream& stream)
  {
    char buffer[100+1];
    stream.getline(buffer,100);

    if (strlen(buffer)!=2 || buffer[0]!='P')
      { throw Excpt_Text(ErrSev_Error,"input is not a PPM format file"); }

    bool greyscale;
    if (buffer[1]=='5')
      greyscale=true;
    else if (buffer[1]=='6')
      greyscale=false;
    else
      { throw Excpt_Text(ErrSev_Error,"input is not a type 5 or type 6 PPM format file"); }


    int width,height,maxval;

    do
      {
	stream.getline(buffer,100);
      } while(buffer[0] == '#' || is_whiteline(buffer));
    sscanf(buffer,"%d %d",&width,&height);
    do
      {
	stream.getline(buffer,100);
      } while(buffer[0] == '#' || is_whiteline(buffer));
    maxval=atoi(buffer);

    if (maxval != 255)
      { throw Excpt_Text(ErrSev_Error,"cannot read PPM file with maximum pixel-value != 255"); }


    ImageParam param = dest.AskParam();
    param.width  = width;
    param.height = height;

    if (greyscale)
      {
	param.colorspace = Colorspace_Greyscale;
	dest.Create(param);

	Pixel*const* Y = dest.AskFrameY();

	for (int y=0;y<height;y++)
	  stream.read((char*)Y[y],width);
      }
    else
      {
	param.colorspace = Colorspace_RGB;
	dest.Create(param);

	uint8* linebuf = new uint8[width * 3];

	Pixel*const* r = dest.AskFrameR();
	Pixel*const* g = dest.AskFrameG();
	Pixel*const* b = dest.AskFrameB();

	for (int y=0;y<height;y++)
	  {
	    stream.read((char*)linebuf,width*3);

	    uint8* p = linebuf;
	    uint8* rp = r[y];
	    uint8* gp = g[y];
	    uint8* bp = b[y];

	    for (int x=0;x<width;x++)
	      {
		*rp++ = *p++;
		*gp++ = *p++;
		*bp++ = *p++;
	      }
	  }

	delete[] linebuf;
      }
  }

}

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

  void WriteImage_PPM(ostream& stream, const Image<Pixel>& img)
  {
    FileWriter_PPM writer;
    writer.Write(img,stream);
  }


  void WriteImage_PPM(const char* filename, const Image<Pixel>& img)
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

    if (maxval > 255)
      { throw Excpt_Text(ErrSev_Error,"cannot read PPM file with maximum pixel-value > 255"); }


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

  static void swapbytes(uint16* linebuf, int n_shorts)
  {
    uint8* p8 = (uint8*)linebuf;
    for (int i=0;i<n_shorts;i++)
      {
	swap(p8[0],p8[1]);
	p8+=2;
      }
  }

  int ReadImage_PPM(Image<uint16>& dest,std::istream& stream)
  {
    char buffer[100+1];
    stream.getline(buffer,100);

    if (strlen(buffer)!=2 || buffer[0]!='P')
      { throw Excpt_Text(ErrSev_Error,"input is not a PPM format file"); }

    bool greyscale;
    bool ascii=false;
    if (buffer[1]=='5')
      { greyscale=true; ascii=false; }
    else if (buffer[1]=='6')
      { greyscale=false; ascii=false; }
    else if (buffer[1]=='2')
      { greyscale=true; ascii=true; }
    else if (buffer[1]=='3')
      { greyscale=false; ascii=true; }
    else
      { throw Excpt_Text(ErrSev_Error,"input is not a type 2,3,5 or 6 PPM format file"); }


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

    if (maxval > 65535)
      { throw Excpt_Text(ErrSev_Error,"cannot read PPM file with maximum pixel-value > 65535"); }

    int nbytes_per_pixel=1;
    if (maxval>255) nbytes_per_pixel=2;

    ImageParam param = dest.AskParam();
    param.width  = width;
    param.height = height;


    if (ascii)
      {
	if (greyscale)
	  {
	    param.colorspace = Colorspace_Greyscale;
	    dest.Create(param);

	    uint16*const* Y = dest.AskFrameY();

	    for (int y=0;y<height;y++)
	      for (int x=0;x<width;x++)
		{
		  stream >> Y[y][x];
		}
	  }
	else
	  {
	    param.colorspace = Colorspace_RGB;
	    dest.Create(param);

	    uint16*const* r = dest.AskFrameR();
	    uint16*const* g = dest.AskFrameG();
	    uint16*const* b = dest.AskFrameB();

	    for (int y=0;y<height;y++)
	      for (int x=0;x<width;x++)
		{
		  stream >> r[y][x] >> g[y][x] >> b[y][x];
		}
	  }
      }
    else if (nbytes_per_pixel==2)
      {
	if (greyscale)
	  {
	    param.colorspace = Colorspace_Greyscale;
	    dest.Create(param);

	    uint16*const* Y = dest.AskFrameY();

	    for (int y=0;y<height;y++)
	      {
		stream.read((char*)Y[y],width*2);
#if !WORDS_BIGENDIAN
		swapbytes(Y[y], width);
#endif
	      }
	  }
	else
	  {
	    param.colorspace = Colorspace_RGB;
	    dest.Create(param);

	    uint16* linebuf = new uint16[width * 3];

	    uint16*const* r = dest.AskFrameR();
	    uint16*const* g = dest.AskFrameG();
	    uint16*const* b = dest.AskFrameB();

	    for (int y=0;y<height;y++)
	      {
		stream.read((char*)linebuf,width*2*3);
#if !WORDS_BIGENDIAN
		swapbytes(linebuf, width*3);
#endif

		uint16* p = linebuf;
		uint16* rp = r[y];
		uint16* gp = g[y];
		uint16* bp = b[y];

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
    else // nbytes_per_pixel==1
      {
	if (greyscale)
	  {
	    param.colorspace = Colorspace_Greyscale;
	    dest.Create(param);

	    uint16*const* Y = dest.AskFrameY();

	    uint8* linebuf = new uint8[width];

	    for (int y=0;y<height;y++)
	      {
		uint8* p = linebuf;
		uint16* yp = Y[y];

		stream.read((char*)linebuf,width);

		for (int x=0;x<width;x++)
		  {
		    *yp++ = *p++;
		  }
	      }

	    delete[] linebuf;
	  }
	else
	  {
	    param.colorspace = Colorspace_RGB;
	    dest.Create(param);

	    uint8* linebuf = new uint8[width * 3];

	    uint16*const* r = dest.AskFrameR();
	    uint16*const* g = dest.AskFrameG();
	    uint16*const* b = dest.AskFrameB();

	    for (int y=0;y<height;y++)
	      {
		stream.read((char*)linebuf,width*3);

		uint8* p = linebuf;
		uint16* rp = r[y];
		uint16* gp = g[y];
		uint16* bp = b[y];

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

    return maxval;
  }

  int ReadImage_PPM(Image<uint16>& dest,const char* filename)
  {
    ifstream istr(filename);
    return ReadImage_PPM(dest,istr);
  }



  void WriteImage_PPM(std::ostream& ostr, const Image<uint16>& img, int maxVal)
  {
    ImageParam param = img.AskParam();

    bool isColor = (param.colorspace == Colorspace_RGB);
    ostr << "P" << (isColor ? "6" : "5") << "\n"
	 << param.width << " " << param.height << "\n"
	 << maxVal << "\n";

    int w = img.getWidth();
    int h = img.getHeight();

    if (maxVal>=256)
      {
	uint16* linebuf = NULL;
	if (isColor)
	  {
	    linebuf = new uint16[w*3];

	    for (int y=0;y<h;y++)
	      {
		uint16* l = linebuf;
		for (int x=0;x<w;x++)
		  {
		    *l++ = img.AskFrameR()[y][x];
		    *l++ = img.AskFrameG()[y][x];
		    *l++ = img.AskFrameB()[y][x];
		  }

#if !WORDS_BIGENDIAN
		swapbytes(linebuf, 3*w);
#endif

		ostr.write((const char*)linebuf, 2*3*w);
	      }
	  }
	else
	  {
	    linebuf = new uint16[w];

	    for (int y=0;y<h;y++)
	      {
		memcpy(linebuf, img.AskFrameY()[y], w*2);
#if !WORDS_BIGENDIAN
		swapbytes(linebuf, w);
#endif

		ostr.write((const char*)linebuf, 2*w);
	      }
	  }

	delete[] linebuf;
      }
    else
      {
	uint8* linebuf = NULL;

	if (isColor)
	  {
	    linebuf = new uint8[w*3];

	    for (int y=0;y<h;y++)
	      {
		uint8* l = linebuf;
		for (int x=0;x<w;x++)
		  {
		    *l++ = img.AskFrameR()[y][x];
		    *l++ = img.AskFrameG()[y][x];
		    *l++ = img.AskFrameB()[y][x];
		  }

		ostr.write((const char*)linebuf, 3*w);
	      }
	  }
	else
	  {
	    linebuf = new uint8[w];

	    for (int y=0;y<h;y++)
	      {
		uint8* l = linebuf;
		for (int x=0;x<w;x++)
		  {
		    *l++ = img.AskFrameY()[y][x];
		  }

		ostr.write((const char*)linebuf, w);
	      }
	  }

	delete[] linebuf;
      }
  }

  void WriteImage_PPM(const char* filename, const Image<uint16>& img, int maxVal)
  {
    ofstream ostr;
    ostr.open(filename, std::ios::out | std::ios::binary);
    return WriteImage_PPM(ostr, img, maxVal);
  }
}

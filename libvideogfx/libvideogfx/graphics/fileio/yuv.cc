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

#include "libvideogfx/graphics/fileio/yuv.hh"


namespace videogfx {
  using namespace std;

  FileReader_YUV1::FileReader_YUV1()
    : d_yuvstr(NULL), d_alphastr(NULL),
      d_interleavedUV(false),
      d_greyscale_input(false),
      d_initialized(false)
  {
  }



  void FileReader_YUV1::Init()
  {
    if (d_initialized)
      return;


    // Get file length to calculate number of frames in file.

    assert(d_yuvstr);

    if (d_yuvstr==&cin)
      d_nFrames=999999;
    else
      {
	d_yuvstr->seekg(0,ios::end);
	long length = d_yuvstr->tellg();
	d_yuvstr->seekg(0,ios::beg);


	// Calculate the size of one frame.

	if (d_greyscale_input)
	  d_Framesize = d_spec.width * d_spec.height;
	else
	  {
	    switch (d_spec.chroma)
	      {
	      case Chroma_420:  d_Framesize = d_spec.width * d_spec.height *3/2; break;
	      case Chroma_422:  d_Framesize = d_spec.width * d_spec.height *2;   break;
	      case Chroma_444:  d_Framesize = d_spec.width * d_spec.height *3;   break;
	      default: assert(0); break;
	      }
	  }

	d_nFrames = length/d_Framesize;
	if (d_nFrames * d_Framesize != length)
	  {
	    cerr << "Input file has strange file size, continuing anyway.\n";
	    // TOOD: Put exceptionhandling here.
	  }

	//cout << d_nFrames << " frames\n";
      }

    d_nextFrame=0;
    d_initialized=true;
  }



  int FileReader_YUV1::AskNFrames() const
  {
    if (d_initialized)
      return d_nFrames;

    (const_cast<FileReader_YUV1*>(this))->Init();
    return d_nFrames;
  }



  bool FileReader_YUV1::IsEOF() const
  {
    (const_cast<FileReader_YUV1*>(this))->Init();
    if (d_yuvstr->eof())
      return true;

    return d_nextFrame >= d_nFrames;
  }



  void FileReader_YUV1::SkipToImage(int nr)
  {
    if (!d_initialized)
      Init();

    assert(nr>=0);
    assert(nr<d_nFrames);

    d_yuvstr->seekg(nr*d_Framesize,ios::beg);
    if (d_alphastr) d_alphastr->seekg(nr * d_spec.width * d_spec.height , ios::beg);
    d_nextFrame=nr;
  }



  void FileReader_YUV1::ReadImage(Image<Pixel>& img)
  {
    if (!d_initialized)
      Init();

    d_spec.has_alpha = (d_alphastr != NULL);
    d_spec.colorspace = Colorspace_YUV;

    img.Create(d_spec);

    Pixel*const* yp = img.AskFrameY();
    Pixel*const* up = (d_greyscale_input ? NULL : img.AskFrameU());
    Pixel*const* vp = (d_greyscale_input ? NULL : img.AskFrameV());

    // Y
    for (int y=0;y<d_spec.height;y++)
      d_yuvstr->read((char*)yp[y],d_spec.width);

    // color

    if (!d_greyscale_input)
      {
	int ch,cw;
	d_spec.AskChromaSizes(cw,ch);

	if (d_interleavedUV)
	  {
	    uint8* buf = new uint8[cw*2];
	  
	    for (int y=0;y<ch;y++)
	      {
		d_yuvstr->read((char*)buf,cw*2);
	      
		// demangle U,V components
	      
		for (int x=0;x<cw;x++)
		  {
		    up[y][x] = buf[x*2  ];
		    vp[y][x] = buf[x*2+1];
		  }
	      }
	
	    delete[] buf;
	  }
	else
	  {
	    // U
	    for (int y=0;y<ch;y++)
	      d_yuvstr->read((char*)up[y],cw);
	  
	    // V
	    for (int y=0;y<ch;y++)
	      d_yuvstr->read((char*)vp[y],cw);
	  }
      }

    // Alpha mask

    if (d_alphastr)
      {
	assert(!img.AskBitmap(Bitmap_Alpha).IsEmpty());
	Pixel*const* aa = img.AskFrameA();

	for (int y=0;y<d_spec.height;y++)
	  d_alphastr->read((char*)aa[y],d_spec.width);
      }

    d_nextFrame++;
  }



  FileWriter_YUV1::FileWriter_YUV1()
    : d_yuvstr(NULL),
      d_alphastr(NULL),
      d_write_greyscale_as_color(false),
      d_write_color_as_greyscale(false),
      d_write_interleaved(false)
  {
  }


  void FileWriter_YUV1::WriteImage(const Image<Pixel>& img)
  {
    ImageParam param = img.AskParam();

    bool input_is_greyscale = (param.colorspace == Colorspace_Greyscale);

    const Pixel*const* Y = img.AskFrameY();
    const Pixel*const* U = (input_is_greyscale ? NULL : img.AskFrameU());
    const Pixel*const* V = (input_is_greyscale ? NULL : img.AskFrameV());

    // write Y

    for (int y=0;y<param.height;y++)
      d_yuvstr->write((char*)Y[y],param.width);

    // write chrominance

    int cw,ch;
    param.AskChromaSizes(cw,ch);

    if (d_write_greyscale_as_color && input_is_greyscale)
      {
	uint8* buf;
	buf = new uint8[2*cw];

	for (int i=0;i<2*cw;i++)
	  buf[i]=128;

	for (int y=0;y<ch;y++)
	  d_yuvstr->write((char*)buf,2*cw);

	delete[] buf;
      }
    else if (input_is_greyscale || (!input_is_greyscale && d_write_color_as_greyscale))
      {
	// write no color information
      }
    else
      {
	assert(!input_is_greyscale);
	assert(!d_write_color_as_greyscale);

	if (d_write_interleaved)
	  {
	    uint8* buf;
	    buf = new uint8[2*cw];
	  
	    for (int y=0;y<ch;y++)
	      {
		for (int i=0;i<cw;i++)
		  {
		    buf[2*i  ] = U[y][i];
		    buf[2*i+1] = V[y][i];
		  }
	      
		d_yuvstr->write((char*)buf,2*cw);
	      }

	    delete[] buf;
	  }
	else
	  {
	    for (int y=0;y<ch;y++)
	      d_yuvstr->write((char*)U[y],cw);
	    for (int y=0;y<ch;y++)
	      d_yuvstr->write((char*)V[y],cw);
	  }
      }

    // alpha

    if (d_alphastr && param.has_alpha)
      {
	const Pixel*const* A = img.AskFrameA();

	for (int y=0;y<param.height;y++)
	  d_alphastr->write((char*)A[y],param.width);
      }
  }

}

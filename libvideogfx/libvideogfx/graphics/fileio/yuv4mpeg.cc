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

#include "libvideogfx/graphics/fileio/yuv4mpeg.hh"

#define Y4M_MAGIC "YUV4MPEG2"
#define Y4M_FRAME_MAGIC "FRAME"
#define Y4M_DELIM " "


namespace videogfx {
  using namespace std;


  FileReader_YUV4MPEG::FileReader_YUV4MPEG()
    : d_yuvstr(NULL),
      d_initialized(false)
  {
  }



  void FileReader_YUV4MPEG::Init()
  {
    Assert(d_yuvstr);

    if (d_initialized)
      return;

    d_yuvstr->seekg(0,ios::end);
    d_length = d_yuvstr->tellg();
    d_yuvstr->seekg(0,ios::beg);

    char buf[512];
    d_yuvstr->getline(buf,511);

    /* look for keyword in header */
    if (strncmp(buf, Y4M_MAGIC, strlen(Y4M_MAGIC)))
      throw Excpt_Text(ErrSev_Error,"input is not a YUV4MPEG2 file");

    d_spec.width = 0;
    d_spec.height = 0;
    d_spec.colorspace = Colorspace_YUV;
    d_spec.chroma = Chroma_420;


    char *token, *value;
    char tag;

    /* parse fields */
    for (token = strtok(buf, Y4M_DELIM); 
	 token != NULL; 
	 token = strtok(NULL, Y4M_DELIM)) {
      if (token[0] == '\0') continue;   /* skip empty strings */
      tag = token[0];
      value = token + 1;
      switch (tag) {
      case 'W':  /* width */
	d_spec.width = atoi(value);
	if (d_spec.width <= 0) throw Excpt_Text(ErrSev_Error,"input width must be positive");
	break;
      case 'H':  /* height */
	d_spec.height = atoi(value); 
	if (d_spec.height <= 0) throw Excpt_Text(ErrSev_Error,"input height must be positive");
	break;
      case 'F':  /* frame rate (fps) */
#if 0
	if ((err = y4m_parse_ratio(&(i->framerate), value)) != Y4M_OK)
	  return err;
	if (i->framerate.n < 0) return Y4M_ERR_RANGE;
#endif
	break;
      case 'I':  /* interlacing */
#if 0
	switch (value[0]) {
	case 'p':  i->interlace = Y4M_ILACE_NONE; break;
	case 't':  i->interlace = Y4M_ILACE_TOP_FIRST; break;
	case 'b':  i->interlace = Y4M_ILACE_BOTTOM_FIRST; break;
	case '?':
	default:
	  i->interlace = Y4M_UNKNOWN; break;
	}
#endif
	break;
      case 'A':  /* sample (pixel) aspect ratio */
#if 0
	if ((err = y4m_parse_ratio(&(i->sampleaspect), value)) != Y4M_OK)
	  return err;
	if (i->sampleaspect.n < 0) return Y4M_ERR_RANGE;
#endif
	break;
      case 'X':  /* 'X' meta-tag */
#if 0
	if ((err = y4m_xtag_add(&(i->x_tags), token)) != Y4M_OK) return err;
#endif
	break;
      default:
#if 0
	/* possible error on unknown options */
	if (_y4mparam_allow_unknown_tags) {
	  /* unknown tags ok:  store in xtag list and warn... */
	  if ((err = y4m_xtag_add(&(i->x_tags), token)) != Y4M_OK) return err;
	  mjpeg_warn("Unknown stream tag encountered:  '%s'", token);
	} else {
	  /* unknown tags are *not* ok */
	  return Y4M_ERR_BADTAG;
	}
#endif
	break;
      }
    }

    d_nextFrame=0;
    d_initialized=true;
  }



  bool FileReader_YUV4MPEG::IsEOF() const
  {
    (const_cast<FileReader_YUV4MPEG*>(this))->Init();

    if (d_yuvstr->eof())
      return true;

    if (d_yuvstr->tellg() >= d_length)
      return true;

    return false;
  }



  void FileReader_YUV4MPEG::SkipToImage(int nr)
  {
    if (nr < d_nextFrame)
      {
	d_yuvstr->seekg(0,ios::beg);
	d_initialized=false;
	Init();
      }

    Image<Pixel> dummy;
    while (nr > d_nextFrame)
      {
	ReadImage(dummy);
      }
  }



  void FileReader_YUV4MPEG::ReadImage(Image<Pixel>& img)
  {
    if (!d_initialized)
      Init();

    Assert(d_yuvstr);

    char buf[512];
    d_yuvstr->getline(buf,511);

    if (strncmp(buf, Y4M_FRAME_MAGIC, strlen(Y4M_FRAME_MAGIC)))
      throw Excpt_Text(ErrSev_Error,"no more frames in input stream");

    img.Create(d_spec);

    Pixel*const* yp = img.AskFrameY();
    Pixel*const* up = img.AskFrameU();
    Pixel*const* vp = img.AskFrameV();

    // Y

    for (int y=0;y<d_spec.height;y++)
      d_yuvstr->read((char*)yp[y],d_spec.width);

    // color

    int ch,cw;
    d_spec.AskChromaSizes(cw,ch);

    // U
    for (int y=0;y<ch;y++)
      d_yuvstr->read((char*)up[y],cw);
	  
    // V
    for (int y=0;y<ch;y++)
      d_yuvstr->read((char*)vp[y],cw);

    d_nextFrame++;
  }


  FileWriter_YUV4MPEG::FileWriter_YUV4MPEG()
    : d_yuvstr(NULL),
      d_header_written(false)
  {
  }


  void FileWriter_YUV4MPEG::WriteImage(const Image<Pixel>& img)
  {
    Assert(d_yuvstr);

    ImageParam param = img.AskParam();

    if (!d_header_written)
      {
	(*d_yuvstr) << Y4M_MAGIC << " W" << param.width << " H" << param.height << "\n";
	d_header_written=true;
      }

    (*d_yuvstr) << Y4M_FRAME_MAGIC << endl;

    const Pixel*const* Y = img.AskFrameY();
    const Pixel*const* U = img.AskFrameU();
    const Pixel*const* V = img.AskFrameV();

    // write Y

    for (int y=0;y<param.height;y++)
      d_yuvstr->write((char*)Y[y],param.width);

    // write chrominance

    int cw,ch;
    param.AskChromaSizes(cw,ch);

    for (int y=0;y<ch;y++)
      d_yuvstr->write((char*)U[y],cw);

    for (int y=0;y<ch;y++)
      d_yuvstr->write((char*)V[y],cw);
  }

}

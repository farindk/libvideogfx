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

#include <libvideogfx/graphics/fileio/unified_writer.hh>
#include <libvideogfx/graphics/fileio/jpeg.hh>
#include <libvideogfx/graphics/fileio/ppm.hh>
#include <libvideogfx/graphics/fileio/yuv.hh>
#include <libvideogfx/graphics/fileio/png.hh>
#include <libvideogfx/graphics/fileio/uyvy.hh>
#include <libvideogfx/graphics/color/colorspace.hh>
#include <libvideogfx/graphics/visualize/regions.hh>
#include "libvideogfx/graphics/datatypes/primitives.hh"
#include "libvideogfx/graphics/draw/scale.hh"
#include "libvideogfx/graphics/draw/pixelops.hh"
#include "config.h"

#if HAVE_X11
#include <libvideogfx/x11/imgwin.hh>
#endif
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>

namespace videogfx {
  using namespace std;

  const WriterStageFactory* UnifiedImageWriter::s_plugins[MAX_WRITER_PLUGINS];
  int UnifiedImageWriter::s_nplugins;


  void WriterStage::AppendAtEnd(WriterStage* next_stage)
  {
    if (next) next->AppendAtEnd(next_stage);
    else next = next_stage;
  }

  void UnifiedImageWriter::RegisterPlugin(const WriterStageFactory* fact)
  {
    Assert(s_nplugins < MAX_WRITER_PLUGINS);
    s_plugins[s_nplugins++] = fact;
  }

  const char* UnifiedImageWriter::AskPluginName(int idx)
  {
    if (idx >= s_nplugins)
      return NULL;
    else
      return s_plugins[idx]->AskName();
  }

  bool UnifiedImageWriter::SetOutput(const char* output_specification)
  {
    char* speccopy = new char[strlen(output_specification)+1];
    strcpy(speccopy,output_specification);

    char* spec = ExpandMacros(speccopy);
    delete[] speccopy;

    if (d_writer_pipeline) delete d_writer_pipeline;
    d_writer_pipeline = NULL;

    while (*spec)
      {
	bool found_one = false;
	for (int i=0;i<s_nplugins;i++)
	  {
	    WriterStage* newpipe = s_plugins[i]->ParseSpec(&spec);
	    if (newpipe)
	      {
		if (d_writer_pipeline==NULL)
		  d_writer_pipeline = newpipe;
		else
		  d_writer_pipeline->AppendAtEnd(newpipe);

		found_one = true;
		break;
	      }
	  }

	if (!found_one)
	  {
	    if (d_writer_pipeline) delete d_writer_pipeline;
	    d_writer_pipeline=NULL;
	    delete[] spec;
	    return false;
	  }
      }

    delete[] spec;
    return true;
  }


  void UnifiedImageWriter::WriteImage(const Image<Pixel>& img)
  {
    AssertDescr(d_writer_pipeline, "no writer specified");
    d_writer_pipeline->WriteImage(img);
  }

  WriterStageFactory::WriterStageFactory() { UnifiedImageWriter::RegisterPlugin(this); }



  // ------------------------------------------------------------------------------

#define MAX_FILENAME_LEN 1000

  class WriterStage_SglPictures : public WriterStage
  {
  public:
    WriterStage_SglPictures() : d_next(0) { d_filename_template[0]=0; }

    enum FileFormat { Format_Undefined, Format_JPEG, Format_PNG, Format_PPM, Format_UYVY };

    void SetFilenameTemplate(const char* t) { Assert(strlen(t)<MAX_FILENAME_LEN); strcpy(d_filename_template,t); }
    void SetFormat(WriterStage_SglPictures::FileFormat f) { d_format=f; }

    void WriteImage(const Image<Pixel>& img)
    {
      GenerateFilename();

      Image<Pixel> conv_img;
      

      switch (d_format)
	{
	case WriterStage_SglPictures::Format_JPEG:
	  AssertDescr(JPEG_Supported(),"JPEG support not compiled in");
	  conv_img = ChangeColorspace_NoCopy(img, Colorspace_YUV, Chroma_420);
	  WriteImage_JPEG(d_filename,conv_img);
	  break;

	case WriterStage_SglPictures::Format_PNG:
	  AssertDescr(PNG_Supported(),"PNG support not compiled in");
	  conv_img = ChangeColorspace_NoCopy(img, Colorspace_RGB);
	  WriteImage_PNG (d_filename,conv_img);
	  break;

	case WriterStage_SglPictures::Format_PPM:
	  conv_img = ChangeColorspace_NoCopy(img, Colorspace_RGB);
	  WriteImage_PPM (d_filename,conv_img);
	  break;

	case WriterStage_SglPictures::Format_UYVY:
	  {
	    conv_img = ChangeColorspace_NoCopy(img, Colorspace_YUV, Chroma_422);

	    // TODO
	    Assert(conv_img.AskWidth()==704);
	    Assert(conv_img.AskHeight()==568);

	    ofstream ostr(d_filename);
	    WriteImage_UYVY (ostr, conv_img);
	  }
	  break;
	}

      d_next++;

      if (next) next->WriteImage(img);
    }

  private:
    int d_next;
    FileFormat d_format;

    char d_filename[MAX_FILENAME_LEN];
    char d_filename_template[MAX_FILENAME_LEN];

    void GenerateFilename() const
    {
      Assert(strlen(d_filename_template)<MAX_FILENAME_LEN);
      snprintf((char*)d_filename,MAX_FILENAME_LEN,d_filename_template,d_next);
    }
  };

  static class WriterStageFactory_SglPictures : public WriterStageFactory
  {
  public:
    WriterStage* ParseSpec(char** spec) const
    {
      WriterStage_SglPictures::FileFormat f = WriterStage_SglPictures::Format_Undefined;

      if (CheckSuffix(*spec, "jpeg") || CheckSuffix(*spec, "jpg"))
	f = WriterStage_SglPictures::Format_JPEG;
      else if (CheckSuffix(*spec, "pgm") || CheckSuffix(*spec, "ppm"))
	f = WriterStage_SglPictures::Format_PPM;
      else if (CheckSuffix(*spec, "png"))
	f = WriterStage_SglPictures::Format_PNG;
      else if (CheckSuffix(*spec, "uyvy"))
	f = WriterStage_SglPictures::Format_UYVY;

      if (f != WriterStage_SglPictures::Format_Undefined)
	{
	  WriterStage_SglPictures* pl = new WriterStage_SglPictures;
	  char* name = ExtractNextOption(*spec);
	  pl->SetFilenameTemplate(name);
	  pl->SetFormat(f);
	  delete[] name;
	  RemoveOption(*spec);

	  return pl;
	}
      else
	return NULL;
    }

    const char* Name() const { return "writer: picture sequence"; }

  } singleton_sglpictures;


  // ------------------------------------------------------------------------------

  class WriterStage_YUVStream : public WriterStage
  {
  public:
    WriterStage_YUVStream() { }

    void SetFilename(const char* t)
    {
      Assert(strlen(t)<MAX_FILENAME_LEN);
      strcpy(d_filename,t);
      // d_ostr.close();
      /* NOTE: strange C++ behaviour: if the stream is closed,
	 and opened thereafter, nothing is written into the file... */
      d_ostr.open(d_filename, std::ios::out | std::ios::binary);
      d_writer.SetYUVStream(d_ostr);
    }

    void SetAlphaFilename(const char* t)
    {
      Assert(strlen(t)<MAX_FILENAME_LEN);
      strcpy(d_alpha_filename,t);
      d_alpha_ostr.open(d_alpha_filename, std::ios::out | std::ios::binary);
      d_writer.SetAlphaStream(d_alpha_ostr);
    }

    void SetInterleavedChroma(bool flag) { d_writer.SetWriteInterleaved(flag); }

    void WriteImage(const Image<Pixel>& img)
    {
      Image<Pixel> conv_img;

      if (img.AskParam().colorspace != Colorspace_YUV)
	conv_img = ChangeColorspace_NoCopy(img, Colorspace_YUV, Chroma_420);
      else
	conv_img = img;

      ImageParam param = img.AskParam();
      d_writer.WriteImage(conv_img);

      if (next) next->WriteImage(img);
    }

  private:
    char d_filename[MAX_FILENAME_LEN];
    char d_alpha_filename[MAX_FILENAME_LEN];
    ofstream d_ostr;
    ofstream d_alpha_ostr;
    ImageWriter_YUV1 d_writer;
  };

  static class WriterStageFactory_YUVStream : public WriterStageFactory
  {
  public:
    WriterStage* ParseSpec(char** spec) const
    {
      if (CheckSuffix(*spec, "yuv"))
	{
	  WriterStage_YUVStream* wr = new WriterStage_YUVStream;
	  char* name = ExtractNextOption(*spec);
	  wr->SetFilename(name);
	  delete[] name;
	  RemoveOption(*spec);

	  wr->SetInterleavedChroma(false);

	  for (;;)
	    {
	      if (MatchOption(*spec, "interleaved"))
		{
		  RemoveOption(*spec);
		  wr->SetInterleavedChroma(true);
		}
	      else if (MatchOption(*spec, "alpha"))
		{
		  RemoveOption(*spec);
		  char* alphaname = ExtractNextOption(*spec);
		  wr->SetAlphaFilename(alphaname);
		  delete[] alphaname;
		  RemoveOption(*spec);
		}
	      else
		break;
	    }

	  return wr;
	}
      else
	return NULL;
    }

    const char* Name() const { return "writer: raw YUV stream"; }

  } singleton_yuvstream;

  // ------------------------------------------------------------------------------

#if HAVE_X11
  class WriterStage_X11Output : public WriterStage
  {
  public:
    WriterStage_X11Output() { d_width=d_height=-1; SetWindowName("output"); }

    void SetWindowName(const char* name)
    {
      Assert(strlen(name)<MAX_FILENAME_LEN);
      strcpy(d_winname,name);
    }

    void WriteImage(const Image<Pixel>& img)
    {
      if (img.AskWidth() != d_width || img.AskHeight() != d_height)
	{
	  d_win.Close();
	  d_width  = img.AskWidth();
	  d_height = img.AskHeight();
	  d_win.Create(d_width, d_height, d_winname);
	}

      Image<Pixel> conv_img;

      d_win.Display(img);

      if (next) next->WriteImage(img);
    }

  private:
    char d_winname[MAX_FILENAME_LEN];
    ImageWindow_Autorefresh_X11 d_win;

    int d_width, d_height;
  };

  static class WriterStageFactory_X11Output : public WriterStageFactory
  {
  public:
    WriterStage* ParseSpec(char** spec) const
    {
      if (MatchOption(*spec, "win"))
	{
	  WriterStage_X11Output* wr = new WriterStage_X11Output;
	  RemoveOption(*spec);

	  return wr;
	}
      else
	return NULL;
    }

    const char* Name() const { return "writer: X11 window"; }

  } singleton_x11output;
#endif


  // ------------------------------------------------------------------------------

  class WriterStage_AlphaToGrey : public WriterStage
  {
  public:
    WriterStage_AlphaToGrey() { }

    void WriteImage(const Image<Pixel>& img)
    {
      Image<Pixel> newimg;
      newimg.Create(img.AskWidth(), img.AskHeight(), Colorspace_Greyscale);
      newimg.ReplaceBitmap(Bitmap_Y, img.AskBitmapA());

      Assert(next);
      next->WriteImage(newimg);
    }
  };

  static class WriterStageFactory_AlphaToGrey : public WriterStageFactory
  {
  public:
    WriterStage* ParseSpec(char** spec) const
    {
      if (MatchOption(*spec, "alpha2grey"))
	{
	  WriterStage_AlphaToGrey* wr = new WriterStage_AlphaToGrey;
	  RemoveOption(*spec);

	  return wr;
	}
      else
	return NULL;
    }

    const char* Name() const { return "filter: alpha to greyscale"; }

  } singleton_alphatogrey;

  // ------------------------------------------------------------------------------

  class WriterStage_AlphaOverlay : public WriterStage
  {
  public:
    WriterStage_AlphaOverlay() { d_col=Color<Pixel>(255,0,0); }

    void SetColor(int r,int g,int b)
    {
      d_col=Color<Pixel>(r,g,b);
    }

    void WriteImage(const Image<Pixel>& img)
    {
      Image<Pixel> newimg;
      ChangeColorspace(newimg, img, Colorspace_RGB);
      OverlayAlphaMask(newimg.AskBitmapR(), img.AskBitmapA(), d_col.c[0]);
      OverlayAlphaMask(newimg.AskBitmapG(), img.AskBitmapA(), d_col.c[1]);
      OverlayAlphaMask(newimg.AskBitmapB(), img.AskBitmapA(), d_col.c[2]);

      Assert(next);
      next->WriteImage(newimg);
    }

  private:
    Color<Pixel> d_col;
  };

  static class WriterStageFactory_Overlay : public WriterStageFactory
  {
  public:
    WriterStage* ParseSpec(char** spec) const
    {
      if (MatchOption(*spec, "alphaoverlay"))
	{
	  WriterStage_AlphaOverlay* wr = new WriterStage_AlphaOverlay;
	  RemoveOption(*spec);

	  if (MatchOption(*spec, "color"))
	    {
	      RemoveOption(*spec);
	      int r = ExtractNextNumber(*spec); RemoveOption(*spec);
	      int g = ExtractNextNumber(*spec); RemoveOption(*spec);
	      int b = ExtractNextNumber(*spec); RemoveOption(*spec);

	      wr->SetColor(r,g,b);
	    }

	  return wr;
	}
      else
	return NULL;
    }

    const char* Name() const { return "filter: alpha overlay"; }

  } singleton_alphaoverlay;


  // ------------------------------------------------------------------------------



  class WriterStage_Resize : public WriterStage
  {
  public:
    WriterStage_Resize() { w=h=0; }

    void SetParam(int ww,int hh) { w=ww; h=hh; }

    void WriteImage(const Image<Pixel>& img)
    {
      Image<Pixel> tmp;

      ImageParam spec=img.AskParam();
      spec.width = w;
      spec.height = h;
      tmp.Create(spec);
      CopyScaled(tmp,0,0,w,h,img);

      Assert(next);
      next->WriteImage(tmp);
    }

  private:
    int w,h;
  };


  static class WriterStageFactory_Resize : public WriterStageFactory
  {
  public:
    WriterStage* ParseSpec(char** spec) const
    {
      if (MatchOption(*spec, "resize"))
	{
	  RemoveOption(*spec);
	  WriterStage_Resize* resize = new WriterStage_Resize;
	  int w = ExtractNextNumber(*spec); RemoveOption(*spec);
	  int h = ExtractNextNumber(*spec); RemoveOption(*spec);
	  resize->SetParam(w,h);
	  return resize;
	}
      else
	return NULL;
    }

    const char* Name() const { return "filter: resize image"; }

  } singleton_resize;


  // ------------------------------------------------------------------------------


  class WriterStage_Start : public WriterStage
  {
  public:
    WriterStage_Start() { d_delay=0; }

    void SetStartFrame(int s) { d_delay=s; }

    void WriteImage(const Image<Pixel>& img)
    {
      if (d_delay)
	{ d_delay--; }
      else
	next->WriteImage(img);
    }

  private:
    int d_delay;
  };


  static class WriterStageFactory_Start : public WriterStageFactory
  {
  public:
    WriterStage* ParseSpec(char** spec) const
    {
      if (MatchOption(*spec, "start"))
	{
	  RemoveOption(*spec);
	  WriterStage_Start* startf = new WriterStage_Start;
	  int f = ExtractNextNumber(*spec); RemoveOption(*spec);
	  startf->SetStartFrame(f);
	  return startf;
	}
      else
	return NULL;
    }

    const char* Name() const { return "filter: set start-frame"; }

  } singleton_startframe;



  // ------------------------------------------------------------------------------


  class WriterStage_Length : public WriterStage
  {
  public:
    WriterStage_Length() { d_len=INT_MAX; }

    void SetSeqLength(int l) { d_len=l; }

    void WriteImage(const Image<Pixel>& img)
    {
      if (d_len)
	{ next->WriteImage(img); d_len--; }
    }

  private:
    int d_len;
  };


  static class WriterStageFactory_Length : public WriterStageFactory
  {
  public:
    WriterStage* ParseSpec(char** spec) const
    {
      if (MatchOption(*spec, "length"))
	{
	  RemoveOption(*spec);
	  WriterStage_Length* len = new WriterStage_Length;
	  int f = ExtractNextNumber(*spec); RemoveOption(*spec);
	  len->SetSeqLength(f);
	  return len;
	}
      else
	return NULL;
    }

    const char* Name() const { return "filter: set sequence length"; }

  } singleton_seqlength;


  static class WriterStageFactory_Range : public WriterStageFactory
  {
  public:
    WriterStage* ParseSpec(char** spec) const
    {
      if (MatchOption(*spec, "range"))
	{
	  RemoveOption(*spec);
	  WriterStage_Length* len   = new WriterStage_Length;
	  WriterStage_Start*  start = new WriterStage_Start;
	  int s = ExtractNextNumber(*spec); RemoveOption(*spec);
	  int e = ExtractNextNumber(*spec); RemoveOption(*spec);
	  start->SetStartFrame(s);
	  len->SetSeqLength(e);
	  start->AppendAtEnd(len);
	  return start;
	}
      else
	return NULL;
    }

    const char* Name() const { return "filter: set sequence range"; }

  } singleton_seqrange;


  // ------------------------------------------------------------------------------


  class WriterStage_Decimate : public WriterStage
  {
  public:
    WriterStage_Decimate() { d_factor=1; d_cnt=0; }

    void SetFactor(int f) { d_factor=f; }

    void WriteImage(const Image<Pixel>& img)
    {
      if ((d_cnt % d_factor)==0)
	next->WriteImage(img);
    }

  private:
    int d_factor;
    int d_cnt;
  };


  static class WriterStageFactory_Decimate : public WriterStageFactory
  {
  public:
    WriterStage* ParseSpec(char** spec) const
    {
      if (MatchOption(*spec, "decimate"))
	{
	  RemoveOption(*spec);
	  WriterStage_Decimate* decim = new WriterStage_Decimate;
	  int f = ExtractNextNumber(*spec); RemoveOption(*spec);
	  decim->SetFactor(f);
	  return decim;
	}
      else
	return NULL;
    }

    const char* Name() const { return "filter: decimate frame-rate"; }

  } singleton_decimate;


  // ------------------------------------------------------------------------------


  class WriterStage_Duplicate : public WriterStage
  {
  public:
    WriterStage_Duplicate() { d_factor=1; }

    void SetFactor(int f) { d_factor=f; }

    void WriteImage(const Image<Pixel>& img)
    {
      for (int i=0;i<d_factor;i++)
	next->WriteImage(img);
    }

  private:
    int d_factor;
  };


  static class WriterStageFactory_Duplicate : public WriterStageFactory
  {
  public:
    WriterStage* ParseSpec(char** spec) const
    {
      if (MatchOption(*spec, "duplicate"))
	{
	  RemoveOption(*spec);
	  WriterStage_Duplicate* dupl = new WriterStage_Duplicate;
	  int f = ExtractNextNumber(*spec); RemoveOption(*spec);
	  dupl->SetFactor(f);
	  return dupl;
	}
      else
	return NULL;
    }

    const char* Name() const { return "filter: duplicate frames"; }

  } singleton_duplicate;

  // ------------------------------------------------------------------------------


  class WriterStage_ContrastBright : public WriterStage
  {
  public:
    WriterStage_ContrastBright() { d_bright=0.0; d_contrast=1.0; }

    void SetFactors(double b,double c) { d_bright=b; d_contrast=c; }

    void WriteImage(const Image<Pixel>& img)
    {
      Image<Pixel> tmp;
      ChangeColorspace(tmp,img, Colorspace_RGB);
      ContrastBrightness(tmp.AskBitmapR(), d_contrast, d_bright);
      ContrastBrightness(tmp.AskBitmapG(), d_contrast, d_bright);
      ContrastBrightness(tmp.AskBitmapB(), d_contrast, d_bright);
      next->WriteImage(tmp);
    }

  private:
    double d_bright, d_contrast;
  };


  static class WriterStageFactory_ContastBright : public WriterStageFactory
  {
  public:
    WriterStage* ParseSpec(char** spec) const
    {
      if (MatchOption(*spec, "contrastbright"))
	{
	  RemoveOption(*spec);
	  WriterStage_ContrastBright* dupl = new WriterStage_ContrastBright;
	  int c = ExtractNextNumber(*spec); RemoveOption(*spec);
	  int b = ExtractNextNumber(*spec); RemoveOption(*spec);
	  dupl->SetFactors(b, c/100.0);
	  return dupl;
	}
      else
	return NULL;
    }

    const char* Name() const { return "filter: change contrast & brightness"; }

  } singleton_contrastbright;


}

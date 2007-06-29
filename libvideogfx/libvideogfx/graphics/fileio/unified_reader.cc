/********************************************************************************
    LibVideoGfx - video processing library
    Copyright (C) 2004  Dirk Farin

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

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "libvideogfx/graphics/fileio/unified_reader.hh"
#include "libvideogfx/graphics/fileio/mplayer.hh"
#include "libvideogfx/graphics/fileio/png.hh"
#include "libvideogfx/graphics/fileio/ppm.hh"
#include "libvideogfx/graphics/fileio/uyvy.hh"
#include "libvideogfx/graphics/fileio/yuv.hh"
#include "libvideogfx/graphics/fileio/jpeg.hh"
#include "libvideogfx/graphics/color/colorspace.hh"
#include "libvideogfx/graphics/draw/draw.hh"
#include "libvideogfx/graphics/draw/scale.hh"
#include "libvideogfx/graphics/draw/format.hh"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <ctype.h>

namespace videogfx {
  using namespace std;

  const ReaderStageFactory* UnifiedImageReader::s_plugins[MAX_READER_PLUGINS];
  int UnifiedImageReader::s_nplugins;

  static const char* configuration_file = "%s/.libvideogfxrc";


  static char* ExtractNextMacroOption(const char* spec,char& nextc);
  static void RemoveMacroOption(char* spec);

  char* ExpandMacros(char* spec)
  {
    //cout << "----------- start (" << spec << ") -------------\n";

    char* result = NULL;

    while (*spec)
      {
	char  nextc;
	char* option = ExtractNextMacroOption(spec, nextc);
	RemoveMacroOption(spec);

	//cerr << "macro-option: " << option << endl;
	//cerr << "remains: " << spec << endl;
	//cerr << "delimiter: " << (nextc ? nextc : '0') << endl;

	if (option[0]=='=') // is a macro
	  {
#define MAX_RCPATH_LEN 500
	    char buf[MAX_RCPATH_LEN];
	    snprintf(buf,MAX_RCPATH_LEN,configuration_file,getenv("HOME"));

	    FILE* fh = fopen(buf,"r");
	    if (!fh)
	      {
		AssertDescr(0,"no macro definition file found in user home directory");
		return NULL;
	      }

	    char* macro = option;
	    char* line=NULL;
	    size_t n=0;

	    for(;;)
	      {
		getline(&line,&n,fh);
		line[strlen(line)-1]=0;
		if (feof(fh))
		  break;

		if (strncmp(macro,line,strlen(macro))==0)
		  {
		    char* replacement;

		    option = new char[strlen(line)-strlen(macro)];
		    strcpy(option,line+strlen(macro)+1);
		    break;
		  }
	      }

	    if (line) free(line);
	    delete[] macro;

	    fclose(fh);


	    // recursive expansion

	    char* recurs = ExpandMacros(option);
	    delete[] option;
	    option = recurs;
	  }


	// Last or expanded option is now in "option". Append to "result"

	//cout << "COMBINE ->\n";

	//cout << "result |" << (result ? result : "NULL") << endl;
	//cout << "option |" << option << endl;
	//cout << "spec   |" << spec << endl;
	//cout << "nextc  |" << nextc << endl;

	if (!result)
	  {
	    result = new char[1+strlen(option)+1];
	    if (*spec) { sprintf(result,"%s%c",option,nextc); }
	    else sprintf(result,"%s",option);
	    delete[] option;
	  }
	else
	  {
	    char* newresult = new char[strlen(result)+1+strlen(option)+1];
	    if (*spec) sprintf(newresult,"%s%s%c",result,option,nextc);
	    else sprintf(newresult,"%s%s",result,option);
	    delete[] option;
	    delete[] result;
	    result = newresult;
	  }

	//cout << ">>>>>> |" << (result ? result : "NULL") << endl;
	//cout << "END COMBINE\n";
	//cout << "result: " << result << endl;
      }

    //cout << "----------- quit (" << result << ") -------------\n";
    return result;
  }



  void ReaderStage::SetPrevious(ReaderStage* previous)
  {
    if (prev) prev->SetPrevious(previous);
    else prev = previous;
  }

  void UnifiedImageReader::RegisterPlugin(const ReaderStageFactory* fact)
  {
    Assert(s_nplugins < MAX_READER_PLUGINS);
    s_plugins[s_nplugins++] = fact;
  }

  const char* UnifiedImageReader::AskPluginName(int idx)
  {
    if (idx >= s_nplugins)
      return NULL;
    else
      return s_plugins[idx]->AskName();
  }


  bool UnifiedImageReader::SetInput(const char* input_specification)
  {
    char* speccopy = new char[strlen(input_specification)+1];
    strcpy(speccopy,input_specification);

    char* spec = ExpandMacros(speccopy);
    delete[] speccopy;

    if (d_loader_pipeline) delete d_loader_pipeline;
    d_loader_pipeline = NULL;
    d_framenr=0; // reading is starting from the beginning


    while (*spec)
      {
	bool found_one = false;
	for (int i=0;i<s_nplugins;i++)
	  {
	    ReaderStage* newpipe = s_plugins[i]->ParseSpec(&spec);
	    if (newpipe)
	      {
		newpipe->SetPrevious(d_loader_pipeline);
		d_loader_pipeline=newpipe;
		found_one = true;
		break;
	      }
	  }

	if (!found_one)
	  {
	    if (d_loader_pipeline) delete d_loader_pipeline;
	    d_loader_pipeline=NULL;
	    delete[] spec;
	    return false;
	  }
      }

    delete[] spec;
    return true;
  }


  ImageParam UnifiedImageReader::AskImageParam() const
  {
    Image<Pixel> img;
    const_cast<UnifiedImageReader*>(this)->PeekImage(img);
    return img.AskParam();
  }

  int  UnifiedImageReader::AskNFrames() const { Assert(d_loader_pipeline); return d_loader_pipeline->AskNFrames(); }
  bool UnifiedImageReader::IsEOF() const { Assert(d_loader_pipeline); return d_loader_pipeline->IsEOF(); }


  void UnifiedImageReader::SkipToImage(int nr)
  {
    Assert(d_loader_pipeline);
    d_framenr = nr;
    bool success = d_loader_pipeline->SkipToImage(nr);
    AssertDescr(success, "loader pipeline does not support skipping");
  }


  void UnifiedImageReader::ReadImage(Image<Pixel>& img)
  {
    AssertDescr(d_loader_pipeline, "no loader specified");

    d_framenr++;

    if (!d_preload.IsEmpty())
      {
	img = d_preload;
	d_preload.Release();
	return;
      }

    d_loader_pipeline->ReadImage(img);

    if (d_colorspace != Colorspace_Invalid || d_chroma != Chroma_Invalid)
      {
	Colorspace   colorspace = (d_colorspace == Colorspace_Invalid ? img.AskParam().colorspace : d_colorspace);
	ChromaFormat chroma     = (d_chroma     == Chroma_Invalid     ? img.AskParam().chroma     : d_chroma);

	Image<Pixel> tmp;
	ChangeColorspace(tmp,img, colorspace, chroma);
	img = tmp;
      }
  }


  void UnifiedImageReader::PeekImage(Image<Pixel>& img)
  {
    if (d_preload.IsEmpty())
      ReadImage(d_preload);

    img = d_preload;
  }


  int  UnifiedImageReader::AskWidth() const
  {
    if (width) return width;

    UnifiedImageReader* ncthis = const_cast<UnifiedImageReader*>(this);

    if (d_preload.IsEmpty())
      ncthis->ReadImage(ncthis->d_preload);

    ncthis->width = d_preload.AskWidth();
    return width;
  }


  int  UnifiedImageReader::AskHeight() const
  {
    if (height) return height;

    UnifiedImageReader* ncthis = const_cast<UnifiedImageReader*>(this);

    if (d_preload.IsEmpty())
      ncthis->ReadImage(ncthis->d_preload);

    ncthis->height = d_preload.AskHeight();
    return height;
  }



  ReaderStageFactory::ReaderStageFactory() { UnifiedImageReader::RegisterPlugin(this); }


  char* ExtractNextOption(const char* spec)
  {
    const char* p = index(spec,':');
    if (!p) p=spec+strlen(spec);
    int len = (p-spec);

    char* opt = new char[len+1];
    strncpy(opt,spec,len);
    opt[len]=0;

    return opt;
  }

  static char* ExtractNextMacroOption(const char* spec,char& nextc)
  {
    if (*spec != '=')
      { nextc=':'; return ExtractNextOption(spec); }

    const char* p = spec;

    while (*p && *p!='/' && *p != ':') p++;
    nextc = *p;
    if (!*p) p=0;

    if (!p) p=spec+strlen(spec);
    int len = (p-spec);

    char* opt = new char[len+1];
    strncpy(opt,spec,len);
    opt[len]=0;

    return opt;
  }

  int  ExtractNextNumber(const char* spec)
  {
    char* o = ExtractNextOption(spec);
    int num = atoi(o);
    delete[] o;
    return num;
  }

  bool MatchOption(const char* spec,const char* option)
  {
    const char* p = index(spec,':');
    if (!p) p=spec+strlen(spec);
    int len = (p-spec);

    if (strlen(option) != len) return false;
    return strncasecmp(option,spec,len)==0;
  }

  bool CheckSuffix(const char* spec,const char* suffix)
  {
    const char* p = index(spec,':');
    if (!p) p=spec+strlen(spec);
    int len = (p-spec);

    if (len<strlen(suffix)+2)
      return false;

    p -= strlen(suffix)+1;

    if (*p != '.') return false;
    p++;

    return strncasecmp(suffix,p,strlen(suffix))==0;
  }

  void RemoveOption(char* spec)
  {
    const char* p = index(spec,':');
    if (!p) { *spec=0; return; }

    int len = (p-spec);

    memmove(spec,spec+len+1,strlen(spec)-len+1);
  }

  static void RemoveMacroOption(char* spec)
  {
    if (*spec!='=')
      { RemoveOption(spec); return; }

    while (*spec != '/' && *spec && *spec !=':')
      memmove(spec,spec+1,strlen(spec));

    if (*spec == '/' || *spec == ':')
      memmove(spec,spec+1,strlen(spec));
  }

  bool ExtractSize(char* spec,int& w,int& h)
  {
    char* s = ExtractNextOption(spec);
    char* i = index(spec,'x');
    if (!i) { delete[] s; w=h=0; return false; }

    // check size format (must be '1234x1234' or '1234X1234')
    {
      char* n;
      n=s;
      do { if (!isdigit(*n)) { delete[] s; w=h=0; return false; } n++; } while(*n && (*n!='x' && *n!='X'));
      if (*n!='x' && *n!='X') { delete[] s; w=h=0; return false; }
      n++;
      do { if (!isdigit(*n)) { delete[] s; w=h=0; return false; } n++; } while(*n);
    }

    w = atoi(s);
    h = atoi(i+1);

    delete[] s;

    RemoveOption(spec);
    return true;
  }

  void ExtractSize(char* spec,int& w,int& h,int default_w,int default_h)
  {
    if (!ExtractSize(spec,w,h))
      {
	w = default_w;
	h = default_h;
      }
  }



  // ------------------------------------------------------------------------------


  class ReaderStage_RGB : public ReaderStage
  {
  public:
    ReaderStage_RGB() { id=0; }

    int  AskNFrames() const { return INT_MAX; }
    bool IsEOF() const { return false; }

    bool SkipToImage(int nr) { id = (nr%3); }
    void ReadImage(Image<Pixel>& img)
    {
      ImageParam spec(d_w,d_h);
      img.Create(spec);
      Clear(img,Color<Pixel>( (id==0) ? 255 : 0 , (id==1) ? 255 : 0 , (id==2) ? 255 : 0 ));
      id = (id+1)%3;
    }

    void SetSize(int w,int h) { d_w=w; d_h=h; }

  private:
    int id;
    int d_w,d_h;
  };


  static class ReaderStageFactory_RGB : public ReaderStageFactory
  {
  public:
    ReaderStage* ParseSpec(char** spec) const
    {
      if (MatchOption(*spec, "rgb"))
	{
	  RemoveOption(*spec);
	  int w,h;
	  ExtractSize(*spec,w,h,352,288);
	  
	  ReaderStage_RGB* pl = new ReaderStage_RGB;
	  pl->SetSize(w,h);
	  return pl;
	}
      else
	return NULL;
    }

    const char* Name() const { return "test pattern: cycle through RGB"; }

  } singleton_rgb;



  // ------------------------------------------------------------------------------



  class ReaderStage_MPlayer : public ReaderStage
  {
  public:
    int  AskNFrames() const { return INT_MAX; }
    bool IsEOF() const { return reader.IsEOF(); }

    bool SkipToImage(int nr) { reader.SkipToImage(nr); return true; } // only forward seek
    void ReadImage(Image<Pixel>& img) { reader.ReadImage(img); }

    void SetInput(const char* name) { reader.Open(name); }

  private:
    FileReader_MPlayer reader;
  };


  static class ReaderStageFactory_MPlayer : public ReaderStageFactory
  {
  public:
    ReaderStage* ParseSpec(char** spec) const
    {
      if (CheckSuffix(*spec, "vdr") ||
	  CheckSuffix(*spec, "avi") ||
	  CheckSuffix(*spec, "wmv") ||
	  CheckSuffix(*spec, "mpg") ||
	  CheckSuffix(*spec, "vob") ||
	  CheckSuffix(*spec, "m1v") ||
	  CheckSuffix(*spec, "m2v") ||
	  CheckSuffix(*spec, "mpeg") ||
	  CheckSuffix(*spec, "wmf"))
	{
	  ReaderStage_MPlayer* pl = new ReaderStage_MPlayer;
	  char* name = ExtractNextOption(*spec);
	  pl->SetInput(name);

	  delete[] name;
	  RemoveOption(*spec);

	  return pl;
	}
      else
	return NULL;
    }

    const char* Name() const { return "loader: mplayer pipe"; }

  } singleton_mplayer;



  // ------------------------------------------------------------------------------


#define MAX_FILENAME_LEN 1000

  class ReaderStage_SglPictures : public ReaderStage
  {
  public:
    ReaderStage_SglPictures() : d_next(0) { d_filename_template[0]=0; }

    enum FileFormat { Format_Undefined, Format_JPEG, Format_PNG, Format_PPM, Format_UYVY };

    void SetFilenameTemplate(const char* t) { Assert(strlen(t)<MAX_FILENAME_LEN); strcpy(d_filename_template,t); }
    void SetFormat(ReaderStage_SglPictures::FileFormat f) { d_format=f; }

    int  AskNFrames() const { return INT_MAX; }
    bool IsEOF() const
    {
      GenerateFilename();
      struct stat buf;
      if (stat(d_filename,&buf)!=0)
	return true;
      else
	return false;
    }

    bool SkipToImage(int nr) { d_next=nr; return true; } // only forward seek
    void ReadImage(Image<Pixel>& img)
    {
      GenerateFilename();

      switch (d_format)
	{
	case ReaderStage_SglPictures::Format_JPEG: ReadImage_JPEG(img, d_filename); break;
	case ReaderStage_SglPictures::Format_PNG:  ReadImage_PNG (img, d_filename); break;
	case ReaderStage_SglPictures::Format_PPM:  ReadImage_PPM (img, d_filename); break;
	case ReaderStage_SglPictures::Format_UYVY:
	  {
	    ifstream istr(d_filename);
	    ReadImage_UYVY (img, istr, 704,568); // TODO
	  }
	  break;
	}

      d_next++;
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


  static class ReaderStageFactory_SglPictures : public ReaderStageFactory
  {
  public:
    ReaderStage* ParseSpec(char** spec) const
    {
      ReaderStage_SglPictures::FileFormat f = ReaderStage_SglPictures::Format_Undefined;

      if (CheckSuffix(*spec, "jpeg") || CheckSuffix(*spec, "jpg"))
	f = ReaderStage_SglPictures::Format_JPEG;
      else if (CheckSuffix(*spec, "pgm") || CheckSuffix(*spec, "ppm"))
	f = ReaderStage_SglPictures::Format_PPM;
      else if (CheckSuffix(*spec, "png"))
	f = ReaderStage_SglPictures::Format_PNG;
      else if (CheckSuffix(*spec, "uyvy"))
	f = ReaderStage_SglPictures::Format_UYVY;

      if (f != ReaderStage_SglPictures::Format_Undefined)
	{
	  ReaderStage_SglPictures* pl = new ReaderStage_SglPictures;
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

    const char* Name() const { return "loader: picture sequence"; }

  } singleton_sglpictures;



  // ------------------------------------------------------------------------------



  class ReaderStage_YUV1 : public ReaderStage
  {
  public:
    ReaderStage_YUV1() { }

    void SetYUVParams(const char* filename, const ImageParam& s)
    {
      istr.open(filename);
      reader.SetYUVStream(istr);
      reader.SetImageParam(s);
      reader.SetInputIsGreyscale(s.colorspace == Colorspace_Greyscale);
      spec=s;
    }

    int  AskNFrames() const { return reader.AskNFrames(); }
    bool IsEOF() const { return reader.IsEOF(); }
    bool SkipToImage(int nr) { reader.SkipToImage(nr); return true; }
    void ReadImage(Image<Pixel>& img) { reader.ReadImage(img); }

  private:
    ifstream istr;
    ImageReader_YUV1 reader;
    ImageParam spec;
  };


  static bool ScanForSizeAbbrev(const char* str, ImageParam& spec)
  {
    for (int s=0;str[s];s++)
      {
	if (strncasecmp(&str[s],"qcif",4)==0) { spec.width = 176; spec.height = 144; return true; }
	if (strncasecmp(&str[s],"cif" ,3)==0) { spec.width = 352; spec.height = 288; return true; }
	if (strncasecmp(&str[s],"sif" ,3)==0) { spec.width = 352; spec.height = 240; return true; }
      }

    return false;
  }

  static bool ScanForSize(const char* str, ImageParam& spec)
  {
    for (int s=0;str[s];s++)
      {
	int w=0,h=0;
	int i=s;
	for (;;)
	  {
	    if (isdigit(str[i])) { w=w*10 + str[i]-'0'; i++; }
	    else break;
	  }
	if (w==0)
	  continue;

	if (str[i]=='x' || str[i]=='X')
	  i++;
	else
	  continue;

	for (;;)
	  {
	    if (isdigit(str[i])) { h=h*10 + str[i]-'0'; i++; }
	    else break;
	  }
	if (h==0)
	  continue;

	spec.width = w;
	spec.height = h;

	//cerr << "found size: " << w << "x" << h << endl;
	return true;
      }

    return false;
  }


  // Yannick Morvan: This routine expects c444 or c420 or c422 in char*str
  // I must say that not too much checking is made
  static ChromaFormat ScanForChroma(const char* str)
  {
    if (strstr(str, "c444")) return Chroma_444;
    if (strstr(str, "c422")) return Chroma_422;
    if (strstr(str, "c420")) return Chroma_420;

    return Chroma_420;
  }


  static class ReaderStageFactory_YUV1 : public ReaderStageFactory
  {
  public:
    ReaderStage* ParseSpec(char** spec) const
    {
      ReaderStage_SglPictures::FileFormat f = ReaderStage_SglPictures::Format_Undefined;

      if (CheckSuffix(*spec, "yuv") || CheckSuffix(*spec, "grey"))
	{
	  ReaderStage_YUV1* pl = new ReaderStage_YUV1;
	  char* name = ExtractNextOption(*spec);
	  ImageParam param;
	  bool greyscale = CheckSuffix(*spec, "grey");
	  param.colorspace= (greyscale ? Colorspace_Greyscale : Colorspace_YUV);
	  param.chroma=ScanForChroma(name);
	  param.width=352;
	  param.height=288;
	  if (!ScanForSize(name,param))
	    ScanForSizeAbbrev(name,param);
	  RemoveOption(*spec);

	  pl->SetYUVParams(name,param);
	  delete[] name;

	  return pl;
	}
      else
	return NULL;
    }

    const char* Name() const { return "loader: yuv file"; }

  } singleton_yuv1;



  // ------------------------------------------------------------------------------


  static void DownscaleBitmap2H(Bitmap<Pixel>& bm, const Bitmap<Pixel>& src)
  {
    int w=bm.AskWidth(), h=bm.AskHeight();
    const Pixel*const* sp = src.AskFrame();
    Pixel*const* dp = bm.AskFrame();

    for (int y=0;y<h;y++)
      {
	dp[y][0] = (sp[y][0]+sp[y][1])/2;

	for (int x=1;x<w-1;x++)
	  {
	    dp[y][x] = (sp[y][2*x-1] + 2*sp[y][2*x] + sp[y][2*x+1])/4;
	  }

	dp[y][w-1] = (sp[y][2*w-2]+sp[y][2*w-1])/2;
      }
  }

  class ReaderStage_Quarter : public ReaderStage
  {
  public:
    int  AskNFrames() const { Assert(prev); return prev->AskNFrames(); }
    bool IsEOF() const { Assert(prev); return prev->IsEOF(); }

    bool SkipToImage(int nr) { Assert(prev); return prev->SkipToImage(nr); }
    void ReadImage(Image<Pixel>& img)
    {
      Assert(prev);

      Image<Pixel> tmp;
      prev->ReadImage(tmp);

      Image<Pixel> deinter;
      deinter = tmp.CreateFieldView(true);

      ImageParam spec = deinter.AskParam();
      spec.width /= 2;
      img.Create(spec);

      //CopyScaled(img,0,0,deinter.AskWidth()/2, deinter.AskHeight(), deinter);

      for (int i=0;i<4;i++)
	if (!img.AskBitmap((BitmapChannel)i).IsEmpty())
	  {
	    DownscaleBitmap2H(img.AskBitmap((BitmapChannel)i), deinter.AskBitmap((BitmapChannel)i));
	  }
    }
  };


  static class ReaderStageFactory_Quarter : public ReaderStageFactory
  {
  public:
    ReaderStage* ParseSpec(char** spec) const
    {
      if (MatchOption(*spec, "quarter"))
	{
	  RemoveOption(*spec);
	  return new ReaderStage_Quarter;
	}
      else
	return NULL;
    }

    const char* Name() const { return "filter: quarter size"; }

  } singleton_quarter;



  // ------------------------------------------------------------------------------



  class ReaderStage_Crop : public ReaderStage
  {
  public:
    ReaderStage_Crop() { l=r=t=b=0; }

    void SetParam(int ll,int rr,int tt,int bb) { l=ll; r=rr; t=tt; b=bb; }

    int  AskNFrames() const { Assert(prev); return prev->AskNFrames(); }
    bool IsEOF() const { Assert(prev); return prev->IsEOF(); }

    bool SkipToImage(int nr) { Assert(prev); return prev->SkipToImage(nr); }
    void ReadImage(Image<Pixel>& img)
    {
      Assert(prev);

      Image<Pixel> tmp;
      prev->ReadImage(tmp);

      Crop(img, tmp, l,r,t,b);
    }

  private:
    int l,r,t,b;
  };


  static class ReaderStageFactory_Crop : public ReaderStageFactory
  {
  public:
    ReaderStage* ParseSpec(char** spec) const
    {
      if (MatchOption(*spec, "crop"))
	{
	  RemoveOption(*spec);
	  ReaderStage_Crop* crop = new ReaderStage_Crop;
	  int l = ExtractNextNumber(*spec); RemoveOption(*spec);
	  int r = ExtractNextNumber(*spec); RemoveOption(*spec);
	  int t = ExtractNextNumber(*spec); RemoveOption(*spec);
	  int b = ExtractNextNumber(*spec); RemoveOption(*spec);
	  crop->SetParam(l,r,t,b);
	  return crop;
	}
      else
	return NULL;
    }

    const char* Name() const { return "filter: crop image"; }

  } singleton_crop;



  // ------------------------------------------------------------------------------



  class ReaderStage_Resize : public ReaderStage
  {
  public:
    ReaderStage_Resize() { w=h=0; }

    void SetParam(int ww,int hh) { w=ww; h=hh; }

    int  AskNFrames() const { Assert(prev); return prev->AskNFrames(); }
    bool IsEOF() const { Assert(prev); return prev->IsEOF(); }

    bool SkipToImage(int nr) { Assert(prev); return prev->SkipToImage(nr); }
    void ReadImage(Image<Pixel>& img)
    {
      Assert(prev);

      Image<Pixel> tmp;
      prev->ReadImage(tmp);

      ImageParam spec=tmp.AskParam();
      spec.width = w;
      spec.height = h;
      img.Create(spec);
      CopyScaled(img,0,0,w,h,tmp);
    }

  private:
    int w,h;
  };


  static class ReaderStageFactory_Resize : public ReaderStageFactory
  {
  public:
    ReaderStage* ParseSpec(char** spec) const
    {
      if (MatchOption(*spec, "resize"))
	{
	  RemoveOption(*spec);
	  ReaderStage_Resize* resize = new ReaderStage_Resize;
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


  class ReaderStage_SeekCache : public ReaderStage
  {
  public:
    ReaderStage_SeekCache() : d_initialized(false) { }
    ~ReaderStage_SeekCache() { unlink("cache.yuv"); }

    int  AskNFrames() const
    {
      const_cast<ReaderStage_SeekCache*>(this)->Initialize();
      return d_reader.AskNFrames();
    }

    bool IsEOF() const
    {
      const_cast<ReaderStage_SeekCache*>(this)->Initialize();
      return d_reader.IsEOF();
    }

    bool SkipToImage(int nr)
    {
      Initialize();
      d_reader.SkipToImage(nr);
      return true;
    }

    void ReadImage(Image<Pixel>& img)
    {
      Initialize();
      d_reader.ReadImage(img);
    }

  private:
    void Initialize()
    {
      if (d_initialized)
	return;

      ImageParam spec;

      {
	// fill cache

	ofstream ostr("cache.yuv");
	d_writer.SetYUVStream(ostr);

	Image<Pixel> img;

	while (!prev->IsEOF())
	  {
	    prev->ReadImage(img);

	    Image<Pixel> yuvimg;
	    ChangeColorspace(yuvimg,img, Colorspace_YUV);
	    d_writer.WriteImage(yuvimg);

	    spec = yuvimg.AskParam();
	  }

	spec.colorspace= Colorspace_RGB;
      }

      // prepare reader

      d_istr.open("cache.yuv");
      d_reader.SetYUVStream(d_istr);
      d_reader.SetImageParam(spec);

      d_initialized=true;
    }

    bool d_initialized;

    ImageParam d_spec;

    ifstream d_istr;
    ImageReader_YUV1 d_reader;
    ImageWriter_YUV1 d_writer;
  };


  static class ReaderStageFactory_SeekCache : public ReaderStageFactory
  {
  public:
    ReaderStage* ParseSpec(char** spec) const
    {
      if (MatchOption(*spec, "cache"))
	{
	  RemoveOption(*spec);
	  ReaderStage_SeekCache* cache = new ReaderStage_SeekCache;
	  //int f = ExtractNextNumber(*spec); RemoveOption(*spec);
	  //decim->SetFactor(f);
	  return cache;
	}
      else
	return NULL;
    }

    const char* Name() const { return "seek cache"; }

  } singleton_cache;



  // ------------------------------------------------------------------------------


  class ReaderStage_Decimate : public ReaderStage
  {
  public:
    ReaderStage_Decimate() { d_factor=1; }

    void SetFactor(int f) { d_factor=f; }

    int  AskNFrames() const { Assert(prev); return prev->AskNFrames()/d_factor; }
    bool IsEOF() const { Assert(prev); return prev->IsEOF(); }

    bool SkipToImage(int nr) { Assert(prev); return prev->SkipToImage(nr*d_factor); }
    void ReadImage(Image<Pixel>& img)
    {
      for (int i=0;i<d_factor;i++)
	prev->ReadImage(img);
    }

  private:
    int d_factor;
  };


  static class ReaderStageFactory_Decimate : public ReaderStageFactory
  {
  public:
    ReaderStage* ParseSpec(char** spec) const
    {
      if (MatchOption(*spec, "decimate"))
	{
	  RemoveOption(*spec);
	  ReaderStage_Decimate* decim = new ReaderStage_Decimate;
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


  class ReaderStage_Start : public ReaderStage
  {
  public:
    ReaderStage_Start() { d_start=0; d_startupread=0; }

    void SetStartFrame(int s) { d_start=s; d_startupread=s; }

    int  AskNFrames() const { Assert(prev); return prev->AskNFrames()-d_start; }
    bool IsEOF() const { Assert(prev); return prev->IsEOF(); }

    bool SkipToImage(int nr) { Assert(prev); d_startupread=0; return prev->SkipToImage(nr+d_start); }
    void ReadImage(Image<Pixel>& img)
    {
      while (d_startupread)
	{
	  prev->ReadImage(img);
	  d_startupread--;
	}

      prev->ReadImage(img);
    }

  private:
    int d_startupread;
    int d_start;
  };


  static class ReaderStageFactory_Start : public ReaderStageFactory
  {
  public:
    ReaderStage* ParseSpec(char** spec) const
    {
      if (MatchOption(*spec, "start"))
	{
	  RemoveOption(*spec);
	  ReaderStage_Start* startf = new ReaderStage_Start;
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


  class ReaderStage_Length : public ReaderStage
  {
  public:
    ReaderStage_Length() { d_curr=0; d_len=INT_MAX; }

    void SetSeqLength(int l) { d_len=l; }

    int  AskNFrames() const { Assert(prev); return std::min(prev->AskNFrames(),d_len); }
    bool IsEOF() const { Assert(prev); if (d_curr>=d_len) return true; else return prev->IsEOF(); }

    bool SkipToImage(int nr) { Assert(prev); d_curr=nr; return prev->SkipToImage(nr); }
    void ReadImage(Image<Pixel>& img)
    {
      prev->ReadImage(img);
      d_curr++;
    }

  private:
    int d_len;
    int d_curr;
  };


  static class ReaderStageFactory_Length : public ReaderStageFactory
  {
  public:
    ReaderStage* ParseSpec(char** spec) const
    {
      if (MatchOption(*spec, "length"))
	{
	  RemoveOption(*spec);
	  ReaderStage_Length* len = new ReaderStage_Length;
	  int f = ExtractNextNumber(*spec); RemoveOption(*spec);
	  len->SetSeqLength(f);
	  return len;
	}
      else
	return NULL;
    }

    const char* Name() const { return "filter: set sequence length"; }

  } singleton_seqlength;


  static class ReaderStageFactory_Range : public ReaderStageFactory
  {
  public:
    ReaderStage* ParseSpec(char** spec) const
    {
      if (MatchOption(*spec, "range"))
	{
	  RemoveOption(*spec);
	  ReaderStage_Length* len   = new ReaderStage_Length;
	  ReaderStage_Start*  start = new ReaderStage_Start;
	  int s = ExtractNextNumber(*spec); RemoveOption(*spec);
	  int e = ExtractNextNumber(*spec); RemoveOption(*spec);
	  start->SetStartFrame(s);
	  len->SetSeqLength(e);
	  len->SetPrevious(start);
	  return len;
	}
      else
	return NULL;
    }

    const char* Name() const { return "filter: set sequence range"; }

  } singleton_seqrange;


  // ------------------------------------------------------------------------------


  class ReaderStage_Alpha : public ReaderStage
  {
  public:
    ReaderStage_Alpha() { d_framenr=0; fh=NULL; }
    ~ReaderStage_Alpha() { if (fh) fclose(fh); }

    void SetAlphaStream(const char* name) { fh=fopen(name,"rb"); }

    int  AskNFrames() const { Assert(prev); return prev->AskNFrames(); }
    bool IsEOF() const { Assert(prev); return prev->IsEOF(); }

    bool SkipToImage(int nr) { Assert(prev); d_framenr=nr; return prev->SkipToImage(nr); }
    void ReadImage(Image<Pixel>& img)
    {
      // read image as usual

      prev->ReadImage(img);
      d_framenr++;


      // seek to position of current alpha mask

      int w=img.AskWidth(), h=img.AskHeight();
      int size = w*h;

      fseek(fh, size*d_framenr, SEEK_SET);


      // load alpha mask

      Bitmap<Pixel> alpha;
      alpha.Create(w,h);

      for (int y=0;y<h;y++)
	{
	  fread(alpha.AskFrame()[y], w,1, fh);
	}


      // insert alpha mask into image

      img.ReplaceBitmap(Bitmap_Alpha, alpha);
      ImageParam spec;
      spec = img.AskParam();
      spec.has_alpha = true;
      img.SetParam(spec);
    }

  private:
    FILE* fh;
    int d_framenr;
  };


  static class ReaderStageFactory_Alpha : public ReaderStageFactory
  {
  public:
    ReaderStage* ParseSpec(char** spec) const
    {
      if (MatchOption(*spec, "alpha"))
	{
	  RemoveOption(*spec);
	  ReaderStage_Alpha* alpha = new ReaderStage_Alpha;
	  char* name = ExtractNextOption(*spec); RemoveOption(*spec);
	  alpha->SetAlphaStream(name);
	  delete[] name;
	  return alpha;
	}
      else
	return NULL;
    }

    const char* Name() const { return "filter: add alpha channel"; }

  } singleton_alpha;


}



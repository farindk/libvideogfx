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

#include "libvideogfx/graphics/fileio/unified_loader.hh"
#include "libvideogfx/graphics/fileio/mplayer.hh"
#include "libvideogfx/graphics/color/colorspace.hh"
#include "libvideogfx/graphics/draw/draw.hh"
#include "libvideogfx/graphics/draw/scale.hh"

#include <string.h>
#include <iostream>

namespace videogfx {
  using namespace std;

  const FileIOFactory* UnifiedImageLoader::s_plugins[MAX_LOADER_PLUGINS];
  int UnifiedImageLoader::s_nplugins;


  void LoaderPlugin::SetPrevious(LoaderPlugin* previous)
  {
    if (prev) prev->SetPrevious(previous);
    else prev = previous;
  }

  void UnifiedImageLoader::RegisterPlugin(const FileIOFactory* fact)
  {
    Assert(s_nplugins < MAX_LOADER_PLUGINS);
    s_plugins[s_nplugins++] = fact;
  }


  bool UnifiedImageLoader::SetInput(const char* input_specification)
  {
    char* spec = new char[strlen(input_specification)+1];
    strcpy(spec,input_specification);

    d_loader_pipeline = NULL;

    while (*spec)
      {
	cout << "current specification: " << spec << endl;

	bool found_one = false;
	for (int i=0;i<s_nplugins;i++)
	  {
	    cout << "  try plugin: " << s_plugins[i]->Name() << endl;

	    LoaderPlugin* newpipe = s_plugins[i]->ParseSpec(&spec);
	    if (newpipe)
	      {
		cout << "MATCH\n";

		newpipe->SetPrevious(d_loader_pipeline);
		d_loader_pipeline=newpipe;
		found_one = true;
		break;
	      }
	  }

	if (!found_one)
	  {
	    delete d_loader_pipeline;
	    d_loader_pipeline=NULL;
	    delete[] spec;
	    return false;
	  }
      }

    delete[] spec;
    return true;
  }


  int  UnifiedImageLoader::AskNFrames() const { Assert(d_loader_pipeline); return d_loader_pipeline->AskNFrames(); }
  bool UnifiedImageLoader::IsEOF() const { Assert(d_loader_pipeline); return d_loader_pipeline->IsEOF(); }
  bool UnifiedImageLoader::SkipToImage(int nr) { Assert(d_loader_pipeline); return d_loader_pipeline->SkipToImage(nr); }

  void UnifiedImageLoader::ReadImage(Image<Pixel>& img)
  {
    AssertDescr(d_loader_pipeline, "no loader specified");

    d_loader_pipeline->ReadImage(img);

    if (d_colorspace != Colorspace_Invalid || d_chroma != Chroma_Invalid)
      {
	Image<Pixel> tmp;
	ChangeColorspace(tmp,img, d_colorspace, d_chroma);
	cout << "CONVERT\n";
	img = tmp;
      }
  }





  FileIOFactory::FileIOFactory() { UnifiedImageLoader::RegisterPlugin(this); }


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

    cout << "len: " << len << endl;

    if (len<strlen(suffix)+2)
      return false;

    p -= strlen(suffix)+1;
    cout << "suffix in file: " << p << endl;

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


  class LoaderPlugin_RGB : public LoaderPlugin
  {
  public:
    LoaderPlugin_RGB() { id=0; }

    int  AskNFrames() const { return 9999999; }
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


  class FileIOFactory_RGB : public FileIOFactory
  {
  public:
    LoaderPlugin* ParseSpec(char** spec) const
    {
      if (MatchOption(*spec, "rgb"))
	{
	  RemoveOption(*spec);
	  int w,h;
	  ExtractSize(*spec,w,h,352,288);
	  
	  LoaderPlugin_RGB* pl = new LoaderPlugin_RGB;
	  pl->SetSize(w,h);
	  return pl;
	}
      else
	return NULL;
    }

    const char* Name() const { return "test pattern: cycle through RGB"; }

  } singleton_rgb;



  // ------------------------------------------------------------------------------



  class LoaderPlugin_MPlayer : public LoaderPlugin
  {
  public:
    int  AskNFrames() const { return 999999; }
    bool IsEOF() const { return reader.IsEOF(); }

    bool SkipToImage(int nr) { reader.SkipToImage(nr); return true; } // only forward seek
    void ReadImage(Image<Pixel>& img) { reader.ReadImage(img); }

    void SetInput(const char* name) { reader.Open(name); }

  private:
    FileReader_MPlayer reader;
  };


  class FileIOFactory_MPlayer : public FileIOFactory
  {
  public:
    LoaderPlugin* ParseSpec(char** spec) const
    {
      if (CheckSuffix(*spec, "vdr") ||
	  CheckSuffix(*spec, "avi") ||
	  CheckSuffix(*spec, "wmf"))
	{
	  LoaderPlugin_MPlayer* pl = new LoaderPlugin_MPlayer;
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



  class LoaderPlugin_Quarter : public LoaderPlugin
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

      CopyScaled(img,0,0,deinter.AskWidth()/2, deinter.AskHeight(), deinter);
    }
  };


  class FileIOFactory_Quarter : public FileIOFactory
  {
  public:
    LoaderPlugin* ParseSpec(char** spec) const
    {
      if (MatchOption(*spec, "quarter"))
	{
	  RemoveOption(*spec);
	  return new LoaderPlugin_Quarter;
	}
      else
	return NULL;
    }

    const char* Name() const { return "filter: quarter size"; }

  } singleton_quarter;
}


#include <libvideogfx/graphics/fileio/imagesink.hh>
#include <libvideogfx/graphics/fileio/jpeg.hh>
#include <libvideogfx/graphics/fileio/ppm.hh>
#include <libvideogfx/graphics/color/colorspace.hh>
#include <stdio.h>

namespace videogfx {
  using namespace std;

  ImageSink_Save::ImageSink_Save(ImageFileFormat fmt)
    : d_filename_template(NULL),
      d_curr_nr(0)
  {
    SetFileFormat(fmt);
    SetFilename("img",true);
  }

  ImageSink_Save::~ImageSink_Save()
  {
    if (d_filename_template) delete[] d_filename_template;
  }

  void ImageSink_Save::SetFilename(const char* tmpl, bool autosuffix)
  {
    if (d_filename_template) delete[] d_filename_template;

    int len = strlen(tmpl+(autosuffix ? 4 : 0)+1);
    d_filename_template = new char[len];

    strcpy(d_filename_template,tmpl);
    d_autosuffix = autosuffix;
  }

  void ImageSink_Save::SendImage(const Image<Pixel>& img)
  {
    // construct output filename

    bool  number = false;
    char* suffix = NULL;

    switch(d_format)
      {
      case Format_JPEG:
	number = true;
	suffix = "jpg";
	break;
      case Format_PPM:
	number = true;
	suffix = "ppm";
	break;
      case Format_PGM:
	number = true;
	suffix = "pgm";
	break;
      }

    Assert(suffix);

    char* buf = new char[strlen(d_filename_template) + 100];

    if (number)
      sprintf(buf,"%s%05d",d_filename_template,d_curr_nr);
    else
      strcpy(buf,d_filename_template);

    if (d_autosuffix)
      {
	strcat(buf,".");
	strcat(buf,suffix);
      }


    // save file

    switch(d_format)
      {
      case Format_JPEG:
	{
	  Image<Pixel> dst;
	  if (img.AskParam().colorspace == Colorspace_YUV &&
	      img.AskParam().chroma     == Chroma_420)
	    {
	      dst = img;
	    }
	  else
	    {
	      ImageParam param = img.AskParam();
	      param.chroma = Chroma_420;
	      dst.SetParam(param);
	      ChangeColorspace(dst,img,Colorspace_YUV);
	    }

	  WriteImage_JPEG(dst,buf);
	}
	break;

      case Format_PPM:
	{
	  Image<Pixel> dst;
	  if (img.AskParam().colorspace == Colorspace_RGB)
	    dst = img;
	  else
	    ChangeColorspace(dst,img,Colorspace_RGB);

	  ofstream ostr(buf);
	  WriteImage_PPM(dst,ostr);
	}
	break;

      case Format_PGM:
	{
	  Image<Pixel> dst;
	  if (img.AskParam().colorspace == Colorspace_Greyscale)
	    dst = img;
	  else
	    ChangeColorspace(dst,img,Colorspace_Greyscale);

	  ofstream ostr(buf);
	  WriteImage_PPM(dst,ostr);
	}
	break;
      }

    delete[] buf;
    d_curr_nr++;
  }

}


#include "libvideogfx/graphics/fileio/jpeg.hh"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>

namespace videogfx {
  using namespace std;

#ifndef HAVE_LIBJPEG
  bool JPEG_Supported()
  {
    return false;
  }

  void ReadImage_JPEG(const char* filename,Image_YUV<Pixel>& img)
  {
    AssertDescr(false,"JPEG support has not been compiled into libvideogfx.\n");
  }

  void WriteImage_JPEG(const char* filename,const Image_YUV<Pixel>& img)
  {
    AssertDescr(false,"JPEG support has not been compiled into libvideogfx.\n");
  }

#else

  extern "C" {
#include <jpeglib.h>
  }

  bool JPEG_Supported()
  {
    return true;
  }


  void ReadImage_JPEG(Image<Pixel>& img,const char* filename)
  {
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    // open input file

    FILE * infile;
    if ((infile = fopen(filename, "rb")) == NULL) {
      fprintf(stderr, "can't open %s\n", filename);
      exit(1);
    }


    // initialize decompressor

    jpeg_create_decompress(&cinfo);
  
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_stdio_src(&cinfo, infile);

    jpeg_read_header(&cinfo, TRUE);
    cinfo.out_color_space = JCS_YCbCr;

    jpeg_start_decompress(&cinfo);

    JSAMPARRAY buffer;
    buffer = (*cinfo.mem->alloc_sarray)
      ((j_common_ptr) &cinfo, JPOOL_IMAGE, cinfo.output_width * cinfo.output_components, 1);


    // create destination image

    ImageParam spec = img.AskParam();
    spec.width      = cinfo.output_width;
    spec.height     = cinfo.output_height;
    spec.colorspace = Colorspace_YUV;
    spec.chroma     = Chroma_420;
    img.Create(spec);

    Pixel*const* py  = img.AskFrameY();
    Pixel*const* pcb = img.AskFrameCb();
    Pixel*const* pcr = img.AskFrameCr();


    // read the image

    while (cinfo.output_scanline < cinfo.output_height) {
      JOCTET* bufp;

      (void) jpeg_read_scanlines(&cinfo, buffer, 1);

      bufp = buffer[0];

      for (unsigned int x=0;x<cinfo.output_width;x+=2)
	{
	  py [cinfo.output_scanline-1][x] = *bufp++;
	  pcb[(cinfo.output_scanline-1)/2][x/2] = *bufp++;
	  pcr[(cinfo.output_scanline-1)/2][x/2] = *bufp++;
	  py [cinfo.output_scanline-1][x+1] = *bufp++;
	  bufp+=2;
	}


      (void) jpeg_read_scanlines(&cinfo, buffer, 1);

      bufp = buffer[0];

      for (unsigned int x=0;x<cinfo.output_width;x++)
	{
	  py [cinfo.output_scanline-1][x] = *bufp++;
	  bufp+=2;
	}
    }


    // cleanup

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    fclose(infile);
  }




  void WriteImage_JPEG(const Image<Pixel>& img,const char* filename)
  {
    Assert(img.AskParam().colorspace == Colorspace_YUV);

    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    // open output file

    FILE * outfile;
    if ((outfile = fopen(filename, "wb")) == NULL) {
      fprintf(stderr, "can't open %s\n", filename);
      exit(1);
    }


    // initialize compressor

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = img.AskWidth();
    cinfo.image_height = img.AskHeight();
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_YCbCr;

    jpeg_set_defaults(&cinfo);

    jpeg_start_compress(&cinfo,TRUE);

    JSAMPARRAY buffer;
    buffer = (*cinfo.mem->alloc_sarray)
      ((j_common_ptr) &cinfo, JPOOL_IMAGE, cinfo.image_width * cinfo.input_components, 1);


    const Pixel*const* py  = img.AskFrameY();
    const Pixel*const* pcb = img.AskFrameCb();
    const Pixel*const* pcr = img.AskFrameCr();


    // write the image

    while (cinfo.next_scanline < cinfo.image_height) {
      JOCTET* bufp;
      JSAMPROW row[1];
      row[0]=buffer[0];

      bufp = buffer[0];

      for (unsigned int x=0;x<cinfo.image_width;x++)
	{
	  *bufp++ = py[cinfo.next_scanline][x];
	  *bufp++ = pcb[(cinfo.next_scanline)/2][x/2];
	  *bufp++ = pcr[(cinfo.next_scanline)/2][x/2];
	}

      jpeg_write_scanlines(&cinfo, row, 1);
    }


    // cleanup

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
  
    fclose(outfile);
  }
#endif

}

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

#include "libvideogfx/graphics/fileio/png.hh"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

#ifdef HAVE_LIBPNG
extern "C" {
#include <png.h>
}
#endif


namespace videogfx {
  using namespace std;

#ifndef HAVE_LIBPNG
  bool PNG_Supported()
  {
    return false;
  }

  void ReadImage_PNG(Image<Pixel>& img, istream& stream)
  {
    AssertDescr(false,"PNG support has not been compiled into libvideogfx.\n");
  }

  void WriteImage_PNG(ostream& stream, const Image<Pixel>& img)
  {
    AssertDescr(false,"PNG support has not been compiled into libvideogfx.\n");
  }

#else
  static
  void
  user_read_fn(png_structp png_ptr, png_bytep data, png_size_t length)
  {
    // get pointer of stream
    istream* is = (istream *)png_get_io_ptr(png_ptr);
    is->read((char*)data, length);
  } // user_read_data


  static
  void
  user_write_fn (png_structp png_ptr, png_bytep data, png_size_t length)
  {
    // get pointer of stream
    ostream* os = (ostream *)png_get_io_ptr(png_ptr);
    os->write((const char*)data, length);
  }

  static
  void
  user_flush_fn (png_structp png_ptr)
  {
    // get pointer of stream
    ostream* os = (ostream *)png_get_io_ptr(png_ptr);
    os->flush();
  }


  bool PNG_Supported()
  {
    return true;
  }


  void ReadImage_PNG(Image<Pixel>& img, istream& is)
  {
    AssertDescr(is != NULL, "Open stream first.");

    png_structp png_ptr;
    png_infop info_ptr;
    png_uint_32 width, height;
    int bit_depth, color_type, interlace_type;

    /* Create and initialize the png_struct with the desired error handler
     * functions.  If you want to use the default stderr and longjump method,
     * you can supply NULL for the last three parameters.  We also supply the
     * the compiler header file version, so that we know if the application
     * was compiled with a compatible version of the library.  REQUIRED
     */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    Assert(png_ptr != NULL);

    /* Allocate/initialize the memory for image information.  REQUIRED. */
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
      png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
      AssertDescr(false, "could not create info_ptr");
    } // if

    /* Set error handling if you are using the setjmp/longjmp method (this is
     * the normal method of doing things with libpng).  REQUIRED unless you
     * set up your own error handlers in the png_create_read_struct() earlier.
     */
    if (setjmp(png_jmpbuf(png_ptr))) {
      /* Free all of the memory associated with the png_ptr and info_ptr */
      png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
      /* If we get here, we had a problem reading the file */
      AssertDescr(false, "fatal error in png library");
    } // if

      /* If you are using replacement read functions, instead of calling
       * png_init_io() here you would call: */
    png_set_read_fn(png_ptr, (void *)&is, user_read_fn);
    /* where user_io_ptr is a structure you want available to the callbacks */

    /* The call to png_read_info() gives us all of the information from the
     * PNG file before the first IDAT (image data chunk).  REQUIRED
     */
    png_read_info(png_ptr, info_ptr);

    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
		 &interlace_type, NULL, NULL);

    AssertDescr(bit_depth < 16, "cannot handle 16 bit images");
      
    /**** Set up the data transformations you want.  Note that these are all
     **** optional.  Only call them if you want/need them.  Many of the
     **** transformations only work on specific types of images, and many
     **** are mutually exclusive.
     ****/

    // \TODO
    //      /* Strip alpha bytes from the input data without combining with the
    //       * background (not recommended).
    //       */
    //      png_set_strip_alpha(png_ptr);

    /* Extract multiple pixels with bit depths of 1, 2, and 4 from a single
     * byte into separate bytes (useful for paletted and grayscale images).
     */
    png_set_packing(png_ptr);


    /* Expand paletted colors into true RGB triplets */
    if (color_type == PNG_COLOR_TYPE_PALETTE)
      png_set_expand(png_ptr);

    /* Expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel */
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
      png_set_expand(png_ptr);

    /* Set the background color to draw transparent and alpha images over.
     * It is possible to set the red, green, and blue components directly
     * for paletted images instead of supplying a palette index.  Note that
     * even if the PNG file supplies a background, you are not required to
     * use it - you should use the (solid) application background if it has one.
     */

#if 0
    // \TODO 0 is index in color lookup table - correct? used already?
    png_color_16 my_background = {0, 255, 255, 255, 255};
    png_color_16 *image_background;

    if (png_get_bKGD(png_ptr, info_ptr, &image_background))
      png_set_background(png_ptr, image_background, PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
    else
      png_set_background(png_ptr, &my_background, PNG_BACKGROUND_GAMMA_SCREEN, 0, 1.0);
#endif


    /* Optional call to gamma correct and add the background to the palette
     * and update info structure.  REQUIRED if you are expecting libpng to
     * update the palette for you (ie you selected such a transform above).
     */
    png_read_update_info(png_ptr, info_ptr);

    /* Allocate the memory to hold the image using the fields of info_ptr. */

    /* The easiest way to read the image: */
    uint8** row_pointers = new png_bytep[height];
    Assert(row_pointers != NULL);

    for (uint32 y = 0; y < height; y++) {
      row_pointers[y] = (png_bytep)malloc(png_get_rowbytes(png_ptr, info_ptr));
      Assert(row_pointers[y] != NULL);
    } // for

      /* Now it's time to read the image.  One of these methods is REQUIRED */
    png_read_image(png_ptr, row_pointers);

    /* read rest of file, and get additional chunks in info_ptr - REQUIRED */
    png_read_end(png_ptr, info_ptr);

    /* clean up after the read, and free any memory allocated - REQUIRED */
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);


    // OK, now we should have the png image in some way in
    // row_pointers, have fun with it

    int band;
    switch (color_type) {
    case PNG_COLOR_TYPE_GRAY:
    case PNG_COLOR_TYPE_GRAY_ALPHA:
      band = 1;
      break;
    case PNG_COLOR_TYPE_PALETTE:
    case PNG_COLOR_TYPE_RGB:
    case PNG_COLOR_TYPE_RGB_ALPHA:
      band = 3;
      break;
    default:
      AssertDescr(false, "unknown color type in png image.");
    } // switch

    ImageParam param = img.AskParam();
    param.width  = width;
    param.height = height;
    param.has_alpha = (color_type & PNG_COLOR_MASK_ALPHA);

    if (band == 1) {
      param.colorspace = Colorspace_Greyscale;
      img.Create(param);
      Pixel*const* Y = img.AskFrameY();
      Pixel*const* A = (param.has_alpha ? img.AskFrameA() : NULL);

      for (uint32 y = 0; y < height; y++) {
	uint8* p = row_pointers[y];

	if (param.has_alpha)
	  {
	    assert(A != NULL);
	    for (uint32 x = 0; x < width; x++) {
	      Y[y][x] = *p++;
	      A[y][x] = *p++;
	    }
	  }
	else
	  {
	    memcpy(Y[y],p, width);
	  }
      }
    } else {
      param.colorspace = Colorspace_RGB;
      img.Create(param);
      Pixel*const* R = img.AskFrameR();
      Pixel*const* G = img.AskFrameG();
      Pixel*const* B = img.AskFrameB();
      Pixel*const* A = (param.has_alpha ? img.AskFrameA() : NULL);

      for (uint32 y = 0; y < height; y++) {
	uint8* p = row_pointers[y];

	if (param.has_alpha)
	  {
	    assert(A != NULL);
	    for (uint32 x = 0;x < width; x++) {
	      R[y][x] = *p++;
	      G[y][x] = *p++;
	      B[y][x] = *p++;
	      A[y][x] = *p++;
	    }
	  }
	else
	  {
	    for (uint32 x = 0;x < width; x++) {
	      R[y][x] = *p++;
	      G[y][x] = *p++;
	      B[y][x] = *p++;
	    }
	  }
      }
    }

    for (uint32 y = 0; y < height; y++) {
      free(row_pointers[y]);
    } // for

    delete[] row_pointers;
  }
    
  void WriteImage_PNG(ostream& os, const Image<Pixel>& img)
  {
    /* Create and initialize the png_struct with the desired error handler
     * functions.  If you want to use the default stderr and longjump method,
     * you can supply NULL for the last three parameters.  We also check that
     * the library version is compatible with the one used at compile time,
     * in case we are using dynamically linked libraries.  REQUIRED.
     */
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    Assert(png_ptr != NULL);

    /* Allocate/initialize the image information data.  REQUIRED */
    png_infop info_ptr = png_create_info_struct(png_ptr);
    Assert(info_ptr != NULL);

    /* Set error handling.  REQUIRED if you aren't supplying your own
     * error hadnling functions in the png_create_write_struct() call.
     */
    if (setjmp(png_jmpbuf(png_ptr))) {
      /* If we get here, we had a problem reading the file */
      png_destroy_write_struct(&png_ptr,  (png_infopp)NULL);
      AssertDescr(false, "could not write png file");
    }

    /* If you are using replacement read functions, instead of calling
     * png_init_io() here you would call */
    png_set_write_fn(png_ptr, (void *) &os, user_write_fn, user_flush_fn);
    /* where user_io_ptr is a structure you want available to the callbacks */


    /* Set the image information here.  Width and height are up to 2^31,
     * bit_depth is one of 1, 2, 4, 8, or 16, but valid values also depend on
     * the color_type selected. color_type is one of PNG_COLOR_TYPE_GRAY,
     * PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE, PNG_COLOR_TYPE_RGB,
     * or PNG_COLOR_TYPE_RGB_ALPHA.  interlace is either PNG_INTERLACE_NONE or
     * PNG_INTERLACE_ADAM7, and the compression_type and filter_type MUST
     * currently be PNG_COMPRESSION_TYPE_BASE and PNG_FILTER_TYPE_BASE. REQUIRED
     */

    // handle only 8 bit images
    int bit_depth = 8;

    ImageParam param = img.AskParam();
    int color_type;
    int bands;
    if (param.colorspace == Colorspace_RGB) {
      color_type = PNG_COLOR_TYPE_RGB;
      bands = 3;
    } else if (param.colorspace == Colorspace_Greyscale) {
      color_type = PNG_COLOR_TYPE_GRAY;
      bands = 1;
    } else {
      AssertDescr(false,"can only save RGB or grayscale images");
    }

    if (param.has_alpha) { color_type |= PNG_COLOR_MASK_ALPHA; bands++; }

    const int w = img.AskWidth();
    const int h = img.AskHeight();

    png_set_IHDR(png_ptr, info_ptr, w, h, bit_depth, color_type,
		 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    /* Write the file header information.  REQUIRED */
    png_write_info(png_ptr, info_ptr);

    uint8** row_pointers = new uint8*[h];
    Assert(row_pointers);

    for (int y = 0; y < h; y++) {
      row_pointers[y] = new uint8[w * bands];
      Assert(row_pointers[y]);
    } // for

    if (bands == 1 || bands == 2) {
      const Pixel*const* Y = img.AskFrameY();
      const Pixel*const* A = ( param.has_alpha ? img.AskFrameA() : NULL );

      for (int y = 0; y < h; y++) {
	uint8* p = row_pointers[y];

	if (param.has_alpha)
	  {
	    for (int x = 0; x < w; x++) {
	      *p++ = Y[y][x];
	      *p++ = A[y][x];
	    }
	  }
	else
	  {
	    for (int x = 0; x < w; x++) {
	      // could use memcpy here
	      *p++ = Y[y][x];
	    }
	  }
      }
    } else {
      const Pixel*const* R = img.AskFrameR();
      const Pixel*const* G = img.AskFrameG();
      const Pixel*const* B = img.AskFrameB();
      const Pixel*const* A = ( param.has_alpha ? img.AskFrameA() : NULL );

      for (int y = 0; y < h; y++) {
	uint8* p = row_pointers[y];

	if (param.has_alpha)
	  {
	    for (int x = 0;x < w; x++) {
	      *p++ = R[y][x];
	      *p++ = G[y][x];
	      *p++ = B[y][x];
	      *p++ = A[y][x];
	    }
	  }
	else
	  {
	    for (int x = 0;x < w; x++) {
	      *p++ = R[y][x];
	      *p++ = G[y][x];
	      *p++ = B[y][x];
	    }
	  }
      }
    }
      
      
    png_write_image(png_ptr, row_pointers);

    /* It is REQUIRED to call this to finish writing the rest of the file */
    png_write_end(png_ptr, info_ptr);

    /* clean up after the write, and free any memory allocated */
    png_destroy_write_struct(&png_ptr, (png_infopp)NULL);

    for (int y = 0; y < h; y++) {
      delete[] row_pointers[y];
    } // for

    delete[] row_pointers;
  }

#endif


  bool ReadImage_PNG(Image<Pixel>& img, const char* filename)
  {
    ifstream ifs(filename, ios::in | ios::binary);
    if (!ifs)
      { return false; }

    ReadImage_PNG(img, ifs);
    return true;
  }

  void WriteImage_PNG(const char* filename, const Image<Pixel>& img)
  {
    ofstream ofs(filename, ios::out | ios::binary);
    Assert(ofs);
    WriteImage_PNG(ofs, img);
  }
}

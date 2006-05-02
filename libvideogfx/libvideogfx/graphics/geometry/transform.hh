/*********************************************************************
  libvideogfx/graphics/geometry/transform.hh

  purpose:
    Image geometry transforms.

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
   30/Nov/1999 - Dirk Farin
    - affine transformation
 ********************************************************************************
    LibVideoGfx - video processing library
    Copyright (C) 1999  Dirk Farin

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

#ifndef LIBVIDEOGFX_GRAPHICS_GEOMETRY_TRANSFORM_HH
#define LIBVIDEOGFX_GRAPHICS_GEOMETRY_TRANSFORM_HH

#include <libvideogfx/graphics/datatypes/image.hh>
#include <libvideogfx/graphics/datatypes/primitives.hh>
#include <libvideogfx/graphics/geometry/matrix.hh>
#include <iostream>


namespace videogfx {

  Matrix4G PerspFrom4PointCorrespondences(const Point2D<double>* start,  // array of 4 points
					  const Point2D<double>* end);   // array of 4 points
  Matrix4G AffineFrom3PointCorrespondences(const Point2D<double>* start, // array of 3 points
					   const Point2D<double>* end);  // array of 3 points

  /* If an alpha-bitmap is given, the bitmap is overlayed such that alpha=0 means completely transparent and
     alpha=255 means completely opaque.
     If 'truncate' is true, bilinear interpolation is switched off, which is approximately 20% faster.
  */
  void TransformBitmap(Bitmap<Pixel>& dest, const Bitmap<Pixel>& src, const Matrix4G& homography_dest2src,
		       const Bitmap<Pixel>* alpha=NULL,bool truncate=false);
}

#endif

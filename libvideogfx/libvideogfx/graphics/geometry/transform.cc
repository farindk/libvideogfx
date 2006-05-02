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

#include <libvideogfx/graphics/geometry/transform.hh>
#include <libvideogfx/graphics/draw/draw.hh>
#include <gsl/gsl_linalg.h>
#include <algorithm>
#include <iomanip>
#include <math.h>

namespace videogfx {
  using namespace std;

  Matrix4G PerspFrom4PointCorrespondences(const Point2D<double>* start,
					  const Point2D<double>* end)
  {
    AssertDescr(0, "Will be implemented when 4x4 matrix inverse is ready");
#if 0
    gsl_vector* rhs      = gsl_vector_alloc(8);
    gsl_matrix* matrix   = gsl_matrix_alloc(8,8);
    gsl_permutation* per = gsl_permutation_calloc(8);

    // build linear equation system to be solved

    for (int i=0;i<4;i++)
      {
	gsl_matrix_set(matrix,2*i  ,0,start[i].x);
	gsl_matrix_set(matrix,2*i  ,1,start[i].y);
	gsl_matrix_set(matrix,2*i  ,2,1         );
	gsl_matrix_set(matrix,2*i  ,3,0         );
	gsl_matrix_set(matrix,2*i  ,4,0         );
	gsl_matrix_set(matrix,2*i  ,5,0         );
	gsl_matrix_set(matrix,2*i  ,6,-start[i].x * end[i].x);
	gsl_matrix_set(matrix,2*i  ,7,-start[i].y * end[i].x);

	gsl_vector_set(rhs,2*i  ,end[i].x);

	gsl_matrix_set(matrix,2*i+1,0,0         );
	gsl_matrix_set(matrix,2*i+1,1,0         );
	gsl_matrix_set(matrix,2*i+1,2,0         );
	gsl_matrix_set(matrix,2*i+1,3,start[i].x);
	gsl_matrix_set(matrix,2*i+1,4,start[i].y);
	gsl_matrix_set(matrix,2*i+1,5,1         );
	gsl_matrix_set(matrix,2*i+1,6,-start[i].x * end[i].y);
	gsl_matrix_set(matrix,2*i+1,7,-start[i].y * end[i].y);

	gsl_vector_set(rhs,2*i+1,end[i].y);
      }

    // solve the equation system using LU-decomposition

    int signum=0;
    gsl_linalg_LU_decomp(matrix,per,&signum);
    gsl_linalg_LU_svx(matrix,per,rhs);

    // fill the perspective transformation fields

    PerspTransformation t;
    t.a[0][0] = gsl_vector_get(rhs,0);
    t.a[0][1] = gsl_vector_get(rhs,1);
    t.tx      = gsl_vector_get(rhs,2);
    t.a[1][0] = gsl_vector_get(rhs,3);
    t.a[1][1] = gsl_vector_get(rhs,4);
    t.ty      = gsl_vector_get(rhs,5);
    t.px      = gsl_vector_get(rhs,6);
    t.py      = gsl_vector_get(rhs,7);

    // cleanup

    gsl_vector_free(rhs);
    gsl_permutation_free(per);
    gsl_matrix_free(matrix);

    return t;
#endif
  }


  Matrix4G AffineFrom3PointCorrespondences(const Point2D<double>* start,
					   const Point2D<double>* end)
  {
    AssertDescr(0, "AffineFrom3PointCorrespondences will be implemented based on 3x3 matrix inverse");
#if 0
    gsl_vector* rhs      = gsl_vector_alloc(6);
    gsl_matrix* matrix   = gsl_matrix_alloc(6,6);
    gsl_permutation* per = gsl_permutation_calloc(6);

    // build linear equation system to be solved

    for (int i=0;i<3;i++)
      {
	gsl_matrix_set(matrix,2*i  ,0,start[i].x);
	gsl_matrix_set(matrix,2*i  ,1,start[i].y);
	gsl_matrix_set(matrix,2*i  ,2,1         );
	gsl_matrix_set(matrix,2*i  ,3,0         );
	gsl_matrix_set(matrix,2*i  ,4,0         );
	gsl_matrix_set(matrix,2*i  ,5,0         );

	gsl_vector_set(rhs,2*i  ,end[i].x);

	gsl_matrix_set(matrix,2*i+1,0,0         );
	gsl_matrix_set(matrix,2*i+1,1,0         );
	gsl_matrix_set(matrix,2*i+1,2,0         );
	gsl_matrix_set(matrix,2*i+1,3,start[i].x);
	gsl_matrix_set(matrix,2*i+1,4,start[i].y);
	gsl_matrix_set(matrix,2*i+1,5,1         );

	gsl_vector_set(rhs,2*i+1,end[i].y);
      }

    // solve the equation system using LU-decomposition

    int signum=0;
    gsl_linalg_LU_decomp(matrix,per,&signum);
    gsl_linalg_LU_svx(matrix,per,rhs);

    // fill the affine transformation fields

    AffineTransformation t;
    t.a[0][0] = gsl_vector_get(rhs,0);
    t.a[0][1] = gsl_vector_get(rhs,1);
    t.tx      = gsl_vector_get(rhs,2);
    t.a[1][0] = gsl_vector_get(rhs,3);
    t.a[1][1] = gsl_vector_get(rhs,4);
    t.ty      = gsl_vector_get(rhs,5);

    // cleanup

    gsl_vector_free(rhs);
    gsl_permutation_free(per);
    gsl_matrix_free(matrix);

    return t;
#endif
  }



  /*
    When drawing a perspectively transformed image, a simple method is to
    determine the bounding box in the output and scan the area, finding
    back each pixel position in the input. However, this is inefficient
    since many pixels in the bounding box fall outside of the input image
    and are hence scanned without use.
    ----- this was the first solution, but this is bad since the scanline
    ----- might cross the horizont line in which case the scanline cannot
    ----- be converted into the source space
    We solve this by transforming the scanline parameters into the input
    space and then clipping this line to the input dimensions. After
    transforming the clipped line back to output coordinates, we get the
    exact range of valid pixel positions.
    Because of rounding errors, we add a safety border around the clipping
    rectangle so that a little larger area is scanned.
    An extension is possible, which also clips the scanline at a rectangle
    slightly inside of the source image. Within this rectangle, the transform
    can be computed without the necessity to check if each pixel falls within
    the valid source area. However, it seems that the branch prediction of
    the processor handles the if statement so well that no improvement could
    be obtained with it.
    ----- end of first solution

    In the new second solution, the scanline is directly intersected with the
    quadrilateral in the destination space.
  */
  void TransformBitmap(Bitmap<Pixel>& dest, const Bitmap<Pixel>& src, const Matrix4G& homo_dest2src,
		       const Bitmap<Pixel>* alpha,bool truncate)
  {
    const int safety_border = 2;

    const Matrix4G& dest2src = homo_dest2src;
    const Matrix4G  src2dest = dest2src.Inverse();

    const int minsrcx = -src.AskXOffset();
    const int minsrcy = -src.AskYOffset();
    const int maxsrcx = -src.AskXOffset()+src.AskWidth()-1;
    const int maxsrcy = -src.AskYOffset()+src.AskHeight()-1;

    // create rectangle of size of the source bitmap

    Point2D<double> sp[4];
    sp[0].x = maxsrcx; sp[0].y = maxsrcy;
    sp[1].x = maxsrcx; sp[1].y = minsrcy;
    sp[2].x = minsrcx; sp[2].y = minsrcy;
    sp[3].x = minsrcx; sp[3].y = maxsrcy;

    /* Transform source rectangle to destination to determine bounding-box.
       - Add some safety border around bounding box to be sure to transform the whole bitmap.
       - The bounding box is cropped vertically to the maximum destination area.
       - The horizontal range is cropped later when the exact range has been determined.
    */

    Point2D<double> dp[4];
    for (int i=0;i<4;i++)
      dp[i] = TransformPoint(src2dest,sp[i]);
 
    int ymin = (int)floor(std::min(std::min(dp[0].y,dp[1].y), std::min(dp[2].y,dp[3].y))) - safety_border;
    int ymax = (int)ceil (std::max(std::max(dp[0].y,dp[1].y), std::max(dp[2].y,dp[3].y))) + safety_border;

    if (ymin<0) ymin=0;
    if (ymax>=dest.AskHeight()) ymax=dest.AskHeight()-1;

    // for each scanline, determine the range that is to be transformed

    const Pixel*const* s = src.AskFrame();
    const Pixel*const* al = (alpha ? alpha->AskFrame() : NULL);
    Pixel*const* d = dest.AskFrame();

    Point2D<double> p;
    for (int y=ymin;y<=ymax;y++)
      {
	// calculate crossings of scanline with quadrilateral

	double xcut[4]; int xcutidx=0;
	for (int i=0;i<4;i++)
	  {
	    double y0 = dp[i].y;
	    double y1 = dp[(i+1)%4].y;
	    if (y0==y1)
	      continue;

	    double a = (y-y1)/(y0-y1);
	    if (a>=0.0 && a<=1.0)
	      {
		double x0 = dp[i].x;
		double x1 = dp[(i+1)%4].x;
		xcut[xcutidx++] = a*x0+(1.0-a)*x1;
	      }
	  }
	assert(xcutidx==2 || xcutidx==0);

	if (xcutidx==0)
	  continue;

	int xstart = (int)std::min(xcut[0],xcut[1])       - safety_border;
	int xend   = (int)ceil(std::max(xcut[0],xcut[1])) + safety_border;

	if (xstart<0) xstart=0;
	if (xend>=dest.AskWidth()) xend=dest.AskWidth()-1;

	// now finally transform the scanline

	Point2D<double> newp;

	if (!truncate)
	  {
	    if (al != NULL)
	      {
		// === transform with alpha channel ===

		for (int x=xstart;x<=xend;x++)
		  {
		    // transform the pixel coordinate

		    newp.x = (x*dest2src[0][0] + y*dest2src[0][1] + dest2src[0][2]) /
		      (x*dest2src[2][0] + y*dest2src[2][1] + dest2src[2][2]);
		    newp.y = (x*dest2src[1][0] + y*dest2src[1][1] + dest2src[1][2]) /
		      (x*dest2src[2][0] + y*dest2src[2][1] + dest2src[2][2]);

		    double x0 = floor(newp.x);
		    double y0 = floor(newp.y);
		    double wx = newp.x-x0;
		    double wy = newp.y-y0;
		    int ix = (int)x0;
		    int iy = (int)y0;

		    if (ix<=maxsrcx-1 && iy<=maxsrcy-1 && ix>=minsrcx && iy>=minsrcy)
		      {
			Pixel input = (Pixel)((s[iy  ][ix]*(1-wx) + s[iy  ][ix+1]*(wx))*(1-wy) +
					      (s[iy+1][ix]*(1-wx) + s[iy+1][ix+1]*(wx))*(  wy));
			Pixel alphaval = al[iy][ix];

			d[y][x] = (input*alphaval + d[y][x]*(255-alphaval))/255;
		      }
		  }
	      }
	    else
	      {
		// === transform without alpha channel ===

		for (int x=xstart;x<=xend;x++)
		  {
		    // transform the pixel coordinate

		    newp.x = (x*dest2src[0][0] + y*dest2src[0][1] + dest2src[0][2]) /
		      (x*dest2src[2][0] + y*dest2src[2][1] + dest2src[2][2]);
		    newp.y = (x*dest2src[1][0] + y*dest2src[1][1] + dest2src[1][2]) /
		      (x*dest2src[2][0] + y*dest2src[2][1] + dest2src[2][2]);

		    double x0 = floor(newp.x);
		    double y0 = floor(newp.y);
		    double wx = newp.x-x0;
		    double wy = newp.y-y0;
		    int ix = (int)x0;
		    int iy = (int)y0;

		    if (ix<=maxsrcx-1 && iy<=maxsrcy-1 && ix>=minsrcx && iy>=minsrcy)
		      {
			d[y][x] = (Pixel)((s[iy  ][ix]*(1-wx) + s[iy  ][ix+1]*(wx))*(1-wy) +
					  (s[iy+1][ix]*(1-wx) + s[iy+1][ix+1]*(wx))*(  wy));
		      }
		  }
	      }
	  }
	else
	  {
	    if (al != NULL)
	      {
		// === transform with alpha channel ===

		for (int x=xstart;x<=xend;x++)
		  {
		    // transform the pixel coordinate

		    newp.x = (x*dest2src[0][0] + y*dest2src[0][1] + dest2src[0][2]) /
		      (x*dest2src[2][0] + y*dest2src[2][1] + dest2src[2][2]);
		    newp.y = (x*dest2src[1][0] + y*dest2src[1][1] + dest2src[1][2]) /
		      (x*dest2src[2][0] + y*dest2src[2][1] + dest2src[2][2]);

		    double x0 = floor(newp.x);
		    double y0 = floor(newp.y);
		    int ix = (int)x0;
		    int iy = (int)y0;

		    if (ix<=maxsrcx && iy<=maxsrcy && ix>=minsrcx && iy>=minsrcy)
		      {
			Pixel input    = s [iy][ix];
			Pixel alphaval = al[iy][ix];

			d[y][x] = (input*alphaval + d[y][x]*(255-alphaval))/255;
		      }
		  }
	      }
	    else
	      {
		// === transform without alpha channel ===

		for (int x=xstart;x<=xend;x++)
		  {
		    // transform the pixel coordinate

		    newp.x = (x*dest2src[0][0] + y*dest2src[0][1] + dest2src[0][2]) /
		      (x*dest2src[2][0] + y*dest2src[2][1] + dest2src[2][2]);
		    newp.y = (x*dest2src[1][0] + y*dest2src[1][1] + dest2src[1][2]) /
		      (x*dest2src[2][0] + y*dest2src[2][1] + dest2src[2][2]);

		    double x0 = floor(newp.x);
		    double y0 = floor(newp.y);
		    int ix = (int)x0;
		    int iy = (int)y0;

		    if (ix<=maxsrcx && iy<=maxsrcy && ix>=minsrcx && iy>=minsrcy)
		      {
			d[y][x] = s[iy][ix];
		      }
		  }
	      }
	  }
      }

  }
}


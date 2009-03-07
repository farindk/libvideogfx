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
#include <limits.h>
// #include <gsl/gsl_linalg.h>
#include <algorithm>
#include <iomanip>
#include <math.h>

namespace videogfx {
  using namespace std;


  static Matrix4G ZorinMatrix(const Matrix4G* pts)
  {
    Matrix4G p1 = CrossProduct(CrossProduct(pts[0],pts[2])  , CrossProduct(pts[1],pts[3]));
    Matrix4G p2 = CrossProduct(CrossProduct(pts[0],pts[1])  , CrossProduct(pts[2],pts[3]));
    Matrix4G p3 = CrossProduct(CrossProduct(pts[0],pts[3])  , CrossProduct(pts[2],pts[1]));

    Matrix4G mat(3,3);
    for (int y=0;y<3;y++)
      {
	mat[y][0] = p1[y][0];
	mat[y][1] = p2[y][0];
	mat[y][2] = p3[y][0];
      }

    return mat;
  }


  Matrix4G PerspFrom4PointCorrespondences(const Point2D<double>* start,
					  const Point2D<double>* end)
  {

#if 0
    /*
      NOTE/TODO: This function seems to be wrong... It does not even work for the simplest case...
      It should be checked how the correct formulas look like, since it will be more efficient.

      This function is based on cut-and-paste code from the article:
      Jim Blinn: "Inferring Transforms", IEEE Computer Graphics and Applications, 1999
    */

    double U0 = start[0].x;
    double U1 = start[1].x;
    double U2 = start[2].x;
    double U3 = start[3].x;
    double V0 = start[0].y;
    double V1 = start[1].y;
    double V2 = start[2].y;
    double V3 = start[3].y;

    double X0 = end[0].x;
    double X1 = end[1].x;
    double X2 = end[2].x;
    double X3 = end[3].x;
    double Y0 = end[0].y;
    double Y1 = end[1].y;
    double Y2 = end[2].y;
    double Y3 = end[3].y;

    //Calculate elements of matrix Mst
    //From four coordinate pairs
    // (Ui Vi), (Zi Yi)
    double U03 = U0-U3; double V03 = V0-V3;
    double U13 = U1-U3; double V13 = V1-V3;
    double U23 = U2-U3; double V23 = V2-V3;
    double w0 = U13*V23 - U23*V13;
    double w1 = U23*V03 - U03*V23;
    double w2 = U03*V13 - U13*V03;
    double Sa00 = Y1-Y2; double Sa10 = X2-X1;
    double Sa01 = Y2-Y0; double Sa02 = Y0-Y1;
    double Sa11 = X0-X2; double Sa12 = X1-X0;
    double Sa20 = X1*Y2 - X2*Y1;
    double Sa21 = X2*Y0 - X0*Y2;
    double Sa22 = X0*Y1 - X1*Y0;
    double z1 = X3*Sa01 + Y3*Sa11 + Sa21;
    double z2 = X3*Sa02 + Y3*Sa12 + Sa22;
    double z0 = Sa20+Sa21+Sa22 - z1 - z2;
    double d0 = w0*z1*z2;
    double d1 = w1*z2*z0;
    double d2 = w2*z0*z1;
    Sa00*=d0; Sa10*=d0; Sa20*=d0;
    Sa01*=d1; Sa11*=d1; Sa21*=d1;
    Sa02*=d2; Sa12*=d2; Sa22*=d2;

    Matrix4G mat(3,3);

    mat[0][0] = Sa00*U0 + Sa01*U1 + Sa02*U2;
    mat[1][0] = Sa10*U0 + Sa11*U1 + Sa12*U2;
    mat[2][0] = Sa20*U0 + Sa21*U1 + Sa22*U2;
    mat[0][1] = Sa01*V0 + Sa01*V1 + Sa02*V2;
    mat[1][1] = Sa11*V0 + Sa11*V1 + Sa12*V2;
    mat[2][1] = Sa21*V0 + Sa21*V1 + Sa22*V2;
    mat[0][2] = Sa00 + Sa01 + Sa02;
    mat[1][2] = Sa10 + Sa11 + Sa12;
    mat[2][2] = Sa20 + Sa21 + Sa22;

    return mat;
#endif

    Matrix4G mats[4], mate[4];

    for (int i=0;i<4;i++)
      {
	mats[i] = Matrix4G(3,1);
	mate[i] = Matrix4G(3,1);

	mats[i][0][0] = start[i].x;
	mats[i][1][0] = start[i].y;
	mats[i][2][0] = 1.0;
	mate[i][0][0] = end[i].x;
	mate[i][1][0] = end[i].y;
	mate[i][2][0] = 1.0;
      }

    return ZorinMatrix(mate) * ZorinMatrix(mats).Inverse();
  }


  Matrix4G AffineFrom3PointCorrespondences(const Point2D<double>* start,
					   const Point2D<double>* end)
  {
    Matrix4G matx(3,3), maty(3,3);
    Matrix4G vecx(3,1), vecy(3,1);
    
    for (int i=0;i<3;i++)
      {
	matx[i][0] = start[i].x;
	matx[i][1] = start[i].y;
	matx[i][2] = 1;

	maty[i][0] = start[i].x;
	maty[i][1] = start[i].y;
	maty[i][2] = 1;

	vecx[i][0] = end[i].x;
	vecy[i][0] = end[i].y;
      }

    matx = matx.Inverse();
    maty = maty.Inverse();

    vecx = matx*vecx;
    vecy = maty*vecy;

    Matrix4G mat(3,3);
    mat[0][0] = vecx[0][0];
    mat[0][1] = vecx[1][0];
    mat[0][2] = vecx[2][0];
    mat[1][0] = vecy[0][0];
    mat[1][1] = vecy[1][0];
    mat[1][2] = vecy[2][0];

    return mat;
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

	int xmincut = INT_MAX;
	int xmaxcut = INT_MIN;
	int xcutidx=0;
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
		int cut = a*x0+(1.0-a)*x1;
		xmincut = min(xmincut, cut);
		xmaxcut = max(xmaxcut, cut);
		xcutidx++;
	      }
	  }

	if (xcutidx==0)
	  continue;

	int xstart = (int)xmincut - safety_border;
	int xend   = (int)xmaxcut + safety_border;

	if (xstart<0) xstart=0;  // TODO: this should be the left offset, not zero
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


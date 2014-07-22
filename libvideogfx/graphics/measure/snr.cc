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

#include "libvideogfx/graphics/measure/snr.hh"

#include <math.h>
#include <algorithm>
using namespace std;

namespace videogfx {

  double CalcMSE(const Bitmap<Pixel>& img1,
		 const Bitmap<Pixel>& img2,
		 int x0,int y0,int x1,int y1)
  {
    const Pixel*const* p1 = img1.AskFrame();
    const Pixel*const* p2 = img2.AskFrame();

    if (x1<0) x1 = img1.AskWidth()-1;
    if (y1<0) y1 = img1.AskHeight()-1;

    const int w = x1-x0+1;
    const int h = y1-y0+1;

    double errsum=0.0;

    for (int y=y0;y<=y1;y++)
      for (int x=x0;x<=x1;x++)
	{
	  double diff = p2[y][x]-p1[y][x];
	  errsum += diff*diff;
	}
    errsum /= w*h;

    return errsum;
  }


  double CalcSNR(const Bitmap<Pixel>& img1,
		 const Bitmap<Pixel>& img2,
		 int x0,int y0,int x1,int y1)
  {
    const Pixel*const* p1 = img1.AskFrame();
    const Pixel*const* p2 = img2.AskFrame();

    if (x1<0) x1 = img1.AskWidth()-1;
    if (y1<0) y1 = img1.AskHeight()-1;

    const int w = x1-x0+1;
    const int h = y1-y0+1;

    double mean_in=0.0,mean_diff=0.0;

    for (int y=y0;y<=y1;y++)
      for (int x=x0;x<=x1;x++)
	{
	  mean_in   += p1[y][x];
	  mean_diff += p2[y][x]-p1[y][x];
	}
    mean_in   /= w*h;
    mean_diff /= w*h;

    double var_in=0.0,var_diff=0.0;
    for (int y=y0;y<=y1;y++)
      for (int x=x0;x<=x1;x++)
	{
	  double t;
	  t = (p1[y][x]-mean_in);            var_in   += t*t;
	  t = (p2[y][x]-p1[y][x]-mean_diff); var_diff += t*t;
	}
    var_in   /= w*h;
    var_diff /= w*h;

    if (var_diff==0.0) return 9e+50;
    else return 10.0*log10(var_in/var_diff);
  }


  double CalcPSNR(const Bitmap<Pixel>& img1,
		  const Bitmap<Pixel>& img2,
		  int x0,int y0,int x1,int y1)
  {
    const double mse = CalcMSE(img1,img2,x0,y0,x1,y1);
    const double rmse = sqrt(mse);
    const double psnr = 20*log10(255.0/rmse);

    return psnr;
  }


  Bitmap<Pixel> CalcErrorMap(const Bitmap<Pixel>& img1,
                             const Bitmap<Pixel>& img2,
                             enum TransferCurve transfer_curve,
                             bool inverted)
  {
    int w = img1.AskWidth();
    int h = img1.AskHeight();

    const Pixel*const* p1 = img1.AskFrame();
    const Pixel*const* p2 = img2.AskFrame();

    Bitmap<Pixel> error;
    error.Create(w,h);

    Pixel*const* p = error.AskFrame();


    // --- prepare transfer curve ---

    Pixel transfer[255+1+255];
    for (int d=-255;d<=255;d++)
      {
        switch (transfer_curve)
          {
          case TransferCurve_Linear:
            transfer[d+255] = std::abs(d);
            break;

          case TransferCurve_Sqrt:
            transfer[d+255] = sqrt(std::abs(d)/255.0)*255;
            break;
          }

        if (inverted) transfer[d+255] = 255 - transfer[d+255];
      }


    // --- generate error map ---

    for (int y=0;y<h;y++)
      for (int x=0;x<w;x++)
        {
          p[y][x] = transfer[p1[y][x] - p2[y][x] + 255];
        }

    return error;
  }
}

/********************************************************************************
    LibVideoGfx - video processing library
    Copyright (C) 2002-2014  Dirk Farin

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

#include "libvideogfx/graphics/measure/ssim.hh"
#include "libvideogfx/graphics/filters/linear.hh"
#include "libvideogfx/containers/array.hh"

#include <math.h>
#include <algorithm>
using namespace std;


namespace videogfx {

  template <class T> T SQ(T t) { return t*t; }


  Bitmap<float> SSIM::calcSSIM(const Bitmap<Pixel>& img_x,
                               const Bitmap<Pixel>& img_y)
  {
    int w = img_x.AskWidth();
    int h = img_y.AskHeight();

    Array<double> filter;
    CreateGaussFilter(filter, sigma, 0.01);

    printf("filter size: %d\n",filter.AskSize());

    Bitmap<float> mean_x, mean_y;
    ConvolveHV(mean_x,img_x, filter);
    ConvolveHV(mean_y,img_y, filter);

    Bitmap<float> meandiff_x, meandiff_y;
    meandiff_x.Create(w,h);
    meandiff_y.Create(w,h);

    for (int y=0;y<h;y++)
      for (int x=0;x<w;x++) {
        meandiff_x[y][x] = SQ(mean_x[y][x] - img_x[y][x]);
        meandiff_y[y][x] = SQ(mean_y[y][x] - img_y[y][x]);
      }

    Bitmap<float> sigma_x, sigma_y;
    ConvolveHV(sigma_x,meandiff_x, filter);
    ConvolveHV(sigma_y,meandiff_y, filter);

    for (int y=0;y<h;y++)
      for (int x=0;x<w;x++) {
        sigma_x[y][x] = sqrt(sigma_x[y][x]);
        sigma_y[y][x] = sqrt(sigma_y[y][x]);
      }

    Bitmap<float> corr_tmp;
    corr_tmp.Create(w,h);
    for (int y=0;y<h;y++)
      for (int x=0;x<w;x++) {
        corr_tmp[y][x] = (img_x[y][x]-mean_x[y][x]) * (img_y[y][x]-mean_y[y][x]);
      }

    Bitmap<float> corr;
    ConvolveHV(corr, corr_tmp, filter);

    //return corr;

    Bitmap<float> ssim;
    ssim.Create(w,h);
    for (int y=0;y<h;y++)
      for (int x=0;x<w;x++) {
        float s = (2*mean_x[y][x]*mean_y[y][x] + C1) * (2*corr[y][x] + C2) /
          ( (mean_x [y][x]*mean_x [y][x] + mean_y [y][x]*mean_y [y][x] + C1 ) *
            (sigma_x[y][x]*sigma_x[y][x] + sigma_y[y][x]*sigma_y[y][x] + C2) );

        if (s<0.0) s = 0.0;
        if (s>1.0) s = 1.0;

        ssim[y][x] = s;
      }

    return ssim;
  }



  float SSIM::calcMSSIM(const Bitmap<Pixel>& img1,
                        const Bitmap<Pixel>& img2)
  {
    int w = img1.AskWidth();
    int h = img1.AskHeight();

    Bitmap<float> ssim = calcSSIM(img1,img2);

    double ssimSum = 0.0;

    for (int y=0;y<h;y++)
      for (int x=0;x<w;x++)
        {
          ssimSum += ssim[y][x];
        }

    ssimSum /= w*h;

    return ssimSum;
  }
}

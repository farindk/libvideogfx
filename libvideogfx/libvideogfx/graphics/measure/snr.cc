
#include "libvideogfx/graphics/measure/snr.hh"
#include <math.h>
#include <algorithm>
using namespace std;


double CalcMSE(const Bitmap<Pixel>& img1,
	       const Bitmap<Pixel>& img2,
	       int x0,int y0,int x1,int y1)
{
  const Pixel*const* p1 = img1.AskFrame();
  const Pixel*const* p2 = img2.AskFrame();

  if (x1<0) x1 = img1.AskLogicalWidth()-1;
  if (y1<0) y1 = img1.AskLogicalHeight()-1;

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

  if (x1<0) x1 = img1.AskLogicalWidth()-1;
  if (y1<0) y1 = img1.AskLogicalHeight()-1;

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


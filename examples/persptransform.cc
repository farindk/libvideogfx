/*
  Demonstration of perspective transform of images.
  The superimposed line shows the horizont line (the image of the line at infinity).
 */

#include "../libvideogfx/libvideogfx.hh"
#include <iostream>

#include <unistd.h>

using namespace videogfx;
using namespace std;


int main(int argc,char** argv)
{
  Image<Pixel> img;
  ReadImage_PPM(img, argv[1]);

  ImageWindow_Autorefresh_X11 win;
  win.Create(img.AskWidth(), img.AskHeight(), "output");

  Image<Pixel> disp;
  disp = img.Clone();

  for (double a=0.0; a<3*M_PI;a+=0.02)
    {
      // compose the transformation matrix by a concatenation of basic 3D-operations

      // 3D movement

      Matrix4G mat(4,4);
      mat = Translation3D(-img.AskWidth()/2,-img.AskHeight()/2,0);
      mat = CCWRotationZ(a/2,4)*mat;
      mat = CCWRotationX(a/4,4)*mat;
      mat = CCWRotationY(a  ,4)*mat;
      mat = Translation3D(0,0,2500)*mat;

      // projection onto 2D plane

      Matrix4G proj(3,4);
      proj[0][0] = 2000;
      proj[1][1] = 2000;
      proj[2][2] = 1;
      mat = proj*mat;
      mat.DeleteColumn(2);

      mat = Translation2D(img.AskWidth()/2,img.AskHeight()/2)*mat;
      mat = mat.Inverse();

      // draw the picture

      Clear(disp, Color<Pixel>(0,0,0));
      TransformBitmap(disp.AskBitmapY(), img.AskBitmapY(), mat, NULL, true);

      // draw horizont line

      Matrix4G hl(3,1);
      hl[0][0] = mat[2][0];
      hl[1][0] = mat[2][1];
      hl[2][0] = mat[2][2];

      Point2D<double> h[2];
      Line2PointPair(hl, h);

      DrawLine(disp.AskBitmapY(), h[0].x, h[0].y, h[1].x, h[1].y, (Pixel)250);


      win.Display(disp);
      //usleep(25000);
    }
}

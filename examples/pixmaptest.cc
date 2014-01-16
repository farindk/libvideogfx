
#include "../libvideogfx/types.hh"
#include "../libvideogfx/graphics/datatypes/bitmap.hh"

#include <unistd.h>
#include <iostream>
#include <iomanip>
using namespace std;
using namespace videogfx;


void Show(Bitmap<Pixel> pm, int x0=0,int y0=0)
{
  const Pixel*const* p = pm.AskFrame();

  for (int y=0;y<pm.AskHeight();y++,cout << endl)
    for (int x=0;x<pm.AskWidth();x++)
      cout << setw(2) << ((int)p[y-y0][x-x0]) << " ";
}


int main(int argc,char** argv)
{
  Bitmap<Pixel> pm(new BitmapProvider_Mem<Pixel>(20,11, 3));

  {
    Pixel*const* p = pm.AskFrame();

    for (int y=0;y<pm.AskHeight();y++)
      for (int x=0;x<pm.AskWidth();x++)
        p[y][x] = x+y;
  }

  Bitmap<Pixel> sub = pm.CreateSubView(3,5,6,4);
  Bitmap<Pixel> top = pm.CreateFieldView(true);
  Bitmap<Pixel> bot = pm.CreateFieldView(false);
  Bitmap<Pixel> sml = top.CreateSubView(2,1,5,2);

  cout << "sub-view tests:\n";

  Show(pm);  cout << pm.AskBorder() << endl;
  Show(sub); cout << sub.AskBorder() << endl;
  Show(top); cout << top.AskBorder() << endl;
  Show(sml); cout << sml.AskBorder() << endl;
  Show(bot); cout << bot.AskBorder() << endl;

  cout << "clone tests:\n";

  Bitmap<Pixel> copy = pm.Clone(5,16,16);
  Show(copy); cout << copy.AskBorder() << endl;

  copy = pm.Clone(1,16,16);
  Show(copy); cout << copy.AskBorder() << endl;

  copy = pm.Clone();
  Show(copy); cout << copy.AskBorder() << endl;

  cout << "zero shift:\n";
  pm.MoveZero(8,3);
  Show(pm, 8,3);

  cout << "copy of zero shifted:\n";
  copy = pm.Clone();
  Show(copy, 8,3);

  return 0;
}

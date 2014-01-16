
#include "../libvideogfx/libvideogfx.hh"
#include <iostream>

using namespace videogfx;
using namespace std;


void Show(const DynArray<int>& d)
{
  for (int i=0;i<d.AskSize();i++)
    cout << d[i] << ' ';
  cout << endl;
}

main()
{
  DynArray<int> dynarray;
  dynarray.SetEmptyValue(-1);

  cout << "Empty: "; Show(dynarray);

  dynarray[2]=1;

  cout << "2 := 1: "; Show(dynarray);

  dynarray.Append(3);

  cout << "append 3: "; Show(dynarray);

  dynarray[23]=23;

  cout << "23 := 23: "; Show(dynarray);
}


#include "../libvideogfx/libvideogfx.hh"
#include <iostream>

using namespace videogfx;
using namespace std;


void Show(const SymMatrix<int>& d)
{
  for (int i=0;i<d.AskSize();i++)
    {
      for (int j=0;j<d.AskSize();j++)
	cout << d.Ask(i,j) << ' ';
      cout << endl;
    }
}

main()
{
  SymMatrix<int> m;
  m.Create(6);

  for (int i=0;i<6;i++)
    for (int j=0;j<6;j++)
      {
	m.Ask(i,j) = i*6+j;
      }

  Show(m);
}

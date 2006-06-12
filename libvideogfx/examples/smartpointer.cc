
#include "../libvideogfx/libvideogfx.hh"

#include <iostream>
using namespace std;
using namespace videogfx;

class test
{
public:
  test(int v) { blubber=v; }

  int blubber;
};

int main()
{
  try
    {
      SP<test> v1 = new test(4711);
      SP<test> v2;
      v2 = v1;
      v2 = new test(123);

      v2().blubber = 321;

      cout << v1().blubber << endl;
      cout << v2().blubber << endl;

      swap(v1,v2);

      cout << v1().blubber << endl;
      cout << v2().blubber << endl;
    }
  catch(const Excpt_Base& e)
    {
      MessageDisplay::Show(e);
    }

  return 0;
}

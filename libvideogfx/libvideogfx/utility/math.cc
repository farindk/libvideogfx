
#include "libvideogfx/utility/math.hh"
#include <assert.h>
#include <iostream.h>
using namespace std;

namespace videogfx {

  int AlignUp(int val,int align)
  {
    assert(align>0);

    int newval;

    if (val%align)
      newval = val+align-(val%align);
    else
      newval = val;

    assert((newval % align)==0);
    assert(newval >= val);
    assert(newval < val+align);

    return newval;
  }


  // calculate least common multiple
  int LeastCommonMultiple(int a,int b)
  {
    int c=1;
    int i=2;

    while (a != 1 && b != 1)
      {
	while (a%i==0 || b%i==0)
	  {
	    if (a%i==0) a /= i;
	    if (b%i==0) b /= i;

	    c *= i;
	  }

	i++;
      }

    c *= a*b;

    return c;
  }

}

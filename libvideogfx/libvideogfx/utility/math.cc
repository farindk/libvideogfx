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

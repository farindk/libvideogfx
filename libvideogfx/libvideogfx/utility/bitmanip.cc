
#include "libvideogfx/utility/bitmanip.hh"
#include <assert.h>


void CalcBitsShift(uint32 mask,int& bits,int& shift)
{
  if (mask==0)
    { shift=0; bits=0; return; }

  shift=0;
  while ((mask&1)==0) { shift++; mask>>=1; }
  bits=0;
  while (mask&1) { bits++; mask>>=1; }

  assert(mask==0); /* This may fail if there are more than one continuous
		      sequences of ones (like 000111000011100). */
}

/*********************************************************************
  libvideogfx/utility/refcntr.hh

  purpose:
    Base class for reference-counted objects.

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
 *********************************************************************/

#ifndef LIBVIDEOGFX_UTILITY_REFCNTR_HH
#define LIBVIDEOGFX_UTILITY_REFCNTR_HH

#include <assert.h>


class ReferenceCntr
{
public:
  ReferenceCntr() : d_refcnt(0) { }
  virtual ~ReferenceCntr() { }

  void IncrRef() { d_refcnt++; }
  void DecrRef() { d_refcnt--; assert(d_refcnt>=0); }
  int  RefCntr() const { return d_refcnt; }

private:
  int d_refcnt;
};

#endif

/*********************************************************************
  libvideogfx/utility/refcntr.hh

  purpose:
    Base class for reference-counted objects.

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
 ********************************************************************************
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

#ifndef LIBVIDEOGFX_UTILITY_REFCNTR_HH
#define LIBVIDEOGFX_UTILITY_REFCNTR_HH

#include <assert.h>

namespace videogfx {

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

}

#endif

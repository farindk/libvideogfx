/********************************************************************************
  $Header$

  purpose:
    Generic smart pointer class.

    SP is a "smart pointer" class with reference counting. You can
    copy smart pointers or pass them to subroutines.

    SSP is a "simple smart pointer" which does not support copying.
    It is useful for automatic object destruction only.

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
   09/May/2002 - Dirk Farin
     - first implementation
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

#ifndef LIBVIDEOGFX_UTILITY_SMARTPOINTER_HH
#define LIBVIDEOGFX_UTILITY_SMARTPOINTER_HH

#include <libvideogfx/error.hh>

namespace videogfx {

  template <class T> class SP
  {
  public:
    SP()
    {
      counter = NULL;
      pointer = NULL;
    }

    SP(T* p)
    {
      counter = new long;
      *counter = 1;

      pointer = p;
    }

    SP(const SP<T>& s)
    {
      if (s.counter) (*s.counter)++;

      counter = s.counter;
      pointer = s.pointer;
    }

    ~SP()
    {
      Decouple();
    }

    SP<T>& operator=(const SP<T>& s)
    {
      if (s.counter) (*s.counter)++;
      Decouple();

      counter = s.counter;
      pointer = s.pointer;

      return *this;
    }

    SP<T>& operator=(T* p)
    {
      if (p==NULL)
	Decouple();
      else
	*this = SP<T>(p);

      return *this;
    }

    bool IsNULL() const { return counter==NULL; }
    operator T*()   { AssertDescr(counter,"smart pointer is NULL"); return pointer; }
    T& operator()() { AssertDescr(counter,"smart pointer is NULL"); return *pointer; }

    void Decouple()
    {
      if (counter)
	{
	  (*counter)--;

	  if (*counter==0)
	    {
	      delete pointer;
	      delete counter;
	    }

	  pointer=NULL;
	  counter=NULL;
	}
    }

  private:
    long* counter;
    T*    pointer;
  };


  // -------- simple smart pointer (without reference counting) -------------


  template <class T> class SSP
  {
  public:
    SSP()
    {
      pointer = NULL;
    }

    SSP(T* p)
    {
      pointer = p;
    }

    SSP(const SSP<T>& s)
    {
      Assert(0);
    }

    ~SSP()
    {
      if (pointer) delete pointer;
    }

    SSP<T>& operator=(const SSP<T>& s)
    {
      Assert(0);
    }

    SSP<T>& operator=(T* p)
    {
      if (pointer) delete pointer;
      pointer = p;

      return *this;
    }

    bool IsNULL() const { return pointer==NULL; }
    operator T*()   { AssertDescr(counter,"smart pointer is NULL"); return pointer; }
    T& operator()() { AssertDescr(counter,"smart pointer is NULL"); return *pointer; }

    void Decouple()
    {
      if (pointer)
	{
	  delete pointer;

	  pointer=NULL;
	}
    }

  private:
    T*    pointer;
  };

}

#endif

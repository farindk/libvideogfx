/********************************************************************************
  libvideogfx/containers/dynarray.hh

  purpose:
    Very simple dynamic array class.

  notes:

  to do:

  author(s):
   - Dirk Farin, farin@ti.uni-mannheim.de

  modifications:
   29/Aug/2001 - Dirk Farin
   - first implementation
 ********************************************************************************
    Copyright (C) 1999  Dirk Farin

    This program is distributed under GNU Public License (GPL) as
    outlined in the COPYING file that comes with the source distribution.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ********************************************************************************/

#ifndef LIBVIDEOGFX_CONTAINERS_DYNARRAY_HH
#define LIBVIDEOGFX_CONTAINERS_DYNARRAY_HH

#include "libvideogfx/types.hh"

namespace videogfx {

  template <class T> class DynArray
  {
  public:
    DynArray(int initial_size=100);
    DynArray(const DynArray<T>&);
    ~DynArray();

    void Append(const T& t);

    /* Append a new, empty element and return a reference to it. */
    T&   AppendNewEntry() { T t; Append(t); return d_array[d_nentries-1]; }

    /* Insert the element at the specified position. */
    void Insert(int pos,const T& t);
    T    RemoveEntry(int n);
    int  Size() const { return d_nentries; }

    T& operator[](int n)       { return d_array[n]; }
    const T& operator[](int n) const { return d_array[n]; }

    const DynArray<T> operator=(const DynArray<T>&);

  private:
    T*   d_array;
    int  d_nentries;
    int  d_size;

    void EnlargeIfFull();
  };


  template <class T> DynArray<T>::DynArray(int initial_size)
  {
    d_array = new T[initial_size];
    d_nentries=0;
    d_size=initial_size;
  }

  template <class T> DynArray<T>::DynArray(const DynArray<T>& t)
  {
    d_array = new T[t.d_size];
    d_size = t.d_size;
    for (int i=0;i<t.d_nentries;i++)
      d_array[i] = t.d_array[i];
    d_nentries=t.d_nentries;
  }

  template <class T> DynArray<T>::~DynArray()
  {
    delete[] d_array;
  }

  template <class T> void DynArray<T>::Append(const T& t)
  {
    EnlargeIfFull();

    d_array[d_nentries] = t;
    d_nentries++;
  }

  template <class T> void DynArray<T>::Insert(int pos,const T& t)
  {
    EnlargeIfFull();

    for (int i=d_nentries;i>pos;i--)
      d_array[i]=d_array[i-1];

    d_array[pos] = t;
    d_nentries++;
  }

  template <class T> void DynArray<T>::EnlargeIfFull()
  {
    if (d_nentries==d_size)
      {
	T* newlist = new T[d_size*2];
	for (int i=0;i<d_size;i++)
	  newlist[i] = d_array[i];

	delete[] d_array;

	d_array = newlist;
	d_size *= 2;
      }
  }


  template <class T> T DynArray<T>::RemoveEntry(int n)
  {
    T e = d_array[n];

    d_nentries--;
    for (int i=n;i<d_nentries;i++)
      d_array[i]=d_array[i+1];

    return e;
  }

  template <class T> const DynArray<T> DynArray<T>::operator=(const DynArray<T>& t)
  {
    if (&t==this)
      return *this;

    delete[] d_array;
    d_array = new T[t.d_size];
    d_size = t.d_size;
    for (int i=0;i<t.d_nentries;i++)
      d_array[i] = t.d_array[i];
    d_nentries=t.d_nentries;

    return *this;
  }

}

#endif

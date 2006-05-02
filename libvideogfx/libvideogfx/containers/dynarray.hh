/********************************************************************************
  libvideogfx/containers/dynarray.hh

  purpose:
    Very simple dynamic array class.

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
   29/Aug/2001 - Dirk Farin
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

#ifndef LIBVIDEOGFX_CONTAINERS_DYNARRAY_HH
#define LIBVIDEOGFX_CONTAINERS_DYNARRAY_HH

#include "libvideogfx/types.hh"
#include <assert.h>

namespace videogfx {

  template <class T> class DynArray
  {
  public:
    DynArray();
    DynArray(int initial_size);
    DynArray(const DynArray<T>&);
    ~DynArray();

    void Clear();
    void Append(const T& t);

    /* Append a new, empty element and return a reference to it. */
    T&   AppendNewEntry() { T t; Append(t); return d_array[d_nentries-1]; }

    /* Insert the element at the specified position. */
    void Insert(int pos,const T& t);
    T    ReturnAndRemoveEntry(int n);
    void RemoveEntry(int n);
    int  AskSize() const { return d_nentries; }

    void SetEmptyValue(const T& e) { d_empty_value=e; d_empty_val_set=true; }

    const T& operator[](int n) const { return d_array[n]; }
    T& operator[](int n)
    {
      EnlargeToSize(n+1);
      if (n>=d_nentries)
	d_nentries = n+1;
      return d_array[n];
    }

    const DynArray<T> operator=(const DynArray<T>&);

  private:
    T    d_empty_value;
    bool d_empty_val_set;

    T*   d_array;
    int  d_nentries;
    int  d_size;

    void EnlargeIfFull();
    void EnlargeToSize(int n);
  };


  template <class T> DynArray<T>::DynArray()
  {
    d_empty_val_set=false;
    d_array = NULL;
    d_nentries=0;
    d_size =0;
  }

  template <class T> DynArray<T>::DynArray(int initial_size)
  {
    d_empty_val_set=false;
    d_array = new T[initial_size];
    d_nentries=0;
    d_size=initial_size;
  }

  template <class T> DynArray<T>::DynArray(const DynArray<T>& t)
  {
    d_empty_val_set=t.d_empty_val_set;
    if (d_empty_val_set)
      d_empty_value = t.d_empty_value;

    d_array = new T[t.d_nentries];
    d_size = t.d_nentries;

    d_nentries=t.d_nentries;
    for (int i=0;i<t.d_nentries;i++)
      d_array[i] = t.d_array[i];
  }

  template <class T> DynArray<T>::~DynArray()
  {
    if (d_array) delete[] d_array;
  }

  template <class T> void DynArray<T>::Clear()
  {
    d_nentries=0;
    if (d_empty_val_set)
      {
	for (int i=0;i<d_size;i++)
	  d_array[i] = d_empty_value;
      }
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
      EnlargeToSize(d_nentries+1);
  }


  template <class T> void DynArray<T>::EnlargeToSize(int n)
  {
    if (n > d_size)
      {
	int newsize = (d_size > 10000 ? d_size+10000 : d_size*2);
	if (n>newsize) newsize=n;

	T* newlist = new T[newsize];
	for (int i=0;i<d_size;i++)
	  newlist[i] = d_array[i];

	if (d_empty_val_set)
	  {
	    for (int i=d_size;i<newsize;i++)
	      newlist[i] = d_empty_value;
	  }

	if (d_array) delete[] d_array;

	d_array = newlist;
	d_size = newsize;
      }
  }


  template <class T> T DynArray<T>::ReturnAndRemoveEntry(int n)
  {
    assert(n>=0 && n<d_nentries);

    T e = d_array[n];
    RemoveEntry(n);

    return e;
  }

  template <class T> void DynArray<T>::RemoveEntry(int n)
  {
    assert(n>=0 && n<d_nentries);

    d_nentries--;
    for (int i=n;i<d_nentries;i++)
      d_array[i]=d_array[i+1];

    if (d_empty_val_set)
      d_array[d_nentries] = d_empty_value;
  }

  template <class T> const DynArray<T> DynArray<T>::operator=(const DynArray<T>& t)
  {
    if (&t==this)
      return *this;

    delete[] d_array;
    d_array = new T[t.d_nentries];
    d_size = t.d_nentries;
    for (int i=0;i<t.d_nentries;i++)
      d_array[i] = t.d_array[i];
    d_nentries=t.d_nentries;

    return *this;
  }

}

#endif

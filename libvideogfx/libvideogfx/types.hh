/********************************************************************************
  types.hh

  purpose:
    Basic type declarations and commonly used macros, inline functions.

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
    03/Nov/1999 - Dirk Farin - New constants: BoolPixel_*
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

#ifndef LIBVIDEOGFX_TYPES_HH
#define LIBVIDEOGFX_TYPES_HH

namespace videogfx {

#ifdef _WIN32
  typedef unsigned _int64 uint64;
  typedef          _int64  int64;
#else
  typedef unsigned long long uint64;
  typedef   signed long long  int64;
#endif
  typedef unsigned long      uint32;
  typedef   signed long       int32;
  typedef unsigned short     uint16;
  typedef   signed short      int16;
  typedef unsigned char      uint8;
  typedef   signed char       int8;

  typedef uint32 uint31;
  typedef uint32 uint30;
  typedef uint32 uint29;
  typedef uint32 uint28;
  typedef uint32 uint27;
  typedef uint32 uint26;
  typedef uint32 uint25;
  typedef uint32 uint24;
  typedef uint32 uint23;
  typedef uint32 uint22;
  typedef uint32 uint21;
  typedef uint32 uint20;
  typedef uint32 uint19;
  typedef uint32 uint18;
  typedef uint32 uint17;

  typedef uint16 uint15;
  typedef uint16 uint14;
  typedef uint16 uint13;
  typedef uint16 uint12;
  typedef uint16 uint11;
  typedef uint16 uint10;
  typedef uint16 uint9;
  typedef int16 int15;
  typedef int16 int14;
  typedef int16 int13;
  typedef int16 int12;
  typedef int16 int11;
  typedef int16 int10;
  typedef int16 int9;

  typedef uint8 uint1;
  typedef uint8 uint2;
  typedef uint8 uint3;
  typedef uint8 uint4;
  typedef uint8 uint5;
  typedef uint8 uint6;
  typedef uint8 uint7;
  typedef  int8  int1;
  typedef  int8  int2;
  typedef  int8  int3;
  typedef  int8  int4;

  typedef uint8 Pixel;

  const Pixel BoolPixel_Set   = 255;
  const Pixel BoolPixel_Clear = 0;

  template <class T> inline T abs(T a) { if (a<0) return -a; else return a; }
  template <class T> inline int  sign(T x) { if (x<0) return -1; if (x>0) return 1; return 0; }

  inline uint32 RightBits(int n)  { return (((uint32)1)<<n)-1; }
  inline uint32 LeftBits32(int n) { return 0xFFFFFFFF - RightBits(32-n); }

  inline uint16 ChangeEndian(uint16 v) { return (v>>8) | (v<<8); }
  inline uint32 ChangeEndian(uint32 v) { return (v>>24) | ((v>>8)&0xFF00) | ((v&0xFF00)<<8) | (v<<24); }
}

#ifndef NULL
#define NULL 0
#endif

#endif

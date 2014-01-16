/********************************************************************************
  macros.hh

  purpose:
    Macros for library internal usage only.

  notes:
    This file does not get installed !

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de
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

#ifndef LIBVIDEOGFX_MACROS_HH
#define LIBVIDEOGFX_MACROS_HH

#include "config.h"
#include "libvideogfx/types.hh"

namespace videogfx {

#ifdef WORDS_BIGENDIAN
  inline uint16 ToLittleEndian(uint16 v) { return ChangeEndian(v); }
  inline uint32 ToLittleEndian(uint32 v) { return ChangeEndian(v); }
  inline uint16 ToBigEndian(uint16 v)    { return v; }
  inline uint32 ToBigEndian(uint32 v)    { return v; }
#else
  inline uint16 ToLittleEndian(uint16 v) { return v; }
  inline uint32 ToLittleEndian(uint32 v) { return v; }
  inline uint16 ToBigEndian(uint16 v)    { return ChangeEndian(v); }
  inline uint32 ToBigEndian(uint32 v)    { return ChangeEndian(v); }
#endif

}

#endif

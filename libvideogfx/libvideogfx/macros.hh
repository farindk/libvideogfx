/********************************************************************************
  macros.hh

  purpose:
    Macros for library internal usage only.

  notes:
    This file does not get installed !

  to do:

  author(s):
   - Dirk Farin, farin@ti.uni-mannheim.de
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

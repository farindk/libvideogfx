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

#include <libvideogfx/graphics/datatypes/bitmap.hh>

namespace videogfx {

  static int default_align_border=0;
  static int default_align_halign=1;
  static int default_align_valign=1;

  void SetAlignmentDefaults(int border,int halign,int valign)
  {
    assert(border>=0);
    assert(halign>=1);
    assert(valign>=1);

    default_align_border=border;
    default_align_halign=halign;
    default_align_valign=valign;
  }

  void AskAlignmentDefaults(int& border,int& halign,int& valign)
  {
    border = default_align_border;
    halign = default_align_halign;
    valign = default_align_valign;
  }

}

/*********************************************************************
  x11/draw_x11.hh

  purpose:
    Drawing routines that need X11 for rendering purpose.
    X11 is needed for example to get access to the fonts that
    are installed on the system.

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
    21/Jul/1999 - Dirk Farin - cleanup
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

#ifndef LIBVIDEOGFX_X11_DRAW_X11_HH
#define LIBVIDEOGFX_X11_DRAW_X11_HH

#include <libvideogfx/graphics/datatypes/image.hh>
#include <libvideogfx/graphics/datatypes/primitives.hh>

namespace videogfx {

  /* Draw text into the bitmap. */
  enum HTextAlign   { HAlign_Left,HAlign_Center,HAlign_Right  };
  enum VTextAlign   { VAlign_Top ,VAlign_Center,VAlign_Bottom };
  enum TextDrawMode { TextDrawMode_Transparent,TextDrawMode_Opaque };

  void WriteText_X11(Bitmap<Pixel>&,const char* txt,int x,int y,
		     Pixel front=255,Pixel back=0,
		     const char* x11fontname="9x15",
		     HTextAlign halign=HAlign_Left,VTextAlign valign=VAlign_Top,
		     TextDrawMode mode = TextDrawMode_Transparent);

  void WriteText_X11(Image<Pixel>&,const char* txt,int x,int y,
		     Color<Pixel> front=Color<Pixel>(255,255,255),Color<Pixel> back=Color<Pixel>(0,0,0),
		     const char* x11fontname="9x15",
		     HTextAlign halign=HAlign_Left,VTextAlign valign=VAlign_Top,
		     TextDrawMode mode = TextDrawMode_Transparent);
}

#endif

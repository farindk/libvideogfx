/*********************************************************************
  libvideogfx/x11/server.hh

  purpose:
    Get access to an X11 server. Create a object of type
    X11ServerConnection and keep it as long as you need access to
    the X11 server.
    Currently, only use the default constructor, which gives you
    a connection to the default X-server.

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

#ifndef LIBVIDEOGFX_X11_SERVER_HH
#define LIBVIDEOGFX_X11_SERVER_HH

#include <X11/Xlib.h>
#undef Bool

namespace videogfx {

  class X11Server;


  class X11ServerConnection
  {
  public:
    X11ServerConnection();            // connect to the default X-server
    X11ServerConnection(X11Server*);  // connect to a special X-server
    ~X11ServerConnection();

    Display* AskDisplay() const;

  private:
    X11Server* d_parent;

    static X11Server* s_default_server;
  };

}

#endif

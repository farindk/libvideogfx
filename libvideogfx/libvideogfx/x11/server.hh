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
   - Dirk Farin, farin@ti.uni-mannheim.de

  modifications:
 *********************************************************************/

#ifndef LIBVIDEOGFX_X11_SERVER_HH
#define LIBVIDEOGFX_X11_SERVER_HH

#include <X11/Xlib.h>

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

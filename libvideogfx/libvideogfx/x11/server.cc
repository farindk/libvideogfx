/*
 *  server.cc
 */

#include "config.h"

#include "libvideogfx/x11/server.hh"
#include "libvideogfx/utility/refcntr.hh"
#include "libvideogfx/error.hh"


class X11Server : public ReferenceCntr
{
public:
  virtual ~X11Server() { }

  virtual Display* AskDisplay() const = 0;
};


class X11Server_Default : public X11Server
{
public:
  X11Server_Default()
  {
    // open standard X-server connection

    d_display = XOpenDisplay(NULL);

    if (d_display==NULL)
      {
	throw Excpt_Text(ErrSev_Error,"cannot open X-server connection");
      }
  }

  ~X11Server_Default()
  {
    if (d_display) XCloseDisplay(d_display);
  }

  Display* AskDisplay() const { return d_display; }

private:
  Display* d_display;
};




X11ServerConnection::X11ServerConnection()
  : d_parent(NULL)
{
  // create a default X server connection if there is no default yet

  if (!s_default_server)
    s_default_server = new X11Server_Default;


  // set connection to default connection

  d_parent = s_default_server;
  d_parent->IncrRef();
}


X11ServerConnection::X11ServerConnection(X11Server* s)
  : d_parent(s)
{
  Assert(s);
  d_parent->IncrRef();
}


X11ServerConnection::~X11ServerConnection()
{
  if (d_parent==NULL)
    return;

  d_parent->DecrRef();
  if (d_parent->RefCntr()==0)
    {
      delete d_parent;

      /* If we destroyed the last connection to the default server,
	 also destroy the default connection. */

      if (d_parent == s_default_server)
	s_default_server=NULL;
    }
}


Display* X11ServerConnection::AskDisplay() const
{
  if (!d_parent)
    return NULL;

  return d_parent->AskDisplay();
}


X11Server* X11ServerConnection::s_default_server = NULL;



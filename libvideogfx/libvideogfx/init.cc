/*
 *  init.cc
 */

#include "init.hh"

namespace videogfx {

  char** glob_argv;
  int    glob_argc;

  void InitLibVideoGfx(int argc,char** argv)
  {
    glob_argc = argc;
    glob_argv = argv;
  }
}

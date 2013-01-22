#!/bin/sh
# Run this to generate all the initial makefiles, etc.

PROJECT=libvideogfx

if [ -x "`which autoreconf 2>/dev/null`" ] ; then
   exec autoreconf -ivf
fi

LIBTOOLIZE=libtoolize
SYSNAME=`uname`
if [ "x$SYSNAME" = "xDarwin" ] ; then
  LIBTOOLIZE=glibtoolize
fi
aclocal -I m4 && \
        autoheader && \
        $LIBTOOLIZE && \
        autoconf && \
        automake --add-missing --force-missing --copy

echo 
echo "Now type 'make' to compile $PROJECT."

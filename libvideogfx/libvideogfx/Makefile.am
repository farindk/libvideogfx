## Makefile.am for libvideogfx/libvideogfx

if HAVE_X11
SUBDIRS = arch containers graphics utility x11 # postscript
else
SUBDIRS = arch containers graphics utility # postscript
endif

INCLUDES = \
	-DDATADIR=\"$(datadir)\"	\
	-DLIBDIR=\"$(libdir)\"		\
	-DSYSCONFDIR=\"$(sysconfdir)\"	\
	-I$(top_srcdir)

#
# libraries to compile and install
#
lib_LTLIBRARIES = libvideogfx.la

# libtool stuff: set version and export symbols for resolving
libvideogfx_includedir = $(includedir)/libvideogfx
libvideogfx_la_LDFLAGS = \
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)	\
	-release $(LT_RELEASE)					\
	-export-dynamic

if HAVE_X11
x11_sources=x11/libvideogfx-x11.la
else
x11_sources=
endif


#
# setup sub-libraries variables
#
libvideogfx_la_LIBADD = \
	arch/libvideogfx-arch.la		\
	graphics/basic/libvideogfx-graphics-basic.la		\
	graphics/fileio/libvideogfx-graphics-fileio.la		\
	graphics/color/libvideogfx-graphics-color.la		\
	graphics/draw/libvideogfx-graphics-draw.la		\
	graphics/measure/libvideogfx-graphics-measure.la	\
	graphics/filters/libvideogfx-graphics-filters.la	\
	graphics/visualize/libvideogfx-graphics-visualize.la	\
	utility/bitstream/libvideogfx-utility-bitstream.la	\
	utility/libvideogfx-utility.la	\
	$(x11_sources)

#	graphics/datatypes/libvideogfx-graphics-datatypes.la	\
#	graphics/fileio/avi/libvideogfx-graphics-fileio-avi.la	\
#	graphics/lowlevel/libvideogfx-graphics-lowlevel.la	\
#	postscript/libvideogfx-postscript.la			\

#
# setup source file variables
#
# Header files for public installation (non-generated)
libvideogfx_public_h_sources = \
	init.hh		\
	types.hh	\
	error.hh	\
	libvideogfx.hh

# Header files that don't get installed
libvideogfx_private_h_sources = macros.hh

# C sources to build the library from
libvideogfx_c_sources = \
	init.cc	\
	error.cc

#
# setup sources and their dependancies
#
libvideogfx_include_HEADERS = $(libvideogfx_public_h_sources)
libvideogfx_la_SOURCES = $(libvideogfx_c_sources)
EXTRA_HEADERS +=
EXTRA_DIST += $(libvideogfx_private_h_sources)


## Makefile.am for libvideogfx/libvideogfx/graphics/fileio

# SUBDIRS = avi

noinst_LTLIBRARIES = libvideogfx-graphics-fileio.la

libvideogfx_graphics_fileio_includedir = \
	$(includedir)/libvideogfx/graphics/fileio

if LINUX
linux_sources = \
	v4l.cc	\
	v4l.hh

linux_headers = \
	v4l.hh
else
linux_sources =
linux_headers =
endif

libvideogfx_graphics_fileio_la_SOURCES = \
	jpeg.cc	\
	jpeg.hh	\
	mpeg.cc	\
	mpeg.hh	\
	uyvy.cc	\
	uyvy.hh	\
	ppm.cc	\
	ppm.hh  \
	yuv.cc	\
	yuv.hh	\
	yuv_vqeg.cc	\
	yuv_vqeg.hh	\
	imagesink.hh	\
	imagesink.cc	\
	audiosink.hh	\
	timedsink.hh	\
	$(linux_sources)

#	read_yuv.cc	\
#	read_yuv.hh	\
#	write_yuv.cc	\
#	write_yuv.hh	\

libvideogfx_graphics_fileio_include_HEADERS = \
        uyvy.hh	\
	jpeg.hh	\
	mpeg.hh	\
	ppm.hh  \
	yuv.hh	\
	yuv_vqeg.hh	\
	imagesink.hh	\
	audiosink.hh	\
	timedsink.hh	\
	$(linux_headers)

#	read_yuv.hh	\
#	write_yuv.hh	\

INCLUDES = \
	-I$(top_srcdir)


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
	png.cc	\
	png.hh	\
	mpeg.cc	\
	mpeg.hh	\
	uyvy.cc	\
	uyvy.hh	\
	ppm.cc	\
	ppm.hh  \
	yuv.cc	\
	yuv.hh	\
	yuv4mpeg.cc	\
	yuv4mpeg.hh	\
	yuv_vqeg.cc	\
	yuv_vqeg.hh	\
	imagesink.hh	\
	imagesink.cc	\
	$(linux_sources)

#	read_yuv.cc	\
#	read_yuv.hh	\
#	write_yuv.cc	\
#	write_yuv.hh	\

libvideogfx_graphics_fileio_include_HEADERS = \
        uyvy.hh	\
	jpeg.hh	\
	png.hh	\
	mpeg.hh	\
	ppm.hh  \
	yuv.hh	\
	yuv4mpeg.hh	\
	yuv_vqeg.hh	\
	imagesink.hh	\
	$(linux_headers)

#	read_yuv.hh	\
#	write_yuv.hh	\

INCLUDES = \
	-I$(top_srcdir)


## Makefile.am for libvideogfx/libvideogfx/graphics/fileio

# SUBDIRS = avi

noinst_LTLIBRARIES = libvideogfx-graphics-fileio.la

libvideogfx_graphics_fileio_includedir = \
	$(includedir)/libvideogfx/graphics/fileio

if LINUX
linux_sources = \
	v4l.cc	\
	v4l.hh	\
	mplayer.hh \
	mplayer.cc

linux_headers = \
	v4l.hh \
	mplayer.hh
else
linux_sources =
linux_headers =
endif

if HAVE_FFMPEG
ffmpeg_sources = ffmpeg.cc
ffmpeg_headers = ffmpeg.hh
else
ffmpeg_sources = 
ffmpeg_headers = 
endif

libvideogfx_graphics_fileio_la_SOURCES = \
	base.hh \
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
	raw.hh  \
	raw.cc  \
	yuv.cc	\
	yuv.hh	\
	yuv4mpeg.cc	\
	yuv4mpeg.hh	\
	yuv_vqeg.cc	\
	yuv_vqeg.hh	\
	imagesink.hh	\
	imagesink.cc	\
	unified_reader.hh \
	unified_reader.cc \
	unified_writer.hh \
	unified_writer.cc \
	$(linux_sources) \
	$(ffmpeg_sources)

libvideogfx_graphics_fileio_include_HEADERS = \
	base.hh \
        uyvy.hh	\
	jpeg.hh	\
	png.hh	\
	mpeg.hh	\
	ppm.hh  \
	raw.hh  \
	yuv.hh	\
	yuv4mpeg.hh	\
	yuv_vqeg.hh	\
	imagesink.hh	\
	unified_reader.hh \
	unified_writer.hh \
	$(linux_headers) \
	$(ffmpeg_headers)

INCLUDES = \
	-I$(top_srcdir)


## Makefile.am for libvideogfx/libvideogfx/audio/fileio

noinst_LTLIBRARIES = libvideogfx-audio-fileio.la

if LINUX
linux_files = audiosink_linux.cc
linux_headers = audiosink_linux.hh
else
linux_files =
linux_headers =
endif

libvideogfx_audio_fileio_includedir = \
	$(includedir)/libvideogfx/audio/fileio

libvideogfx_audio_fileio_la_SOURCES = \
	audiosink.hh	\
	audiosink.cc	\
	timedsink.hh	\
	$(linux_files)

libvideogfx_audio_fileio_include_HEADERS = \
	audiosink.hh	\
	timedsink.hh	\
	$(linux_headers)

AM_CPPFLAGS = \
	-I$(top_srcdir)


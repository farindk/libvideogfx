## Makefile.am for libvideogfx/libvideogfx/arch

noinst_LTLIBRARIES = libvideogfx-arch.la

libvideogfx_arch_la_SOURCES = \
	cpu.hh cpu.cc

libvideogfx_arch_includedir = \
	$(includedir)/libvideogfx/arch

if ENABLE_MMX
mmx_headers =	\
	mmx.h
else
mmx_header =
endif

libvideogfx_arch_include_HEADERS = \
	cpu.hh	\
	$(mmx_headers)

AM_CPPFLAGS = \
	-I$(top_srcdir)

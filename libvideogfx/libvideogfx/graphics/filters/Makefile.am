## Makefile.am for libvideogfx/libvideogfx/graphics/filters

noinst_LTLIBRARIES = \
	libvideogfx-graphics-filters.la

libvideogfx_graphics_filters_includedir = \
	$(includedir)/libvideogfx/graphics/filters

if ENABLE_MMX
mmx_sources =	gauss_mmx.cc \
		gauss_mmx.hh
mmx_headers = gauss_mmx.hh
else
mmx_sources =
mmx_headers =
endif

libvideogfx_graphics_filters_la_SOURCES = \
	linear.cc	\
	linear.hh	\
	binomial.cc	\
	binomial.hh	\
	$(mmx_sources)

libvideogfx_graphics_filters_include_HEADERS = \
	linear.hh	\
	binomial.hh	\
	$(mmx_headers)

INCLUDES = \
	-I$(top_srcdir)

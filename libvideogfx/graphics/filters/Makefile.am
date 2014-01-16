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
	median.cc	\
	median.hh	\
	binomial.cc	\
	binomial.hh	\
	scale.hh \
	scale.cc \
	resampler.cpp \
	resampler.h \
	$(mmx_sources)

libvideogfx_graphics_filters_include_HEADERS = \
	linear.hh	\
	median.hh	\
	binomial.hh	\
	scale.hh \
	resampler.h \
	$(mmx_headers)

INCLUDES = \
	-I$(top_srcdir)

## Makefile.am for libvideogfx/libvideogfx/graphics/color

noinst_LTLIBRARIES = libvideogfx-graphics-color.la

if ENABLE_MMX
mmx_sources =	\
	internal_grey2rgb_mmx.cc	\
	internal_grey2rgb_mmx.hh	\
	internal_yuv2rgb_mmx.cc	\
	internal_yuv2rgb_mmx.hh
else
mmx_sources =
endif

libvideogfx_graphics_color_la_SOURCES = \
	img2raw.cc	\
	img2raw.hh	\
	internal_grey2rgb_scalar.hh	\
	internal_grey2rgb_scalar.cc	\
	internal_yuv2rgb_scalar.hh	\
	internal_yuv2rgb_scalar.cc	\
	internal_rgb2rgb_scalar.hh	\
	internal_rgb2rgb_scalar.cc	\
	internal_img2raw.hh	\
	colorspace.cc	\
	colorspace.hh	\
	$(mmx_sources)


libvideogfx_graphics_color_includedir = \
	$(includedir)/libvideogfx/graphics/color

libvideogfx_graphics_color_include_HEADERS = \
	img2raw.hh	\
	colorspace.hh

AM_CPPFLAGS = \
	-I$(top_srcdir)

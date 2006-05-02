## Makefile.am for libvideogfx/examples

#
# test programs, not to be installed
#

if HAVE_X11
x11progs = x11test chromaspaces drawingtest copytest filedisplay imagetest viewfile median persptransform
else
x11progs =
endif

if LINUX
if HAVE_X11
linuxprogs = v4ltest v4ledges
endif
endif

if LINUX
audioprogs = audioout
endif

noinst_PROGRAMS = test-libvideogfx bitstream_output cputest pixmaptest filtertest \
	ppmflip chromatest smartpointer dynarray symmatrix \
	$(linuxprogs) $(audioprogs) $(x11progs) # ppm2avi

test_libvideogfx_DEPENDENCIES = ../libvideogfx/libvideogfx.la
test_libvideogfx_CFLAGS = $(X_CFLAGS)
test_libvideogfx_LDFLAGS = -g $(X_LDFLAGS)
test_libvideogfx_LDADD = ../libvideogfx/libvideogfx.la -lstdc++ $(X_LIBS) $(jpeg_libs) $(png_libs)
test_libvideogfx_SOURCES = test-libvideogfx.cc

dynarray_DEPENDENCIES = ../libvideogfx/libvideogfx.la
dynarray_CFLAGS = $(X_CFLAGS)
dynarray_LDFLAGS = -g $(X_LDFLAGS)
dynarray_LDADD = ../libvideogfx/libvideogfx.la -lstdc++ $(X_LIBS) $(jpeg_libs) $(png_libs)
dynarray_SOURCES = dynarray.cc

symmatrix_DEPENDENCIES = ../libvideogfx/libvideogfx.la
symmatrix_CFLAGS = $(X_CFLAGS)
symmatrix_LDFLAGS = -g $(X_LDFLAGS)
symmatrix_LDADD = ../libvideogfx/libvideogfx.la -lstdc++ $(X_LIBS) $(jpeg_libs) $(png_libs)
symmatrix_SOURCES = symmatrix.cc

bitstream_output_DEPENDENCIES = ../libvideogfx/libvideogfx.la
bitstream_output_CFLAGS = $(X_CFLAGS)
bitstream_output_LDFLAGS = -g $(X_LDFLAGS)
bitstream_output_LDADD = ../libvideogfx/libvideogfx.la -lstdc++ $(X_LIBS) $(jpeg_libs) $(png_libs)
bitstream_output_SOURCES = bitstream_output.cc

smartpointer_DEPENDENCIES = ../libvideogfx/libvideogfx.la
smartpointer_CFLAGS = $(X_CFLAGS)
smartpointer_LDFLAGS = -g $(X_LDFLAGS)
smartpointer_LDADD = ../libvideogfx/libvideogfx.la -lstdc++ $(X_LIBS) $(jpeg_libs) $(png_libs)
smartpointer_SOURCES = smartpointer.cc

#ppm2avi_DEPENDENCIES = ../libvideogfx/libvideogfx.la
#ppm2avi_CFLAGS = $(X_CFLAGS)
#ppm2avi_LDFLAGS = -g $(X_LDFLAGS)
#ppm2avi_LDADD = ../libvideogfx/libvideogfx.la -lstdc++ $(X_LIBS) $(jpeg_libs) $(png_libs)
#ppm2avi_SOURCES = ppm2avi.cc

cputest_DEPENDENCIES = ../libvideogfx/libvideogfx.la
cputest_CFLAGS = $(X_CFLAGS)
cputest_LDFLAGS = -g $(X_LDFLAGS)
cputest_LDADD = ../libvideogfx/libvideogfx.la -lstdc++ $(X_LIBS) $(jpeg_libs) $(png_libs)
cputest_SOURCES = cputest.cc

pixmaptest_DEPENDENCIES = ../libvideogfx/libvideogfx.la
pixmaptest_CFLAGS = $(X_CFLAGS)
pixmaptest_LDFLAGS = -g $(X_LDFLAGS)
pixmaptest_LDADD = ../libvideogfx/libvideogfx.la -lstdc++ $(X_LIBS) $(jpeg_libs) $(png_libs)
pixmaptest_SOURCES = pixmaptest.cc

filtertest_DEPENDENCIES = ../libvideogfx/libvideogfx.la
filtertest_CFLAGS = $(X_CFLAGS)
filtertest_LDFLAGS = -g $(X_LDFLAGS)
filtertest_LDADD = ../libvideogfx/libvideogfx.la -lstdc++ $(X_LIBS) $(jpeg_libs) $(png_libs)
filtertest_SOURCES = filtertest.cc

ppmflip_DEPENDENCIES = ../libvideogfx/libvideogfx.la
ppmflip_CFLAGS = $(X_CFLAGS)
ppmflip_LDFLAGS = -g $(X_LDFLAGS)
ppmflip_LDADD = ../libvideogfx/libvideogfx.la -lstdc++ $(X_LIBS) $(jpeg_libs) $(png_libs)
ppmflip_SOURCES = ppmflip.cc

chromatest_DEPENDENCIES = ../libvideogfx/libvideogfx.la
chromatest_CFLAGS = $(X_CFLAGS)
chromatest_LDFLAGS = -g $(X_LDFLAGS)
chromatest_LDADD = ../libvideogfx/libvideogfx.la -lstdc++ $(X_LIBS) $(jpeg_libs) $(png_libs)
chromatest_SOURCES = chromatest.cc

if HAVE_X11
x11test_DEPENDENCIES = ../libvideogfx/libvideogfx.la
x11test_CFLAGS = $(X_CFLAGS)
x11test_LDFLAGS = -g $(X_LDFLAGS)
x11test_LDADD = ../libvideogfx/libvideogfx.la -lstdc++ $(X_LIBS) $(jpeg_libs) $(png_libs)
x11test_SOURCES = x11test.cc

viewfile_DEPENDENCIES = ../libvideogfx/libvideogfx.la
viewfile_CFLAGS = $(X_CFLAGS)
viewfile_LDFLAGS = -g $(X_LDFLAGS)
viewfile_LDADD = ../libvideogfx/libvideogfx.la -lstdc++ $(X_LIBS) $(jpeg_libs) $(png_libs)
viewfile_SOURCES = viewfile.cc

persptransform_DEPENDENCIES = ../libvideogfx/libvideogfx.la
persptransform_CFLAGS = $(X_CFLAGS)
persptransform_LDFLAGS = -g $(X_LDFLAGS)
persptransform_LDADD = ../libvideogfx/libvideogfx.la -lstdc++ $(X_LIBS) $(jpeg_libs) $(png_libs)
persptransform_SOURCES = persptransform.cc

median_DEPENDENCIES = ../libvideogfx/libvideogfx.la
median_CFLAGS = $(X_CFLAGS)
median_LDFLAGS = -g $(X_LDFLAGS)
median_LDADD = ../libvideogfx/libvideogfx.la -lstdc++ $(X_LIBS) $(jpeg_libs) $(png_libs)
median_SOURCES = median.cc

filedisplay_DEPENDENCIES = ../libvideogfx/libvideogfx.la
filedisplay_CFLAGS = $(X_CFLAGS)
filedisplay_LDFLAGS = -g $(X_LDFLAGS)
filedisplay_LDADD = ../libvideogfx/libvideogfx.la -lstdc++ $(X_LIBS) $(jpeg_libs) $(png_libs)
filedisplay_SOURCES = filedisplay.cc

chromaspaces_DEPENDENCIES = ../libvideogfx/libvideogfx.la
chromaspaces_CFLAGS = $(X_CFLAGS)
chromaspaces_LDFLAGS = -g $(X_LDFLAGS)
chromaspaces_LDADD = ../libvideogfx/libvideogfx.la -lstdc++ $(X_LIBS) $(jpeg_libs) $(png_libs)
chromaspaces_SOURCES = chromaspaces.cc

imagetest_DEPENDENCIES = ../libvideogfx/libvideogfx.la
imagetest_CFLAGS = $(X_CFLAGS)
imagetest_LDFLAGS = -g $(X_LDFLAGS)
imagetest_LDADD = ../libvideogfx/libvideogfx.la -lstdc++ $(X_LIBS) $(jpeg_libs) $(png_libs)
imagetest_SOURCES = imagetest.cc

drawingtest_DEPENDENCIES = ../libvideogfx/libvideogfx.la
drawingtest_CFLAGS = $(X_CFLAGS)
drawingtest_LDFLAGS = -g $(X_LDFLAGS)
drawingtest_LDADD = ../libvideogfx/libvideogfx.la -lstdc++ $(X_LIBS) $(jpeg_libs) $(png_libs)
drawingtest_SOURCES = drawingtest.cc

copytest_DEPENDENCIES = ../libvideogfx/libvideogfx.la
copytest_CFLAGS = $(X_CFLAGS)
copytest_LDFLAGS = -g $(X_LDFLAGS)
copytest_LDADD = ../libvideogfx/libvideogfx.la -lstdc++ $(X_LIBS) $(jpeg_libs) $(png_libs)
copytest_SOURCES = copytest.cc
endif

audioout_DEPENDENCIES = ../libvideogfx/libvideogfx.la
audioout_CFLAGS = $(X_CFLAGS)
audioout_LDFLAGS = -g $(X_LDFLAGS)
audioout_LDADD = ../libvideogfx/libvideogfx.la -lstdc++ $(X_LIBS) $(jpeg_libs) $(png_libs)
audioout_SOURCES = audioout.cc

if LINUX
if HAVE_X11
v4ltest_DEPENDENCIES = ../libvideogfx/libvideogfx.la
v4ltest_CFLAGS = $(X_CFLAGS)
v4ltest_LDFLAGS = -g $(X_LDFLAGS)
v4ltest_LDADD = ../libvideogfx/libvideogfx.la -lstdc++ $(X_LIBS) $(jpeg_libs) $(png_libs)
v4ltest_SOURCES = v4ltest.cc

v4ledges_DEPENDENCIES = ../libvideogfx/libvideogfx.la
v4ledges_CFLAGS = $(X_CFLAGS)
v4ledges_LDFLAGS = -g $(X_LDFLAGS)
v4ledges_LDADD = ../libvideogfx/libvideogfx.la -lstdc++ $(X_LIBS) $(jpeg_libs) $(png_libs)
v4ledges_SOURCES = v4ledges.cc
endif
endif

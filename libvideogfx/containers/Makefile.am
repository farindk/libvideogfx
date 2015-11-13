## Makefile.am for libvideogfx/libvideogfx/containers

noinst_LTLIBRARIES = libvideogfx-containers.la

libvideogfx_containers_includedir =  \
	$(includedir)/libvideogfx/containers

libvideogfx_containers_la_SOURCES = \
	dummy.cc 	\
	heap.hh		\
	heap.icc	\
	dynarray.hh	\
	array.hh	\
	array.icc	\
	array2.hh	\
	array2.icc	\
	symmatrix.hh  \
	symmatrix.icc	\
	queue.hh	\
	queue.icc

# Header files for public installation (non-generated)
libvideogfx_containers_include_HEADERS = \
	heap.hh		\
	dynarray.hh	\
	array.hh	\
	array2.hh	\
	symmatrix.hh  \
	queue.hh

AM_CPPFLAGS = \
	-I$(top_srcdir)
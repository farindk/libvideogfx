## Makefile.am for libvideogfx/libvideogfx/containers

noinst_LTLIBRARIES = libvideogfx-containers.la

libvideogfx_containers_includedir = $(includedir)/libvideogfx/containers

# Header files for public installation (non-generated)
libvideogfx_containers_include_HEADERS = \
	heap.hh		\
	heap.icc	\
	dynarray.hh	\
	array.hh	\
	array.icc	\
	array2.hh	\
	array2.icc	\
	queue.hh	\
	queue.icc

libvideogfx_containers_la_SOURCES =

INCLUDES = \
	-I$(top_srcdir)

.PHONY: files

files:
	@files=`ls $(DISTFILES) 2> /dev/null`; for p in $$files; do \
	  echo $$p; \
	done


#ifndef LIBVIDEOGFX_LIBVIDEOGFX_HH
#define LIBVIDEOGFX_LIBVIDEOGFX_HH

#include <libvideogfx/types.hh>
#include <libvideogfx/init.hh>
#include <libvideogfx/error.hh>
#include <libvideogfx/arch/cpu.hh>
#include <libvideogfx/containers/array.hh>
#include <libvideogfx/containers/array2.hh>
#include <libvideogfx/containers/dynarray.hh>
#include <libvideogfx/containers/heap.hh>
#include <libvideogfx/containers/queue.hh>
#include <libvideogfx/graphics/basic/bitmap.hh>
#include <libvideogfx/graphics/basic/image.hh>
#include <libvideogfx/graphics/color/colorspace.hh>
#include <libvideogfx/graphics/color/img2raw.hh>
#include <libvideogfx/graphics/datatypes/primitives.hh>
#include <libvideogfx/graphics/draw/draw.hh>
#include <libvideogfx/graphics/draw/blit.hh>
#include <libvideogfx/graphics/draw/format.hh>
#include <libvideogfx/graphics/draw/pixelops.hh>
#include <libvideogfx/graphics/draw/scale.hh>
#include <libvideogfx/graphics/fileio/jpeg.hh>
#include <libvideogfx/graphics/fileio/mpeg.hh>
#include <libvideogfx/graphics/fileio/ppm.hh>
#include <libvideogfx/graphics/fileio/uyvy.hh>
#include <libvideogfx/graphics/fileio/yuv.hh>
#include <libvideogfx/graphics/fileio/yuv_vqeg.hh>
#include <libvideogfx/graphics/fileio/imagesink.hh>
//#include <libvideogfx/graphics/fileio/avi/aviformat.hh>
#include <libvideogfx/graphics/filters/linear.hh>
//#include <libvideogfx/graphics/filters/nonlin.hh>
#include <libvideogfx/graphics/measure/snr.hh>
//#include <libvideogfx/graphics/visualize/markblks.hh>
//#include <libvideogfx/graphics/visualize/motionfield.hh>
//#include <libvideogfx/graphics/visualize/regions.hh>
//#include <libvideogfx/postscript/image2ps.hh>
//#include <libvideogfx/postscript/layout.hh>
//#include <libvideogfx/postscript/mvfield.hh>
//#include <libvideogfx/postscript/regions.hh>
#include <libvideogfx/utility/bitmanip.hh>
#include <libvideogfx/utility/math.hh>
#include <libvideogfx/utility/refcntr.hh>
#include <libvideogfx/utility/bitstream/bitbuffer.hh>
#include <libvideogfx/utility/bitstream/bitreader.hh>
#include <libvideogfx/utility/bitstream/bitreader_fast.hh>

#ifdef HAVE_X11
#include <libvideogfx/x11/dispimg.hh>
#include <libvideogfx/x11/imgwin.hh>
#include <libvideogfx/x11/server.hh>
#include <libvideogfx/x11/draw_x11.hh>

// #include <libvideogfx/graphics/fileio/v4l_grab.hh>
// #include <libvideogfx/graphics/draw/draw_x11.hh>
#endif

#endif

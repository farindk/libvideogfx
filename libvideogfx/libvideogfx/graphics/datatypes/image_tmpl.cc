/*
 *  image_tmpl.cc
 */

#include "image.hh"
#include <assert.h>

template class Image<Pixel>;
template class Image<double>;
template class Image<int>;
template class Image<short>;

template class Bitmap<Pixel>;
template class Bitmap<bool>;
template class Bitmap<double>;
template class Bitmap<int>;
template class Bitmap<short>;



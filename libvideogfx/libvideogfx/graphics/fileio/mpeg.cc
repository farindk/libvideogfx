
#include "libvideogfx/graphics/fileio/mpeg.hh"
#include <stdlib.h>
#include <algorithm>
using namespace std;


FileReader_MPEG::FileReader_MPEG()
  : d_fh(NULL),
    d_next_framenr(0),
    d_image_cache_full(false)
{
}

FileReader_MPEG::~FileReader_MPEG()
{
  if (d_fh) pclose(d_fh);
}

void FileReader_MPEG::Open(const char* filename)
{
  if (d_fh) pclose(d_fh);

  char buf[100];
  sprintf(buf,"dvdview -L -W - %s",filename);
#ifdef _WIN32
  d_fh = popen(buf,"rb");
#else
  d_fh = popen(buf,"r");
#endif
}

bool FileReader_MPEG::IsEOF() const
{
  if (d_image_cache_full)
    return false;

  //d_image_cache_full = const_cast<FileReader_MPEG*>(this)->Preload(d_next_image_cache);
  d_image_cache_full = Preload(d_next_image_cache);

  return !d_image_cache_full;
}


void FileReader_MPEG::SkipToImage(int nr)
{
  AssertDescr(nr>=d_next_framenr,"cannot search backwards in MPEG stream (not implemented yet)");

  Image<Pixel> dummy;
  while (nr>d_next_framenr)
    ReadImage(dummy);
}

static int32 Read4(FILE* fh)
{
  int32 n=0;
  unsigned char c;
  fread(&c,1,1,fh); n |= ((int32)c)<<24;
  fread(&c,1,1,fh); n |= ((int32)c)<<16;
  fread(&c,1,1,fh); n |= ((int32)c)<<8;
  fread(&c,1,1,fh); n |= (int32)c;

  return n;
}


static int16 Read2(FILE* fh)
{
  int16 n=0;
  unsigned char c;
  fread(&c,1,1,fh); n |= ((int16)c)<<8;
  fread(&c,1,1,fh); n |= (int16)c;

  return n;
}

static void Skip(FILE* fh,int n)
{
  unsigned char c[100];
  while (n)
    {
      int nskip = min(100,n);
      fread(&c,nskip,1,fh);
      n-=nskip;
    }
}


bool FileReader_MPEG::ReadImage(Image<Pixel>& img)
{
  if (d_image_cache_full)
    {
      img = d_next_image_cache;
      d_image_cache_full = false;
      d_next_image_cache.Release();
      d_next_framenr++;

      return true;
    }
  else
    {
      bool image_read = Preload(img);

      return image_read;
    }
}

bool FileReader_MPEG::Preload(Image<Pixel>& dest) const
{
  int framenr;

  framenr=Read4(d_fh);
  int w = Read2(d_fh);
  int h = Read2(d_fh);

  Skip(d_fh,128-4-2-2);

  // cerr << "Nr: " << framenr << " width: " << spec.width << " height: " << spec.height << endl;

  if (feof(d_fh))
    return false;

  ImageParam spec = dest.AskParam();

  if (spec.width != w || spec.height != h ||
      spec.chroma != Chroma_420 ||
      spec.colorspace != Colorspace_YUV)
    {
      spec.chroma     = Chroma_420;
      spec.colorspace = Colorspace_YUV;
      spec.width =w;
      spec.height=h;

      dest.Create(spec);
    }

  Pixel*const* yp = dest.AskFrameY();
  Pixel*const* up = dest.AskFrameU();
  Pixel*const* vp = dest.AskFrameV();

  for (int y=0;y<h;y++)
    fread(yp[y],w,1,d_fh);

  int cw,ch;
  spec.AskChromaSizes(cw,ch);

  for (int y=0;y<ch;y++)
    fread(up[y],cw,1,d_fh);

  for (int y=0;y<ch;y++)
    fread(vp[y],cw,1,d_fh);

  return true;
}


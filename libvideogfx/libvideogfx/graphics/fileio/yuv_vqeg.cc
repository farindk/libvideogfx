
#include "yuv_vqeg.hh"

namespace videogfx {
  using namespace std;

  FileReader_YUV_VQEG::FileReader_YUV_VQEG()
    : d_yuvstr(NULL), d_mode625(false),
      d_nextframe(0)
  {
  }



  void FileReader_YUV_VQEG::SkipToImage(int nr)
  {
    assert(nr>=0);
    assert(nr<AskNFrames());

    int size;
    if (d_mode625) size = 829440; else size = 699840;

    d_yuvstr->seekg(nr*size,ios::beg);
    d_nextframe=nr;
  }



  void FileReader_YUV_VQEG::ReadImage(Image<Pixel>& img)
  {
    ImageParam spec;
    spec.width = 720;
    spec.height = (d_mode625 ? 576 : 486);
    spec.chroma = Chroma_422;
    spec.colorspace = Colorspace_YUV;

    img.Create(spec);

    Pixel*const* yp = img.AskFrameY();
    Pixel*const* up = img.AskFrameU();
    Pixel*const* vp = img.AskFrameV();

    char line[1440];

    for (int y=0;y<spec.height;y++)
      {
	d_yuvstr->read(line,1440);

	// TODO: 525: bottom field first
	//       625: top field first

	Pixel* ypp = yp[y];
	Pixel* upp = up[y];
	Pixel* vpp = vp[y];
	char*  lp  = line;

	for (int x=0;x<1440;x+=4)
	  {
	    *upp++ = *lp++;
	    *ypp++ = *lp++;
	    *vpp++ = *lp++;
	    *ypp++ = *lp++;
	  }
      }

    d_nextframe++;
  }

}


#include "libvideogfx/graphics/fileio/mplayer.hh"
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>


using namespace std;

namespace videogfx
{

  void FileReader_MPlayer::Open(const char* filedescr)
  {
    mkfifo("stream.yuv",0777);

    if (fork()==0)
      {
	//chdir("/tmp");
	//printf("----------------------------------- starting mplayer...\n");
	char buf[1000];
	sprintf(buf,"mplayer -fps 99999 -really-quiet -vo yuv4mpeg %s",filedescr);
	system(buf);
	//printf("----------------------------------- mplayer stopped...\n");
      }
    else
      {
	//printf("----------------------------------- starting reader...\n");

	d_istr.open("stream.yuv", std::ios::in | std::ios::binary);
	d_reader.SetYUVStream(d_istr);
      }
  }


  FileReader_MPlayer::~FileReader_MPlayer() { Close(); }

  void FileReader_MPlayer::Close()
  {
    unlink("stream.yuv");
  }


  bool FileReader_MPlayer::IsEOF() const
  {
    return d_reader.IsEOF();
  }


  void FileReader_MPlayer::ReadImage(Image<Pixel>& img)
  {
    d_reader.ReadImage(img);
  }


  void FileReader_MPlayer::SkipToImage(int nr)
  {
    d_reader.SkipToImage(nr);
  }

}

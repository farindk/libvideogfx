
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
    if (filedescr)
      {
	d_filedescr = new char[strlen(filedescr)+1];
	strcpy(d_filedescr, filedescr);
      }
    else
      filedescr = d_filedescr;


    mkfifo("stream.yuv",0777);

    pid_t pid;
    if ((pid=fork())==0)
      {
	//chdir("/tmp");
	//printf("----------------------------------- starting mplayer...\n");
	char buf[1000];
	sprintf(buf,"mplayer -fps 99999 -really-quiet -ao none -vo yuv4mpeg %s",filedescr);
	//cout << buf << endl;
	
	system(buf);
	//printf("----------------------------------- mplayer stopped...\n");
	exit(0);
      }
    else
      {
	mplayer_pid = pid;
	//printf("----------------------------------- starting reader...\n");

	d_istr.open("stream.yuv", std::ios::in | std::ios::binary);
	d_reader.SetYUVStream(d_istr);

	d_reader.ReadImage(d_preload);
      }
  }


  FileReader_MPlayer::~FileReader_MPlayer() { Close(); if (d_filedescr) delete[] d_filedescr; }

  void FileReader_MPlayer::Close()
  {
    if (mplayer_pid)
      {
	kill(mplayer_pid, SIGKILL);
	unlink("stream.yuv");
	mplayer_pid = 0;
      }
  }


  bool FileReader_MPlayer::IsEOF() const
  {
    if (!d_preload.IsEmpty()) return false;
    else return true;

    //return d_reader.IsEOF();
  }


  void FileReader_MPlayer::ReadImage(Image<Pixel>& img)
  {
    //cout << "mplayer: readimage / framenr=" << framenr << endl;

    if (!d_preload.IsEmpty())
      {
	img=d_preload;
	d_preload.Release();
	framenr++;
      }

    d_reader.ReadImage(d_preload);
  }


  void FileReader_MPlayer::SkipToImage(int nr)
  {
    //cout << "mplayer: skip " << nr << " (" << framenr << ")\n";

    if (nr==framenr)
      return;

#if 0
    if (nr<framenr)
      {
	cout << "reopen\n";
	d_reader.Reset();
	Close();
	Open(NULL); // reopen
	cout << "reopen end\n";
      }
#endif

    //    cout << "skip\n";
    d_reader.SkipToImage(nr);
    //    cout << "skip end\n";
    framenr=nr;

    d_reader.ReadImage(d_preload);
    
  }

}

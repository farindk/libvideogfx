
#ifndef FFMPEG_WRITER_HH
#define FFMPEG_WRITER_HH

#include <libvideogfx/graphics/datatypes/image.hh>
#include <vector>
#include <string>
#include <stdint.h>

#ifndef UINT64_C
#define UINT64_C(x) (x ## ULL)
#endif

#ifndef INT64_C
#define INT64_C(x) (x ## LL)
#endif

extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}


namespace videogfx
{

class FFMPEG_Writer
{
public:
  FFMPEG_Writer();
  ~FFMPEG_Writer();

  void Open(const char* filename, const char* format="avi");
  void Close();

  int AddVideoStream(int w,int h,float fps, int bitrate=1500000);
  int AddAudioStream(int samplerate,int nchannels);

  bool Start();
  void PushImage(const Image<Pixel>&, int channel=-1);
  void PushAudioSamples(int16* samples,int n, int channel=-1);

private:
  std::string mFilename;

  AVOutputFormat* fmt;
  AVFormatContext* oc;
  AVStream *audioStream;
  AVStream *videoStream;

  AVFrame *picture;
  AVFrame *tmp_picture;
  uint8_t *video_outbuf;
  int video_outbuf_size;
  struct SwsContext *img_convert_ctx;
};

}

#endif

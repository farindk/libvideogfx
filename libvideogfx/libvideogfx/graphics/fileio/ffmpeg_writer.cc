#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "ffmpeg_writer.hh"

#include <libvideogfx/graphics/color/colorspace.hh>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <iostream>

#ifndef UINT64_C
#define UINT64_C(x) (x ## ULL)
#endif

#ifndef INT64_C
#define INT64_C(x) (x ## LL)
#endif

extern "C" {
#ifdef HAVE_FFMPEG_AVUTIL_H
#include <ffmpeg/avutil.h>
#include <ffmpeg/mathematics.h>
#else
#include <libavutil/avutil.h>
#include <libavutil/mathematics.h>
#endif
#ifdef HAVE_FFMPEG_AVFORMAT_H
#include <ffmpeg/avformat.h>
#else
#include <libavformat/avformat.h>
#endif
#ifdef HAVE_FFMPEG_SWSCALE_H
#include <ffmpeg/swscale.h>
#else
#include <libswscale/swscale.h>
#endif
}

#if !defined(HAVE_AVMEDIA_TYPE_VIDEO)
# define AVMEDIA_TYPE_VIDEO CODEC_TYPE_VIDEO
# define AVMEDIA_TYPE_AUDIO CODEC_TYPE_AUDIO
# define AV_PKT_FLAG_KEY PKT_FLAG_KEY
# define AV_SAMPLE_FMT_S16 SAMPLE_FMT_S16
# define AVIO_FLAG_WRITE URL_WRONLY
#endif

#ifndef HAVE_AVIO_OPEN
#define avio_open url_fopen
#ifdef HAVE_AVFORMAT_CONTEXT_BYTEIOCONTEXT_POINTER
#  define avio_close(x) url_fclose(x)
#else
#  define avio_close(x) url_fclose(&(x))
#endif
#define av_dump_format dump_format
#define av_guess_format guess_format
#endif

#include <unistd.h>


namespace videogfx
{

static AVFrame *allocPicture(enum PixelFormat pix_fmt, int width, int height)
{
    AVFrame *picture;
    uint8_t *picture_buf;
    int size;

    picture = avcodec_alloc_frame();
    if (!picture)
        return NULL;
    size = avpicture_get_size(pix_fmt, width, height);
    picture_buf = (uint8_t*)av_malloc(size);
    if (!picture_buf) {
        av_free(picture);
        return NULL;
    }
    avpicture_fill((AVPicture *)picture, picture_buf,
                   pix_fmt, width, height);
    return picture;
}


static void fillImage(AVFrame *pict, int width, int height, const Image<Pixel>& yuv)
{
  for (int y=0;y<height;y++)
    memcpy(&pict->data[0][y * pict->linesize[0]], yuv.AskFrameY()[y], width);

  for (int y=0;y<height/2;y++)
    memcpy(&pict->data[1][y * pict->linesize[1]], yuv.AskFrameCb()[y], width/2);

  for (int y=0;y<height/2;y++)
    memcpy(&pict->data[2][y * pict->linesize[2]], yuv.AskFrameCr()[y], width/2);
}




FFMPEG_Writer::FFMPEG_Writer()
{
  av_register_all();

  img_convert_ctx = NULL;
}


FFMPEG_Writer::~FFMPEG_Writer()
{
  if (oc) { Close(); }
}

void FFMPEG_Writer::Open(const char* filename, const char* format)
{
  fmt = av_guess_format(format,NULL,NULL);
#ifdef HAVE_AVFORMAT_ALLOW_CONTEXT
  oc = avformat_alloc_context();
#else
  oc = av_alloc_format_context();
#endif
  oc->oformat = fmt;

  mFilename = filename;
}

void FFMPEG_Writer::Close()
{
  av_write_trailer(oc);
  avio_close(oc->pb);

  av_free(oc);

  oc=NULL;
}

int gcd(int a,int b)
{
  while (b!=0)
    {
      int h=a%b;
      a=b;
      b=h;
    }
  return a;
}

int FFMPEG_Writer::AddVideoStream(int w,int h,float fps, int bitrate)
{
  std::cout << "int FFMPEG_Writer::AddVideoStream(" << w << "," << h << "," << fps << "," << bitrate << ")\n";

  AVStream *st;
  st = videoStream = avformat_new_stream(oc, NULL);
  if (!st) {
    std::cerr << "Could not alloc ffmpeg video stream\n";
    return -1;
  }

  st->id = 0;
  int d = roundf(fps*100);
  int n = 100;
  int g = gcd(d,n);

  AVCodecContext *c;
  c = st->codec;
  c->codec_id = fmt->video_codec;
  c->codec_type = AVMEDIA_TYPE_VIDEO;
  c->bit_rate = bitrate;
  c->width  = w;
  c->height = h;
  c->time_base.den = d/g;
  c->time_base.num = n/g;
  c->gop_size = 25;
  c->pix_fmt = PIX_FMT_YUV420P;

  if(oc->oformat->flags & AVFMT_GLOBALHEADER)
    c->flags |= CODEC_FLAG_GLOBAL_HEADER;


  // openVideo()

  AVCodec *codec;

  codec = avcodec_find_encoder(c->codec_id);
  if (!codec) {
    std::cerr << "codec not found\n";
    return -1;
  }

  if (avcodec_open2(c, codec, NULL) < 0) {
    std::cerr << "could not open codec\n";
    return -1;
  }

  video_outbuf = NULL;
  if (!(oc->oformat->flags & AVFMT_RAWPICTURE)) {
    video_outbuf_size = 200000;
    video_outbuf = (uint8_t*)av_malloc(video_outbuf_size);
  }

  picture = allocPicture(c->pix_fmt, c->width, c->height);
  if (!picture) {
    std::cerr << "could not allocate picture\n";
    return -1;
  }

  tmp_picture = NULL;
  if (c->pix_fmt != PIX_FMT_YUV420P) {
    tmp_picture = allocPicture(PIX_FMT_YUV420P, c->width, c->height);
    if (!tmp_picture) {
      std::cerr << "could not allocate picture\n";
      return -1;
    }
  }

  return 0;
}

int FFMPEG_Writer::AddAudioStream(int samplerate,int nchannels, int bitrate)
{
  std::cout << "int FFMPEG_Writer::AddAudioStream(" << samplerate << "," << nchannels << "," << bitrate << ")\n";

  AVStream *st;
  st = audioStream = avformat_new_stream(oc, NULL);
  if (!st) {
    std::cerr << "could not alloc ffmpeg audio stream\n";
    return .1;
  }

  st->id = 1;
  AVCodecContext *c;
  c = st->codec;
  c->codec_id = CODEC_ID_MP3; //fmt->audio_codec;
  c->codec_type = AVMEDIA_TYPE_AUDIO;
  c->sample_fmt = AV_SAMPLE_FMT_S16;
  c->bit_rate = bitrate;
  c->sample_rate = samplerate;
  c->channels = nchannels;

  if(oc->oformat->flags & AVFMT_GLOBALHEADER)
    c->flags |= CODEC_FLAG_GLOBAL_HEADER;


  // openAudio()

  AVCodec *codec;

  codec = avcodec_find_encoder(c->codec_id);
  if (!codec) {
    std::cerr << "codec not found\n";
    exit(1);
  }

  if (avcodec_open2(c, codec, NULL) < 0) {
    std::cerr << "could not open codec\n";
    exit(1);
  }


  // alloc audio buffers

  audio_outbuf_size = FF_MIN_BUFFER_SIZE;
  audio_outbuf = (uint8_t*)av_malloc(audio_outbuf_size);
  audio_input_frame_size = c->frame_size;
    
  samples = (int16_t*)av_malloc(audio_input_frame_size * 2 * c->channels);
  nBufferedSamples=0;

  return 0;
}


bool FFMPEG_Writer::Start()
{
  std::cout << "bool FFMPEG_Writer::Start()\n";

  //av_set_parameters(oc, NULL);
  av_dump_format(oc, 0, mFilename.c_str(), 1);

  if (avio_open(&oc->pb, mFilename.c_str(), AVIO_FLAG_WRITE) < 0) {
    std::cerr << "Could not open '" << mFilename << "'\n";
    return false;
  }

#ifdef HAVE_AVFORMAT_AVFORMAT_WRITE_HEADER
  avformat_write_header(oc, NULL);
#else
  av_write_header(oc);
#endif
  return true;
}


void FFMPEG_Writer::PushImage(const Image<Pixel>& img, int channel)
{
  //std::cout << "void FFMPEG_Writer::PushImage(const Image<Pixel>& img, int channel)\n";

  const Image<Pixel> yuv = ChangeColorspace_NoCopy(img,
						   Colorspace_YUV,
						   Chroma_420);

  AVStream* st = videoStream;

  int out_size, ret;
  AVCodecContext *c;

  c = st->codec;

  if (c->pix_fmt != PIX_FMT_YUV420P) {
    if (img_convert_ctx == NULL) {
      img_convert_ctx = sws_getContext(c->width, c->height,
				       PIX_FMT_YUV420P,
				       c->width, c->height,
				       c->pix_fmt,
				       SWS_BICUBIC, NULL, NULL, NULL);
      if (img_convert_ctx == NULL) {
	std::cerr << "cannot initialize context\n";
	exit(1);
      }
    }
    fillImage(tmp_picture, c->width, c->height, yuv);
    sws_scale(img_convert_ctx, tmp_picture->data, tmp_picture->linesize,
	      0, c->height, picture->data, picture->linesize);
  } else {
    fillImage(picture, c->width, c->height, yuv);
  }


  if (oc->oformat->flags & AVFMT_RAWPICTURE) {
    AVPacket pkt;
    av_init_packet(&pkt);

    pkt.flags |= AV_PKT_FLAG_KEY;
    pkt.stream_index= st->index;
    pkt.data= (uint8_t *)picture;
    pkt.size= sizeof(AVPicture);

    ret = av_interleaved_write_frame(oc, &pkt);
  } else {
    out_size = avcodec_encode_video(c, video_outbuf, video_outbuf_size, picture);
    if (out_size > 0) {
      AVPacket pkt;
      av_init_packet(&pkt);

      if (c->coded_frame->pts != AV_NOPTS_VALUE)
	pkt.pts= av_rescale_q(c->coded_frame->pts, c->time_base, st->time_base);
      if(c->coded_frame->key_frame)
	pkt.flags |= AV_PKT_FLAG_KEY;
      pkt.stream_index= st->index;
      pkt.data= video_outbuf;
      pkt.size= out_size;

      ret = av_interleaved_write_frame(oc, &pkt);
    } else {
      ret = 0;
    }
  }
  if (ret != 0) {
    std::cerr << "error writing video\n";
    exit(1);
  }

}

void FFMPEG_Writer::PushAudioSamples(const int16* p,int n, int channel)
{
  //std::cout << "void FFMPEG_Writer::PushAudioSamples(const int16* p,int n, int channel)\n";

  //std::cout << "---\n";
  //std::cout << nBufferedSamples << " <+ " << n << "\n";

  // if there are still buffered samples, append some more until we have a complete audio frame

  if (nBufferedSamples>0)
    {
      int maxAdd = audio_input_frame_size-nBufferedSamples;
      int nAdd = std::min(maxAdd,n);

      memcpy(samples+nBufferedSamples, p, nAdd*2);
      p += nAdd;
      n -= nAdd;
      nBufferedSamples+=nAdd;

      //std::cout << "fill buffer " << nAdd << "\n";
    }

  //std::cout << nBufferedSamples << " <+ " << n << "\n";

  // if buffer is full, encode this frame

  if (nBufferedSamples==audio_input_frame_size)
    {
      encodeAudioFrame(samples);
      nBufferedSamples=0;
      //std::cout << "encode buffer\n";
    }

  //std::cout << nBufferedSamples << " <+ " << n << "\n";

  // encode more frames from input

  while (n>=audio_input_frame_size)
    {
      /*
      for (int i=0;i<n;i++)
        {
          std::cout << p[i] << " ";
          if ((i%32)==31) std::cout << "\n";
        }
      */

      encodeAudioFrame(p);
      p += audio_input_frame_size;
      n -= audio_input_frame_size;
      //std::cout << "encode input\n";
    }

  //std::cout << nBufferedSamples << " <+ " << n << "\n";

  // copy remaining samples into buffer

  if (n>0)
    {
      memcpy(samples, p, n*2);
      nBufferedSamples += n;
      //std::cout << "copy to buffer\n";
    }

  //std::cout << nBufferedSamples << " <+ " << n << "\n";
}


void FFMPEG_Writer::flushAudioBuffer()
{
  if (nBufferedSamples>0)
    {
      // fill will zeros

      memset(samples,0, (audio_input_frame_size-nBufferedSamples)*2);
      encodeAudioFrame(samples);

      nBufferedSamples=0;
    }
}

void FFMPEG_Writer::encodeAudioFrame(const int16* p)
{
    AVPacket pkt;
    av_init_packet(&pkt);

    AVCodecContext *c = audioStream->codec;
    
    pkt.size= avcodec_encode_audio(c, audio_outbuf, audio_outbuf_size, p);

    if (c->coded_frame && c->coded_frame->pts != AV_NOPTS_VALUE)
      pkt.pts= av_rescale_q(c->coded_frame->pts, c->time_base, audioStream->time_base);
    pkt.flags |= AV_PKT_FLAG_KEY;
    pkt.stream_index= audioStream->index;
    pkt.data= audio_outbuf;

    if (av_interleaved_write_frame(oc, &pkt) != 0) {
      std::cerr << "cannot write audio\n";
      assert(false);
    }
}











// ----------------------------------------------------------------------------------------------------


#if 0
int audio_samplePtr=0;
//SoundFile audio;


static void openAudio(AVFormatContext *oc, AVStream *st)
{
}



static bool fillAudio(int16_t *samples, int frame_size, int nb_channels)
{
  int maxLen = audio.getNFrames() - audio_samplePtr;
  int n = std::min(maxLen, frame_size);

  if (n==0) return false;

  audio.extractData(samples, n, audio_samplePtr);

  audio_samplePtr += n;

  return true;
}

static bool writeAudioFrame(AVFormatContext *oc, AVStream *st)
{
    AVCodecContext *c;
    AVPacket pkt;
    av_init_packet(&pkt);

    c = st->codec;
    
    bool cont = fillAudio(samples, audio_input_frame_size, c->channels);
    if (!cont) return false;

    pkt.size= avcodec_encode_audio(c, audio_outbuf, audio_outbuf_size, samples);

    if (c->coded_frame && c->coded_frame->pts != AV_NOPTS_VALUE)
        pkt.pts= av_rescale_q(c->coded_frame->pts, c->time_base, st->time_base);
    pkt.flags |= AV_PKT_FLAG_KEY;
    pkt.stream_index= st->index;
    pkt.data= audio_outbuf;

    if (av_interleaved_write_frame(oc, &pkt) != 0) {
      std::cerr << "cannot write audio\n";
      exit(1);
    }

    return true;
}
#endif


#if 0
const char* outputFile=0;
const char* slidesFile=0;
const char* audioFile =0;
const char* outputFormat="avi";

int main(int argc, char** argv)
{
  audio.open(audioFile);
  int sampleRate = audio.getSampleRate();


  openAudio(oc, audioStream);
  openVideo(oc, videoStream);


  for (;;)
    {
      double audio_pts, video_pts;

      audio_pts = (double)audioStream->pts.val * audioStream->time_base.num/audioStream->time_base.den;
      video_pts = (double)videoStream->pts.val * videoStream->time_base.num/videoStream->time_base.den;

      advanceSlides(video_pts);

      if (audio_pts < video_pts) {
	if (!writeAudioFrame(oc, audioStream))
	  break;
      }
      else
	writeVideoFrame(oc, videoStream);
    }


  return 0;
}

#endif

}

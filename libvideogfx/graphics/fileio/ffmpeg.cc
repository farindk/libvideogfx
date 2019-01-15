/********************************************************************************
  $Header$

    Read file using the FFMPEG library.
 ********************************************************************************
    Copyright (C) 2011 Dirk Farin (see the README for complete list of authors)

    This program is distributed under GNU Public License (GPL) as
    outlined in the COPYING file that comes with the source distribution.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ********************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libvideogfx/graphics/fileio/ffmpeg.hh"
//#include <iostream>

#ifndef UINT64_C
#define UINT64_C(x) (x ## ULL)
#endif

extern "C" {
#ifdef HAVE_FFMPEG_AVCODEC_H
#include <ffmpeg/avcodec.h>
#else
#include <libavcodec/avcodec.h>
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
#define AVMEDIA_TYPE_VIDEO CODEC_TYPE_VIDEO
#endif

using namespace std;

namespace videogfx
{
  FileReader_FFMPEG::FileReader_FFMPEG()
  {
    formatCtx = NULL;
    codecCtx  = NULL;
    codec     = NULL;
    frame     = NULL;
    frameRGB  = NULL;
    buffer    = NULL;
    m_eof = true;
    m_preloadFrameNr = -1;

    av_register_all();
  }

  FileReader_FFMPEG::~FileReader_FFMPEG()
  {
    Close();
  }


  bool FileReader_FFMPEG::Open(const char* filename)
  {
    Close();


    // open file

#ifdef HAVE_AVFORMAT_AVFORMAT_OPEN_INPUT
    if (avformat_open_input(&formatCtx, filename, NULL, NULL) != 0)
      return false;
#else
    if (av_open_input_file(&formatCtx, filename, NULL, 0, NULL) != 0)
      return false;
#endif

    if (avformat_find_stream_info(formatCtx, NULL)<0)
      return false;


    // find video stream

    videoStreamIdx=-1;
    for (int i=0;i< formatCtx->nb_streams; i++)
      if (formatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
	{
	  codecCtx = formatCtx->streams[i]->codec;

	  w = codecCtx->width;
	  h = codecCtx->height;
#ifdef HAVE_AVCODEC_TICKS_PER_FRAME
	  fps = float(codecCtx->time_base.den)/codecCtx->time_base.num / codecCtx->ticks_per_frame;
#else
          fps = float(codecCtx->time_base.den)/codecCtx->time_base.num;
#endif

	  videoStreamIdx=i;
	  break;
	}

    if (codecCtx==NULL)
      return false;


    // open decoder

    codec = avcodec_find_decoder(codecCtx->codec_id);
    if (codec==NULL)
      return false;

    if (avcodec_open2(codecCtx, codec, NULL)<0)
      return false;


    // alloc frame storage

    //frame = avcodec_alloc_frame();
    frame = av_frame_alloc();
    if (frame==NULL)
      return false;

    //frameRGB = avcodec_alloc_frame();
    frameRGB = av_frame_alloc();
    if (frameRGB==NULL)
      return false;

    //int nBytesPerFrame = avpicture_get_size(PIX_FMT_RGB24, codecCtx->width, codecCtx->height);
    int nBytesPerFrame = avpicture_get_size(AV_PIX_FMT_RGB24, codecCtx->width, codecCtx->height);
    buffer = static_cast<uint8_t*>(av_malloc(nBytesPerFrame));
    //avpicture_fill(reinterpret_cast<AVPicture*>(frameRGB), buffer, PIX_FMT_RGB24, codecCtx->width, codecCtx->height);
    avpicture_fill(reinterpret_cast<AVPicture*>(frameRGB), buffer, AV_PIX_FMT_RGB24, codecCtx->width, codecCtx->height);


    // preload first frame

    m_isInSync=false;
    m_eof=false;
    preload(0);

    return true;
  }


  void FileReader_FFMPEG::Close()
  {
    m_preloadFrameNr = -1;

    if (buffer)    { av_free(buffer); buffer=NULL; }
    if (frameRGB)  { av_free(frameRGB); frameRGB=NULL; }
    if (frame)     { av_free(frame); frame=NULL; }
    if (codecCtx)  { avcodec_close(codecCtx); codecCtx=NULL; }
    if (formatCtx) { avformat_close_input(&formatCtx); formatCtx=NULL; }
  }


  void FileReader_FFMPEG::preload(int frameNr)
  {
    assert(!m_eof);

    // read packet

    AVPacket packet;

    while (av_read_frame(formatCtx, &packet)==0)  // while OK
      {
	int frameFinished = 0;

        if (packet.stream_index == videoStreamIdx)
	  {
#ifdef HAVE_AVCODEC_DECODE_VIDEO2
            avcodec_decode_video2(codecCtx, frame, &frameFinished, &packet);
#else
            avcodec_decode_video(codecCtx, frame, &frameFinished, packet.data, packet.size);
#endif

            if (!m_isInSync && frame->key_frame)
	      m_isInSync=true;
	  }

        av_free_packet(&packet);


	if (m_isInSync && frameFinished)
	  {
	    m_preloadFrameNr = frameNr;
	    return;
	  }
      }

    m_eof = true;
    m_preloadFrameNr = -1;
  }


  bool FileReader_FFMPEG::ReadImage(Image<Pixel>& img)
  {
    if (m_eof) return false;

    assert(m_preloadFrameNr >= 0);
    convertRGBBuffer(img);

    preload(m_preloadFrameNr+1);
    return true;
  }


  void FileReader_FFMPEG::SkipToImage(int nr)
  {
    if (nr==m_preloadFrameNr)
      return;

    while (nr>m_preloadFrameNr)
      {
	preload(m_preloadFrameNr+1);
      }
  }


  void FileReader_FFMPEG::convertRGBBuffer(Image<Pixel>& img)
  {
    // convert to RGB

    struct SwsContext* swsContext
      = sws_getContext(codecCtx->width, codecCtx->height, codecCtx->pix_fmt,
		       //codecCtx->width, codecCtx->height, PIX_FMT_RGB24, SWS_POINT, NULL, NULL, NULL);
		       codecCtx->width, codecCtx->height, AV_PIX_FMT_RGB24, SWS_POINT, NULL, NULL, NULL);

    sws_scale(swsContext, frame->data, frame->linesize, 0, codecCtx->height,
	      frameRGB->data, frameRGB->linesize);

    sws_freeContext(swsContext);


    // convert format to libvideogfx images

    img.Create(w,h, Colorspace_RGB);

    for (int y=0;y<h;y++)
      {
	uint8* p = frameRGB->data[0]+y*frameRGB->linesize[0];
	Pixel* pr = img.AskFrameR()[y];
	Pixel* pg = img.AskFrameG()[y];
	Pixel* pb = img.AskFrameB()[y];

	for (int x=0;x<w;x++)
	  {
	    pr[x] = p[0];
	    pg[x] = p[1];
	    pb[x] = p[2];
	    p+=3;
	  }
      }
  }


}

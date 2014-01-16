/********************************************************************************
  $Header$

    MPEG-2 audio decoder
 ********************************************************************************
    LibVideoGfx - video processing library
    Copyright (C) 2002  Dirk Farin

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ********************************************************************************/

#ifndef LIBVIDEOGFX_AUDIO_FILEIO_AUDIOSINK_LINUX_HH
#define LIBVIDEOGFX_AUDIO_FILEIO_AUDIOSINK_LINUX_HH

#include <libvideogfx/audio/fileio/audiosink.hh>
#include <libvideogfx/audio/fileio/timedsink.hh>
#include <libvideogfx/utility/bitstream/bytebuffer.hh>
#include <libvideogfx/containers/queue.hh>


namespace videogfx {

  class AudioSink_LinuxSndCard : public TimedAudioSink
  {
  public:
    AudioSink_LinuxSndCard();
    ~AudioSink_LinuxSndCard();

    void       SetParam(const AudioParam& p);
    AudioParam AskParam() const;

    void SendTimestamp(Timestamp timestamp);
    void SendSamples(const int16* samples,int len);


    void  Reset();
    bool  PresentationDataPending() const;
    Timestamp NextDataPresentationTime() const;
    Timestamp LastDataPresentationTime() const;
    void  PresentData(Timestamp now);
    bool  IsRealtimeSink() const { return true; }

    bool  EnoughSpaceForMoreData() const;

    Timestamp GetCurrentTime() const;   // the timerID is not specified

  private:
    void Initialize();

    bool d_initialized;
    int  d_fd;
    int  d_blksize;

    int  d_channels;
    int  d_rate;
    int  d_bits_per_sample;
    int  d_sample_format;
    int  d_bytes_per_sec;

    ByteBuffer d_buffer; // more data to be played
    int64  d_total_bytes_sent_to_dma;

    struct ptspos
    {
      Timestamp pts;
      int64     bytenr;
    };

    Queue<ptspos> d_queue;
  };

}

#endif

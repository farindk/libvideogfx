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

  class AudioSink_LinuxSndCard : public AudioSink
  {
  public:
    AudioSink_LinuxSndCard();
    ~AudioSink_LinuxSndCard();

    void       SetParam(const AudioParam& p);
    AudioParam AskParam() const;

    void SendSamples(const int16* samples,int len,int64 timestamp);


    void  Reset();
    bool  PresentationDataPending() const;
    int64 NextDataPresentationTime();
    int64 LastDataPresentationTime();
    void  PresentData(int64 now);
    bool  IsRealtimeSink() const { return true; }

    int64 GetCurrentTime() const;

  private:
    void Initialize();

    bool d_initialized;
    int  d_fd;
    int  d_blksize;
    bool d_has_started;

    int  d_channels;
    int  d_rate;
    int  d_bits_per_sample;
    int  d_sample_format;
    int  d_bytes_per_sec;

    ByteBuffer d_buffer; // more data to be played
    int64  d_start_pts;  // pts of very first sample
    int64  d_last_pts;   // pts of very last sample so far
    int64  d_total_bytes_sent_to_dma;

    Queue<int64> d_queue_pts;
    Queue<int64> d_queue_bytenr;
  };

}

#endif

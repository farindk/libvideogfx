/********************************************************************************
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

#include "libvideogfx/audio/fileio/audiosink_linux.hh"
#include "libvideogfx/error.hh"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"

#include <sys/soundcard.h>



namespace videogfx {

  AudioSink_LinuxSndCard::AudioSink_LinuxSndCard()
  {
    d_initialized=false;
    d_blksize=0;

    AudioParam p;
    p.n_channels = 2;
    p.rate = 44100;
    p.bits_per_sample = 16;
    SetParam(p);

    Reset();
  }


  AudioSink_LinuxSndCard::~AudioSink_LinuxSndCard()
  {
    if (d_initialized)
      {
	close(d_fd);
      }
  }


  void AudioSink_LinuxSndCard::Initialize()
  {
    if (d_initialized)
      {
	close(d_fd);
      }

    const char* SOUND_DEVICE = "/dev/audio";
      
    if ( (d_fd = open(SOUND_DEVICE, O_WRONLY|O_NONBLOCK, 0)) == -1) { 
      perror("Cannot open sound device"); exit(0); 
    }

    if ( ioctl(d_fd, SNDCTL_DSP_SETFMT, &d_sample_format) == -1 ) perror("SNDCTL_DSP_SETFMT");
    if ( ioctl(d_fd, SNDCTL_DSP_CHANNELS, &d_channels) == -1 ) perror("SNDCTL_DSP_CHANNELS");
    if ( ioctl(d_fd, SNDCTL_DSP_SPEED, &d_rate) == -1 ) perror("SNDCTL_DSP_SPEED");

    switch (d_sample_format)
      {
      case AFMT_U8: d_bits_per_sample=8; break;
      case AFMT_S16_LE: d_bits_per_sample=16; break;
      case AFMT_S16_BE: d_bits_per_sample=16; break;
	/*
	  case AFMT_S32_LE: d_bits_per_sample=32; break;
	  case AFMT_S32_BE: d_bits_per_sample=32; break;
	  case AFMT_S32_NE: d_bits_per_sample=32; break;
	*/
      default:
	throw Excpt_Text(ErrSev_Error,"unsupported audio sample format");
	break;
      }

    d_initialized=true;
  }

  void       AudioSink_LinuxSndCard::SetParam(const AudioParam& p)
  {
    bool changed=false;
    if (p.n_channels != d_channels) { changed=true; d_channels=p.n_channels; }
    if (p.rate != d_rate) { changed=true; d_rate=p.rate; }
    if (p.bits_per_sample != d_bits_per_sample)
      {
	changed=true;
	d_bits_per_sample = p.bits_per_sample;
	d_sample_format = AFMT_S16_NE;
      }

    if (changed)
      Initialize();

    d_bytes_per_sec = d_channels*d_rate*d_bits_per_sample/8;
  }

  AudioParam AudioSink_LinuxSndCard::AskParam() const
  {
    AudioParam p;
    p.n_channels = d_channels;
    p.rate = d_rate;
    p.bits_per_sample = d_bits_per_sample;

    return p;
  }

  void AudioSink_LinuxSndCard::SendTimestamp(Timestamp timestamp)
  {
    // put timestamp into queue

    struct ptspos p;
    p.pts = timestamp;
    p.bytenr = (d_total_bytes_sent_to_dma + d_buffer.AskLength());
    d_queue.Append(p);
  }


  void AudioSink_LinuxSndCard::SendSamples(const int16* samples,int len)
  {
#if 0
    Assert(d_initialized);

    if (d_sample_format != AFMT_S16_NE)
      {
	AssertDescr(0,"non-native sample formats not supported yet");
      }
#endif

    uint8* buf = d_buffer.GetPtrToAppendToBuffer(len*2);

    for (int i=0;i<len;i++)
      {
	buf[1] = samples[i]>>8;
	buf[0] = samples[i]&0xFF;
	buf+= 2;
      }
  }


  void  AudioSink_LinuxSndCard::Reset()
  {
    d_buffer.Clear();

    d_total_bytes_sent_to_dma = 0;

    d_queue.Clear();
  }

  bool  AudioSink_LinuxSndCard::PresentationDataPending() const
  {
    return d_buffer.AskLength() > 0;
  }

  Timestamp AudioSink_LinuxSndCard::NextDataPresentationTime() const
  {
    Assert(d_initialized);

    if (d_queue.IsEmpty())
      {
	// unknown timestamps

	Timestamp ts;
	ts.timestamp=0;
	ts.timerID=-1;
	return ts;
      }

    Timestamp nextpts;
    nextpts = d_queue.AskHead().pts;
    nextpts.timestamp += (d_total_bytes_sent_to_dma-d_queue.AskHead().bytenr)*1000 / d_bytes_per_sec;

    nextpts.timestamp -= 100; // send data 1 second earlier to prevent short pauses in the audio

    return nextpts;
  }

  Timestamp AudioSink_LinuxSndCard::LastDataPresentationTime() const
  {
    Timestamp lastpts;
    lastpts = d_queue.AskTail().pts;
    lastpts.timestamp += (d_buffer.AskLength()+d_total_bytes_sent_to_dma - d_queue.AskTail().bytenr)*1000 / d_bytes_per_sec;

    return lastpts;
  }

  void  AudioSink_LinuxSndCard::PresentData(Timestamp now)
  {
    if (now.timestamp > NextDataPresentationTime().timestamp+500)
      {
	// We are late. Skip audio data to get on sync again.

	int truncatelength = (now.timestamp - NextDataPresentationTime().timestamp)*d_bytes_per_sec/1000;
	if (truncatelength>d_buffer.AskLength()) truncatelength = d_buffer.AskLength();
	truncatelength &= ~3;

	cout << "truncate: " << truncatelength << endl;
	d_total_bytes_sent_to_dma += truncatelength;
	d_buffer.TruncateBufferAtFront(truncatelength);
      }

    if (d_buffer.AskLength()>0)
      {
	int written = write(d_fd, d_buffer.AskData(), d_buffer.AskLength());

	if (written<0)
	  {
	  }
	else
	  {
	    d_total_bytes_sent_to_dma += written;
	    d_buffer.TruncateBufferAtFront(written);
	  }
      }

    while (!d_queue.IsEmpty() && d_total_bytes_sent_to_dma > d_queue.AskHead().bytenr)
      {
	// If there is more data in our user buffer, but no PTS, generate a dummy PTS.

	if (d_queue.AskSize()==1)
	  {
	    struct ptspos p;
	    p.bytenr = d_total_bytes_sent_to_dma + d_buffer.AskLength();
	    p.pts    = LastDataPresentationTime();
	    d_queue.Append(p);
	  }

	d_queue.RemoveHead();
      }
  }

  bool AudioSink_LinuxSndCard::EnoughSpaceForMoreData() const
  {
    if (LastDataPresentationTime().timestamp - NextDataPresentationTime().timestamp > 5000)
      return false;
    else
      return true;
  }

  Timestamp AudioSink_LinuxSndCard::GetCurrentTime() const
  {
    Timestamp ts;

    if (!d_initialized)
      {
	ts.timerID = -1;
	ts.timestamp = -1;
	return ts;
      }

    audio_buf_info info;
    if ( ioctl(d_fd, SNDCTL_DSP_GETOSPACE, &info) == -1 ) perror("SNDCTL_DSP_GETOSPACE");

    int bytes_in_buffer = info.fragstotal*info.fragsize - info.bytes;

    ts.timestamp = (d_total_bytes_sent_to_dma-bytes_in_buffer)*1000 / d_bytes_per_sec;
    ts.timerID = 0;

    return ts;
  }

}

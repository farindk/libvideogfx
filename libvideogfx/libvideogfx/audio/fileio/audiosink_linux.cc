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

  void AudioSink_LinuxSndCard::SendSamples(const int16* samples,int len,int64 timestamp)
  {
    audio_buf_info info;

    if (!d_initialized)
      Initialize();

    if (!d_initialized)
      return;

    if (d_start_pts<0)
      d_start_pts = timestamp;

    d_last_pts = timestamp;

    if (d_sample_format != AFMT_S16_NE)
      {
	AssertDescr(0,"non-native sample formats not supported yet");
      }


    // put timestamp into queue

    d_queue_pts.Append(timestamp);
    d_queue_bytenr.Append(d_total_bytes_sent_to_dma + d_buffer.AskLength());

    // PresentData(0);  // play data in buffer

    int written=0;
#if 0
    if (d_has_started && d_buffer.AskLength()==0)
      {
	written = write(d_fd, (char*)samples, len*2)/2;
      }
#endif

    if (written < len)
      {
	uint8* buf = d_buffer.GetPtrToAppendToBuffer((len-written)*2);

	for (int i=written;i<len;i++)
	  {
	    buf[1] = samples[i]>>8;
	    buf[0] = samples[i]&0xFF;
	    buf+= 2;
	  }
      }
  }


  void  AudioSink_LinuxSndCard::Reset()
  {
    d_buffer.Clear();

    d_start_pts = -1;
    d_last_pts = 0;
    d_has_started = false;

    d_total_bytes_sent_to_dma = 0;
    d_queue_pts.Clear();
    d_queue_bytenr.Clear();
  }

  bool  AudioSink_LinuxSndCard::PresentationDataPending() const
  {
    return d_buffer.AskLength() > 0;
  }

  int64 AudioSink_LinuxSndCard::NextDataPresentationTime()
  {
    if (d_initialized)
      {
	if (d_queue_pts.IsEmpty())
	  return d_last_pts;

	int64 nextpts = d_queue_pts.AskHead() + (d_total_bytes_sent_to_dma-d_queue_bytenr.AskHead())*1000 / d_bytes_per_sec;

	return nextpts; // - 1000;
      }
    else
      return 0; //d_start_pts;
  }

  int64 AudioSink_LinuxSndCard::LastDataPresentationTime()
  {
    return d_last_pts;
  }

  void  AudioSink_LinuxSndCard::PresentData(int64 now)
  {
    d_has_started=true;

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

	    if (!d_queue_bytenr.IsEmpty() && d_total_bytes_sent_to_dma >= d_queue_bytenr.AskHead())
	      {
		d_queue_bytenr.RemoveHead();
		d_queue_pts.RemoveHead();


		// If there is more data in our user buffer, but no PTS, generate a dummy PTS.

		if (d_queue_pts.IsEmpty())
		  {
		    d_queue_bytenr.Append(d_total_bytes_sent_to_dma);
		    d_queue_pts.Append(d_start_pts + d_total_bytes_sent_to_dma*1000 / d_bytes_per_sec);
		  }
	      }
	  }
      }
  }

  int64 AudioSink_LinuxSndCard::GetCurrentTime() const
  {
    if (!d_initialized)
      return 0;

    audio_buf_info info;
    if ( ioctl(d_fd, SNDCTL_DSP_GETOSPACE, &info) == -1 ) perror("SNDCTL_DSP_GETOSPACE");

    int bytes_in_buffer = info.fragstotal*info.fragsize - info.bytes;

    int64 pts = d_start_pts + (d_total_bytes_sent_to_dma-bytes_in_buffer)*1000 / d_bytes_per_sec;

    return pts;
  }

}

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
  }


  AudioSink_LinuxSndCard::~AudioSink_LinuxSndCard()
  {
    if (d_initialized)
      {
	close(d_fd);
      }
  }


  int AudioSink_LinuxSndCard::SendSamples(int16* left,int16* right,int len)
  {
    if (!d_initialized)
      {
	const char* SOUND_DEVICE = "/dev/dsp";
      
	if ( (d_fd = open(SOUND_DEVICE, O_WRONLY|O_NONBLOCK, 0)) == -1) { 
	  perror("Cannot open sound device"); exit(0); 
	}

	int format   = AFMT_S16_LE;
	int channels = 2; //pcm->channels;
	int rate     = 44100; //pcm->samplerate;
	//printf("Set %d channels at rate %d\n", channels, rate);

	if ( ioctl(d_fd, SNDCTL_DSP_SETFMT, &format) == -1 ) perror("SNDCTL_DSP_SETFMT");
	if ( ioctl(d_fd, SNDCTL_DSP_CHANNELS, &channels) == -1 ) perror("SNDCTL_DSP_CHANNELS");
	if ( ioctl(d_fd, SNDCTL_DSP_SPEED, &rate) == -1 ) perror("SNDCTL_DSP_SPEED");

	d_initialized=true;
      }

    char buf[2000*2*2];
    int  idx=0;

    int total_written = 0;

    for (int i=0;i<len;i++)
      {
	*((int16*)(&buf[idx])) = left[i];
	idx+=2;
	*((int16*)(&buf[idx])) = right[i];
	idx+=2;

	if (idx==2000*2*2)
	  {
	    int written = write(d_fd, buf, 2000*2*2);
	    total_written += written;
	    idx=0;

	    if (written<2000*2*2)
	      return total_written/4;
	  }
      }

    if (idx>0)
      {
	int written = write(d_fd, buf, idx);
	total_written += written;
      }

    return total_written/4;
  }

}

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

namespace videogfx {

  class AudioSink_LinuxSndCard : public AudioSink
  {
  public:
    AudioSink_LinuxSndCard();
    ~AudioSink_LinuxSndCard();

    void SendSamples(int16* left,int16* right,int len);

  private:
    bool d_initialized;
    int  d_fd;
  };

}

#endif

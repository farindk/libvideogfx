/********************************************************************************
  audio/fileio/audiosink.hh
    Generic audio data sink.

  purpose:

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
   14/Apr/2002 - Dirk Farin
     - new interface definition
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

#ifndef LIBVIDEOGFX_AUDIO_FILEIO_AUDIOSINK_HH
#define LIBVIDEOGFX_AUDIO_FILEIO_AUDIOSINK_HH

#include <libvideogfx/types.hh>
#include <iostream>

namespace videogfx {

  struct AudioParam
  {
    AudioParam() : n_channels(2), rate(44100), bits_per_sample(16), little_endian(true) { }

    int  n_channels;
    int  rate;
    int  bits_per_sample;
    bool little_endian;
  };


  class AudioSink
  {
  public:
    virtual ~AudioSink() { }

    virtual void SetParam(const AudioParam& p) { }
    virtual AudioParam AskParam() const { return AudioParam(); }

    virtual int  SendSamples(int16* left,int16* right,int len) = 0;
    virtual int  AskBufferingDelay() const { return 0; } // buffering delay in msecs
  };


  class AudioSink_cout : public AudioSink
  {
  public:
    int SendSamples(int16* left,int16* right,int len)
    {
      for (int i=0;i<len;i++)
	{
	  std::cout.write((char*)&left[i],2);
	  std::cout.write((char*)&right[i],2);
	}

      return len;
    }
  };

}

#endif

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
#include <libvideogfx/audio/fileio/timedsink.hh>
#include <iostream>


namespace videogfx {

  struct AudioParam
  {
    AudioParam() : n_channels(2), rate(44100), bits_per_sample(16) { }

    int  n_channels;
    int  rate;
    int  bits_per_sample;
  };


  class AudioSink
  {
  public:
    virtual ~AudioSink() { }

    virtual void       SetParam(const AudioParam& p) { }
    virtual AudioParam AskParam() const { return AudioParam(); }

    virtual void SendSamples(const int8*  samples,int len);
    virtual void SendSamples(const int16* samples,int len);
    virtual void SendSamples(const int32* samples,int len);
    virtual int  AskBufferingDelay() const { return 0; } // buffering delay in msecs
  };


  class TimedAudioSink : public AudioSink,
			 public TimedPresentationSink
  {
  public:
    /* The this method the insert timestamps between audio samples.
       The specified timestamp relates to the first sample of the
       next SendSamples() call.
    */

    virtual void SendTimestamp(Timestamp) { }
  };


  class AudioSink_cout : public AudioSink
  {
  public:
    void SendSamples(const int16* samples,int len)
    {
      for (int i=0;i<len;i++)
	{
	  std::cout.write((char*)&samples[i],2);
	}
    }
  };

}

#endif

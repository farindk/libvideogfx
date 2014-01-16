
#include "libvideogfx/audio/fileio/audiosink.hh"
#include <algorithm>

namespace videogfx {

  void AudioSink::SendSamples(const int8*  samples,int len)
  {
    int16 buf[1000];

    while (len)
      {
	int l = std::min(len,1000);
	for (int i=0;i<l;i++)
	  buf[i] = samples[i] << 8;

	SendSamples(buf,l);
	len -= l;
      }
  }

  void AudioSink::SendSamples(const int16* samples,int len)
  {
    // MessageDisplay::Show(ErrSev_Warning,"No 16 bit audio output defined. Most probably, this is an error.");

    int32 buf[1000];

    while (len)
      {
	int l = std::min(len,1000);
	for (int i=0;i<l;i++)
	  buf[i] = samples[i] << 16;

	SendSamples(buf,l);
	len -= l;
      }
  }

  void AudioSink::SendSamples(const int32* samples,int len)
  {
    int16 buf[1000];

    while (len)
      {
	int l = std::min(len,1000);
	for (int i=0;i<l;i++)
	  buf[i] = samples[i] >> 16;

	SendSamples(buf,l);
	len -= l;
      }
  }

}


#include <math.h>
#include <iostream>
#include "libvideogfx/libvideogfx.hh"
#include <unistd.h>

using namespace std;
using namespace videogfx;


int main()
{
  try
    {
      int16 sine [44100];
      int16 sine2[44100];

      for (int i=0;i<44100;i++)
	{
	  sine [i] = (int)(20000*sin(2*M_PI*i* 500/44100));
	  sine2[i] = (int)(20000*sin(2*M_PI*i* 750/44100));
	}

      AudioSink_LinuxSndCard snd;
      AudioParam param;
      param.n_channels = 2;
      param.rate = 44100;
      snd.SetParam(param);
      param = snd.AskParam();

      int16 buf[2*44100];
      for (int i=0;i<44100;i++)
	{
	  buf[2*i  ]=sine [i];
	  buf[2*i+1]=sine2[i];
	}

      for (int i=0;i<5;i++)
	{
	  snd.SendSamples(buf,2*44100);
	}

      while (snd.PresentationDataPending())
	{
	  snd.PresentData(snd.NextDataPresentationTime());
	  usleep(100000);
	}
    }
  catch (const Excpt_Base& e)
    {
      MessageDisplay::Show(e);
    }

  return 0;
}

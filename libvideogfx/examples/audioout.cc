
#include <math.h>
#include <iostream>
#include "libvideogfx/libvideogfx.hh"
#include <unistd.h>

using namespace std;
using namespace videogfx;


int main()
{
  int16 sine[44100];

  for (int i=0;i<44100;i++)
    {
      sine[i] = (int)(20000*sin(2*M_PI*i*1000/44100));
      //cout << i << " " << sine[i] << endl;
    }

  AudioSink_LinuxSndCard snd;

  for (int i=0;i<10;i++)
    {
      int total=44100;
      while (total)
	{
	  int n=snd.SendSamples(&sine[44100-total],&sine[44100-total],total);
	  if (n)
	    {
	      cout << total << " " << n << endl;
	      total -= n;
	    }
	  usleep(100000);
	}
    }

  return 0;
}

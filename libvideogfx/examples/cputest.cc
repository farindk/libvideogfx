
#include "../libvideogfx/arch/cpu.hh"

#include <unistd.h>
#include <iostream>
using namespace std;
using namespace videogfx;


int main(int argc,char** argv)
{
  try
    {
      char buf[1000+1];
      CPU_Capabilities* cpu = CPU_Capabilities::AskCapabilities();

      cpu->GetCPUInfo(buf,1000);

      cout << "CPU    : " << buf << endl;
      cout << "Arch   : " << cpu->GetArchitectureName() << endl;

      if (cpu->AskArchitecture() == CPUArch_X86)
	{
	  cout << "MMX    : " << (cpu->HasMMX() ? "yes" : "no") << endl;
	  cout << "3dNow! : " << (cpu->Has3dNow() ? "yes" : "no") << endl;
	  cout << "SSE2   : " << (cpu->HasSSE2() ? "yes" : "no") << endl;
	  cout << "SSE4.1 : " << (cpu->HasSSE4_1() ? "yes" : "no") << endl;
	  cout << "AVX    : " << (cpu->HasAVX() ? "yes" : "no") << endl;
	  cout << "CMOV   : " << (cpu->HasCMOV() ? "yes" : "no") << endl;
	}
    }
  catch(const Excpt_Base& b)
    {
      MessageDisplay::Show(b);
    }

  return 0;
}

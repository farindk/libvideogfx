
#include "libvideogfx/arch/cpu.hh"

#include <unistd.h>
#include <iostream>
using namespace std;
using namespace videogfx;


main(int argc,char** argv)
{
  try
    {
      char buf[1000+1];
      CPU_Capabilities* cpu = CPU_Capabilities::AskCapabilities();

      cpu->GetCPUInfo(buf,1000);

      cout << "CPU    : " << buf << endl;
      cout << "Arch   : " << cpu->GetArchitectureName() << endl;

      if (cpu->GetArchitecture() == CPUArch_X86)
	{
	  cout << "MMX    : " << (cpu->HasMMX() ? "yes" : "no") << endl;
	  cout << "MMX-ext: " << (cpu->HasMMX2() ? "yes" : "no") << endl;
	  cout << "3dNow! : " << (cpu->Has3dNow() ? "yes" : "no") << endl;
	  cout << "MTRR   : " << (cpu->HasMTRR() ? "yes" : "no") << endl;
	  cout << "CMOV   : " << (cpu->HasCMOV() ? "yes" : "no") << endl;
	  cout << "FPU    : " << (cpu->HasFPU() ? "yes" : "no") << endl;
	}
    }
  catch(const Excpt_Base& e)
    {
      MessageDisplay::Show(e);
    }
}


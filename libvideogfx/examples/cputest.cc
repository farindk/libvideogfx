
#include "libvideogfx/arch/cpu.hh"

#include <unistd.h>
#include <iostream>
using namespace std;


main(int argc,char** argv)
{
  char buf[1000+1];
  CPU_Capabilities* cpu = CPU_Capabilities::AskCapabilities();

  cpu->GetCPUInfo(buf,1000);

  cout << "CPU    : " << buf << endl;
  cout << "Arch   : " << cpu->GetArchitectureName() << endl;

  cout << "MMX    : " << (cpu->HasMMX() ? "yes" : "no") << endl;
  cout << "MMX-ext: " << (cpu->HasMMX2() ? "yes" : "no") << endl;
  cout << "3dNow! : " << (cpu->Has3dNow() ? "yes" : "no") << endl;
  cout << "MTRR   : " << (cpu->HasMTRR() ? "yes" : "no") << endl;
  cout << "CMOV   : " << (cpu->HasCMOV() ? "yes" : "no") << endl;
  cout << "FPU    : " << (cpu->HasFPU() ? "yes" : "no") << endl;
}

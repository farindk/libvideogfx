
#include <libvideogfx/arch/cpu.hh>
#include <libvideogfx/error.hh>
#include <libvideogfx/types.hh>
#include "config.h"

#include <string.h>



const char* CPU_Capabilities::ArchName(CPU_Architecture arch)
{
  switch (arch)
    {
    case CPUArch_X86:     return "intel x86"; break;
    case CPUArch_68k:     return "680x0"; break;
    case CPUArch_PowerPC: return "PowerPC"; break;
    case CPUArch_ARM:     return "ARM"; break;
    case CPUArch_Sparc:   return "Sparc"; break;
    case CPUArch_HPPA:    return "HP-PA"; break;
    default:              return "unknown"; break;
    }
}



class CPU_Generic : public CPU_Capabilities
{
  // nothing special
};

// --------------------------------------------------------------------------

#if LINUX && CPU_x86

#include <fstream>
using namespace std;


class CPU_LinuxProc : public CPU_Capabilities
{
public:
  CPU_LinuxProc()
  {
    mmx=mmx2=AMD3dnow=mtrr=cmov=fpu=false;
    d_typename=NULL;

    char buf[500+1];
    ifstream str("/proc/cpuinfo");
    if (str)
      {
	while (!str.eof())
	  {
	    str.getline(buf,500);
	    if (strncmp(buf,"flags",5)==0)
	      {
		if (strstr(buf,"mmx"))    mmx =true;
		if (strstr(buf,"sse"))    mmx2=true;
		if (strstr(buf,"mmxext")) mmx2=true;
		if (strstr(buf,"mtrr"))   mtrr=true;
		if (strstr(buf,"cmov"))   cmov=true;
		if (strstr(buf,"3dnow"))  AMD3dnow=true;
		if (strstr(buf,"fpu"))    fpu =true;
	      }

	    if (!d_typename && strncmp(buf,"model name",10)==0)
	      {
		const char* b = strstr(buf,":")+2;
		d_typename = new char[strlen(b)+1];
		strcpy(d_typename,b);
	      }
	  }
      }
  }

  void GetCPUInfo(char* buf,int maxChars,bool long_descr=false)
  {
    Assert(strlen(d_typename) < maxChars);
    strcpy(buf,d_typename);

    if (long_descr)
      {
	maxChars -= strlen(buf);
	if (HasMMX()) { Assert(maxChars >= 4); strcat(buf," MMX"); }
	if (HasMMX2()) { Assert(maxChars >= 7); strcat(buf," MMXext"); }
	if (Has3dNow()) { Assert(maxChars >= 7); strcat(buf," 3dNow!"); }
	if (HasMTRR()) { Assert(maxChars >= 5); strcat(buf," MTRR"); }
	if (HasCMOV()) { Assert(maxChars >= 5); strcat(buf," CMOV"); }
	if (HasFPU()) { Assert(maxChars >= 4); strcat(buf," FPU"); }
      }
  }

  CPU_Architecture GetArchitecture() const { return CPUArch_X86; }

  bool HasMMX() const { return mmx; }
  bool HasMMX2() const { return mmx2; }
  bool Has3dNow() const { return AMD3dnow; }
  bool HasMTRR() const { return mtrr; }
  bool HasCMOV() const { return cmov; }
  bool HasFPU()  const { return fpu; }

private:
  bool mmx,mmx2,AMD3dnow,mtrr,cmov,fpu;
  char* d_typename;
};
#endif


// --------------------------------------------------------------------------

const char* CPU_Capabilities::wrong_cpu_err = "CPU capability of non-present CPU architecture requested";
CPU_Capabilities* CPU_Capabilities::d_cpu = NULL;


CPU_Capabilities* CPU_Capabilities::AskCapabilities()
{
  if (!d_cpu)
    {
#if LINUX && CPU_x86
      if (!d_cpu) d_cpu = new CPU_LinuxProc;
#endif
      if (!d_cpu) d_cpu = new CPU_Generic;
    }

  Assert(d_cpu);

  return d_cpu;
}


void CPU_Capabilities::GetCPUInfo(char* buf,int maxChars,bool)
{
  const char* unknown_arch = "unknown CPU";

  Assert(strlen(unknown_arch) < maxChars);
  strcpy(buf,unknown_arch);
}


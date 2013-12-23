/********************************************************************************
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

#include <libvideogfx/arch/cpu.hh>
#include <libvideogfx/error.hh>
#include <libvideogfx/types.hh>
#include "config.h"

#include <string.h>
#include <fstream>

namespace videogfx {
  using namespace std;

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

#if CPU_x86

#define CPU_CAP_MMX      (1<<0)
#define CPU_CAP_MMXEXT   (1<<1)
#define CPU_CAP_SSE      (1<<2)
#define CPU_CAP_SSE2     (1<<3)
#define CPU_CAP_SSE3     (1<<4)
#define CPU_CAP_SSSE3    (1<<5)
#define CPU_CAP_SSE4_1   (1<<6)
#define CPU_CAP_SSE4_2   (1<<7)
#define CPU_CAP_AVX      (1<<8)
#define CPU_CAP_SSE4a    (1<<9)
#define CPU_CAP_3Dnow    (1<<10)
#define CPU_CAP_3DnowExt (1<<11)
#define CPU_CAP_CMOV     (1<<12)

  class CPU_X86 : public CPU_Capabilities
  {
  public:
    CPU_X86()
    {
      m_flags = 0;

      uint32_t regs[4];
      uint32_t a = 1;

#ifdef _MSC_VER
      __cpuid((int *)regs, (int)a);

#else
      __asm__ volatile
        ("cpuid" : "=a" (regs[0]), "=b" (regs[1]), "=c" (regs[2]), "=d" (regs[3])
         : "a" (a), "c" (0));
#endif

      uint32_t ecx = regs[2];
      uint32_t edx = regs[3];

      if ((edx & (1<<15))) m_flags |= CPU_CAP_CMOV;
      if ((edx & (1<<23))) m_flags |= CPU_CAP_MMX;
      if ((edx & (1<<25))) m_flags |= CPU_CAP_SSE;
      if ((edx & (1<<26))) m_flags |= CPU_CAP_SSE2;
      if ((ecx & (1<< 0))) m_flags |= CPU_CAP_SSE3;
      if ((ecx & (1<< 9))) m_flags |= CPU_CAP_SSSE3;
      if ((ecx & (1<<19))) m_flags |= CPU_CAP_SSE4_1;
      if ((ecx & (1<<20))) m_flags |= CPU_CAP_SSE4_2;
      if ((ecx & (1<<28))) m_flags |= CPU_CAP_AVX;


      a = 0x80000001;

#ifdef _MSC_VER
      __cpuid((int *)regs, (int)a);

#else
      __asm__ volatile
        ("cpuid" : "=a" (regs[0]), "=b" (regs[1]), "=c" (regs[2]), "=d" (regs[3])
         : "a" (a), "c" (0));
#endif

      ecx = regs[2];
      edx = regs[3];

      if ((edx & (1<<22))) m_flags |= CPU_CAP_MMXEXT;
      if ((edx & (1<<30))) m_flags |= CPU_CAP_3DnowExt;
      if ((edx & (1<<31))) m_flags |= CPU_CAP_3Dnow;
      if ((ecx & (1<< 6))) m_flags |= CPU_CAP_SSE4a;
    }

    void GetCPUInfo(char* buf,int maxChars,bool long_descr=false)
    {
      assert(maxChars>=3);
      strcpy(buf,"X86");
      maxChars-=3;

      if (long_descr)
	{
	  maxChars -= strlen(buf);
	  if (HasMMX()) { assert(maxChars >= 4); strcat(buf," MMX"); }
	  if (HasSSE()) { assert(maxChars >= 4); strcat(buf," SSE"); }
	  if (HasSSE4_1()) { assert(maxChars >= 4); strcat(buf," SSE4.1"); }
	  if (Has3dNow()) { assert(maxChars >= 7); strcat(buf," 3dNow!"); }
	  if (HasCMOV()) { assert(maxChars >= 5); strcat(buf," CMOV"); }
	}
    }

    CPU_Architecture AskArchitecture() const { return CPUArch_X86; }

    bool HasMMX() const     { return !!(m_flags & CPU_CAP_MMX); }
    bool HasMMXExt() const  { return !!(m_flags & CPU_CAP_MMXEXT); }
    bool HasSSE() const     { return !!(m_flags & CPU_CAP_SSE); }
    bool HasSSE2() const    { return !!(m_flags & CPU_CAP_SSE2); }
    bool HasSSE3() const    { return !!(m_flags & CPU_CAP_SSE3); }
    bool HasSSSE3() const   { return !!(m_flags & CPU_CAP_SSSE3); }
    bool HasSSE4_1() const  { return !!(m_flags & CPU_CAP_SSE4_1); }
    bool HasSSE4_2() const  { return !!(m_flags & CPU_CAP_SSE4_2); }
    bool HasAVX() const     { return !!(m_flags & CPU_CAP_AVX); }
    bool HasSSE4a() const   { return !!(m_flags & CPU_CAP_SSE4a); }
    bool Has3dNow() const   { return !!(m_flags & CPU_CAP_3Dnow); }
    bool Has3dNowExt() const   { return !!(m_flags & CPU_CAP_3DnowExt); }
    bool HasCMOV() const    { return !!(m_flags & CPU_CAP_CMOV); }

  private:
    uint32_t m_flags;
  };
#endif


  // --------------------------------------------------------------------------

  const char* CPU_Capabilities::wrong_cpu_err = "CPU capability of non-present CPU architecture requested";
  CPU_Capabilities* CPU_Capabilities::d_cpu = NULL;


  CPU_Capabilities* CPU_Capabilities::AskCapabilities()
  {
    if (!d_cpu)
      {
#if CPU_x86
	if (!d_cpu) d_cpu = new CPU_X86;
#endif
	if (!d_cpu) d_cpu = new CPU_Generic;
      }

    assert(d_cpu);

    return d_cpu;
  }


  void CPU_Capabilities::GetCPUInfo(char* buf,int maxChars,bool)
  {
    const char* unknown_arch = "unknown CPU";

    assert((int)strlen(unknown_arch) < maxChars);
    strcpy(buf,unknown_arch);
  }

}

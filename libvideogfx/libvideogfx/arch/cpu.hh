/********************************************************************************
  libvideogfx/arch/cpu.hh

  purpose:

  notes:
    Use CPU_Capabilities::AskCapabilities() to get access to the object
    describing the active CPU.

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
    09/Nov/1999 - Dirk Farin - First revision
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

#ifndef LIBVIDEOGFX_ARCH_CPU_HH
#define LIBVIDEOGFX_ARCH_CPU_HH

#include <libvideogfx/error.hh>

namespace videogfx {

  enum CPU_Architecture
  {
    CPUArch_X86, CPUArch_68k, CPUArch_PowerPC, CPUArch_ARM, CPUArch_Sparc,
    CPUArch_HPPA,
    CPUArch_Unknown
  };


  class CPU_Capabilities
  {
  public:
    static CPU_Capabilities* AskCapabilities(); // get capabilities of CPU


    virtual ~CPU_Capabilities() { }

    virtual void GetCPUInfo(char* buf,int maxChars,bool long_description=false);
    virtual CPU_Architecture AskArchitecture() const { return CPUArch_Unknown; }
    const char* GetArchitectureName() const { return ArchName(AskArchitecture()); }
    static const char* ArchName(CPU_Architecture arch);


    // x86 architecture

    virtual bool HasMMX()   const { AssertDescr(0,wrong_cpu_err); return false; }
    virtual bool HasMMX2()  const { AssertDescr(0,wrong_cpu_err); return false; }
    virtual bool Has3dNow() const { AssertDescr(0,wrong_cpu_err); return false; }
    virtual bool HasMTRR()  const { AssertDescr(0,wrong_cpu_err); return false; }
    virtual bool HasCMOV()  const { AssertDescr(0,wrong_cpu_err); return false; }
    virtual bool HasFPU()   const { AssertDescr(0,wrong_cpu_err); return false; }


    // ARM architecture

    virtual bool HasXScale()  const { AssertDescr(0,wrong_cpu_err); return false; }


    // SPARC architecture

    virtual bool HasVIS()  const { AssertDescr(0,wrong_cpu_err); return false; }

  private:
    static CPU_Capabilities* d_cpu;
    static const char* wrong_cpu_err;
  };
}

#endif

/********************************************************************************
    LibVideoGfx - video processing library
    Copyright (C) 2002  Dirk Farin, Gerald Kuehne

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

#include <libvideogfx.hh>
#include "config.h"

#if ENABLE_MMX
#  include "libvideogfx/arch/mmx.h"
#endif


/*
  Flow graph for row transform:

  MEM:      I00 I01 I02 I03 I04 I05 I06 I07      8bit      I08 I09 I10 I11 I12 I13 I14 I15
  Reg:      I07 I06 I05 I04 I03 I02 I01 I00      8bit                   .....
  .        /  deinterlace / \  deinterlace \
  .        I07 I05 I03 I01   I06 I04 I02 I00    16bit

  .       -----------------------------------

  .        I13 I11 I09 I07   I05 I03 I01 I-1    16bit
  .    2*  I14 I12 I10 I08   I06 I04 I02 I00     ...
  .        I15 I13 I11 I09   I07 I05 I03 I01     ...
  .     =  O07 O06 O05 O04   O03 O02 O01 O00    16bit
  .        \              \ /              /
  Reg:      O07 O06 O05 O04 O03 O02 O01 O00      8bit
  MEM:      O00 O01 O02 O03 O04 O05 O06 O07      8bit
 */

namespace videogfx {

#if ENABLE_MMX
  static void LowPass_Binomial_Downsample_MMX (Bitmap<Pixel>& destbm, const Bitmap<Pixel>& srcbm)
  {
    destbm.Create((srcbm.AskWidth()+1)/2,(srcbm.AskHeight()+1)/2 ,8);

    const Pixel*const* src = srcbm.AskFrame();
    Pixel*const* dst = destbm.AskFrame();
    int w = srcbm.AskWidth();
    int h = srcbm.AskHeight();

    uint8* line = new Pixel[w+32];
    uint8* l = &line[16];

    uint64 hb = 0xFF00FF00L; hb = hb | (hb<<32);  // do it the awkward way to be compilable with Windows
    uint64 lb = 0x00FF00FFL; lb = lb | (lb<<32);
    uint64 a2 = 0x00020002L; a2 = a2 | (a2<<32);

    for (int y=0;y<h;y+=2)
      {
	// Column transform

	pxor_r2r(mm0,mm0);

	for (int x=0;x<w;x+=8)
	  {
	    uint64* sp_m1 = (uint64*)&src[y-1][x];
	    uint64* sp_0  = (uint64*)&src[y  ][x];
	    uint64* sp_p1 = (uint64*)&src[y+1][x];
	    uint64* dp    = (uint64*)&l[x];

	    // unpack 8 pixels of (y-1) to 16bit in (mm1,mm2)

	    movq_m2r(*sp_m1,mm1);
	    movq_r2r(mm1,mm2);
	    punpckhbw_r2r(mm0,mm1);
	    punpcklbw_r2r(mm0,mm2);

	    // unpack 8 pixels of (y) to 16bit and add twice to (mm1,mm2)

	    movq_m2r(*sp_0,mm3);
	    movq_r2r(mm3,mm4);
	    punpckhbw_r2r(mm0,mm3);
	    punpcklbw_r2r(mm0,mm4);
	    paddw_r2r(mm3,mm1);
	    paddw_r2r(mm4,mm2);
	    paddw_r2r(mm3,mm1);
	    paddw_r2r(mm4,mm2);

	    // unpack 8 pixels of (y+1) to 16bit and add to (mm1,mm2)

	    movq_m2r(*sp_p1,mm5);
	    movq_r2r(mm5,mm6);
	    punpckhbw_r2r(mm0,mm5);
	    punpcklbw_r2r(mm0,mm6);
	    paddw_r2r(mm5,mm1);
	    paddw_r2r(mm6,mm2);

	    // divide by 4

	    paddw_m2r(a2,mm1);
	    paddw_m2r(a2,mm2);

	    psrlw_i2r(2,mm1);
	    psrlw_i2r(2,mm2);

	    // store to line buffer

	    packuswb_r2r(mm1,mm2);

	    movq_r2m(mm2,*dp);
	  }

	l[-1] = l[0];
	l[w]  = l[w-1];

	// Row transform

	for (int x=0;x<w;x+=16)
	  {
	    uint64* sp_m1 = (uint64*)&l[x-8];
	    uint64* sp_0  = (uint64*)&l[x];
	    uint64* sp_p1 = (uint64*)&l[x+8];
	    uint64* dp    = (uint64*)&dst[y/2][x/2];

	    // "deinterlace" 8 pixels at (x) to 16bit to (mm1,mm2)

	    movq_m2r(*sp_0,mm1);
	    movq_r2r(mm1,mm2);
	    pand_m2r(hb,mm1);
	    psrlw_i2r(8,mm1);       // mm1: I07 I05 I03 I01
	    pand_m2r(lb,mm2);       // mm2: I06 I04 I02 I00

	    // "deinterlace" 8 pixels at (x+8) to 16bit to (mm3,mm4)

	    movq_m2r(*sp_p1,mm3);
	    movq_r2r(mm3,mm4);
	    pand_m2r(hb,mm3);
	    psrlw_i2r(8,mm3);       // mm3: I15 I13 I11 I09
	    pand_m2r(lb,mm4);       // mm4: I14 I12 I10 I08

	    // --- process first 4 output pixels ---

	    // construct mm5 with: I13 I11 I09 I07

	    movq_r2r(mm3,mm5);
	    movq_r2r(mm1,mm6);
	    psllq_i2r(16,mm5);
	    psrlq_i2r(48,mm6);
	    por_r2r(mm6,mm5);

	    //  sum filter in mm5:     I13 I11 I09 I07

	    paddw_r2r(mm4,mm5);  // 2* I14 I12 I10 I08
	    paddw_r2r(mm4,mm5);
	    paddw_r2r(mm3,mm5);  //    I15 I13 I11 I09

	    paddw_m2r(a2,mm5);
	    psrlw_i2r(2,mm5); // divide by 4

	    // --- process next 4 output pixels ---

	    // construct mm0 with: I05 I03 I01 I-1

	    movq_m2r(*sp_m1,mm7);
	    pand_m2r(hb,mm7);       // mm7: I-1 xxx xxx xxx
	    psrlq_i2r(56,mm7);

	    movq_r2r(mm1,mm0);
	    psllq_i2r(16,mm0);
	    por_r2r(mm7,mm0);

	    // sum filter in mm0:      I05 I03 I01 I-1

	    paddw_r2r(mm2,mm0); // 2*  I06 I04 I02 I00
	    paddw_r2r(mm2,mm0);
	    paddw_r2r(mm1,mm0); //     I07 I05 I03 I01

	    paddw_m2r(a2,mm0);
	    psrlw_i2r(2,mm0);  // divide by 4


	    // build 8 output pixels and save

	    packuswb_r2r(mm5,mm0);

	    movq_r2m(mm0,*dp);
	  }
      }

    emms();

    delete[] line;
  }



  static void LowPass_Binomial_MMX (Bitmap<Pixel>& destbm, const Bitmap<Pixel>& srcbm)
  {
    destbm.Create(srcbm.AskWidth(),srcbm.AskHeight() ,8,8,8);

    const Pixel*const* src = srcbm.AskFrame();
    Pixel*const* dst = destbm.AskFrame();
    int w = srcbm.AskWidth();
    int h = srcbm.AskHeight();

    uint8* line = new Pixel[w+32];
    uint8* l = &line[16];

    uint64 hb = 0xFF00FF00L; hb = hb | (hb<<32);  // do it the awkward way to be compilable with Windows
    uint64 lb = 0x00FF00FFL; lb = lb | (lb<<32);
    uint64 a2 = 0x00020002L; a2 = a2 | (a2<<32);

    for (int y=0;y<h;y++)
      {
	// Column transform

	pxor_r2r(mm0,mm0);

	for (int x=0;x<w;x+=8)
	  {
	    uint64* sp_m1 = (uint64*)&src[y-1][x];
	    uint64* sp_0  = (uint64*)&src[y  ][x];
	    uint64* sp_p1 = (uint64*)&src[y+1][x];
	    uint64* dp    = (uint64*)&l[x];

	    // unpack 8 pixels of (y-1) to 16bit in (mm1,mm2)

	    movq_m2r(*sp_m1,mm1);
	    movq_r2r(mm1,mm2);
	    punpckhbw_r2r(mm0,mm1);
	    punpcklbw_r2r(mm0,mm2);

	    // unpack 8 pixels of (y) to 16bit and add twice to (mm1,mm2)

	    movq_m2r(*sp_0,mm3);
	    movq_r2r(mm3,mm4);
	    punpckhbw_r2r(mm0,mm3);
	    punpcklbw_r2r(mm0,mm4);
	    paddw_r2r(mm3,mm1);
	    paddw_r2r(mm4,mm2);
	    paddw_r2r(mm3,mm1);
	    paddw_r2r(mm4,mm2);

	    // unpack 8 pixels of (y+1) to 16bit and add to (mm1,mm2)

	    movq_m2r(*sp_p1,mm5);
	    movq_r2r(mm5,mm6);
	    punpckhbw_r2r(mm0,mm5);
	    punpcklbw_r2r(mm0,mm6);
	    paddw_r2r(mm5,mm1);
	    paddw_r2r(mm6,mm2);

	    // divide by 4

	    paddw_m2r(a2,mm1);
	    paddw_m2r(a2,mm2);

	    psrlw_i2r(2,mm1);
	    psrlw_i2r(2,mm2);

	    // store to line buffer

	    packuswb_r2r(mm1,mm2);

	    movq_r2m(mm2,*dp);
	  }

	l[-1] = l[0];
	l[w]  = l[w-1];

	// Row transform

	for (int x=0;x<w;x+=16)
	  {
	    uint64* sp_m1 = (uint64*)&l[x-8];
	    uint64* sp_0  = (uint64*)&l[x];
	    uint64* sp_p1 = (uint64*)&l[x+8];
	    uint64* dp    = (uint64*)&dst[y][x];
	    uint64* dp2   = (uint64*)&dst[y][x+8];

	    uint64* sp1_m1 = (uint64*)&l[x+1-8];
	    uint64* sp1_0  = (uint64*)&l[x+1];
	    uint64* sp1_p1 = (uint64*)&l[x+1+8];

	    // --------------------- first 8 output pixels -------------------------

	    // "deinterlace" 8 pixels at (x) to 16bit to (mm1,mm2)

	    movq_m2r(*sp_0,mm1);
	    movq_r2r(mm1,mm2);
	    pand_m2r(hb,mm1);
	    psrlw_i2r(8,mm1);       // mm1: I07 I05 I03 I01
	    pand_m2r(lb,mm2);       // mm2: I06 I04 I02 I00

	    // "deinterlace" 8 pixels at (x+8) to 16bit to (mm3,mm4)

	    movq_m2r(*sp_p1,mm3);
	    movq_r2r(mm3,mm4);
	    pand_m2r(hb,mm3);
	    psrlw_i2r(8,mm3);       // mm3: I15 I13 I11 I09
	    pand_m2r(lb,mm4);       // mm4: I14 I12 I10 I08

	    // --- process first 4 output pixels ---

	    // construct mm5 with: I13 I11 I09 I07

	    movq_r2r(mm3,mm5);
	    movq_r2r(mm1,mm6);
	    psllq_i2r(16,mm5);
	    psrlq_i2r(48,mm6);
	    por_r2r(mm6,mm5);

	    //  sum filter in mm5:     I13 I11 I09 I07

	    paddw_r2r(mm4,mm5);  // 2* I14 I12 I10 I08
	    paddw_r2r(mm4,mm5);
	    paddw_r2r(mm3,mm5);  //    I15 I13 I11 I09

	    paddw_m2r(a2,mm5);
	    psrlw_i2r(2,mm5); // divide by 4

	    // --- process next 4 output pixels ---

	    // construct mm0 with: I05 I03 I01 I-1

	    movq_m2r(*sp_m1,mm7);
	    pand_m2r(hb,mm7);       // mm7: I-1 xxx xxx xxx
	    psrlq_i2r(56,mm7);

	    movq_r2r(mm1,mm0);
	    psllq_i2r(16,mm0);
	    por_r2r(mm7,mm0);

	    // sum filter in mm0:      I05 I03 I01 I-1

	    paddw_r2r(mm2,mm0); // 2*  I06 I04 I02 I00
	    paddw_r2r(mm2,mm0);
	    paddw_r2r(mm1,mm0); //     I07 I05 I03 I01

	    paddw_m2r(a2,mm0);
	    psrlw_i2r(2,mm0);  // divide by 4


	    // build 8 output pixels and save

	    //packuswb_r2r(mm5,mm0);

	    movq_r2m(mm0,*dp);
	    movq_r2m(mm5,*dp2);

	    // --------------------- second 8 output pixels -------------------------

	    // "deinterlace" 8 pixels at (x) to 16bit to (mm1,mm2)

	    movq_m2r(*sp1_0,mm1);
	    movq_r2r(mm1,mm2);
	    pand_m2r(hb,mm1);
	    psrlw_i2r(8,mm1);       // mm1: I07 I05 I03 I01
	    pand_m2r(lb,mm2);       // mm2: I06 I04 I02 I00

	    // "deinterlace" 8 pixels at (x+8) to 16bit to (mm3,mm4)

	    movq_m2r(*sp1_p1,mm3);
	    movq_r2r(mm3,mm4);
	    pand_m2r(hb,mm3);
	    psrlw_i2r(8,mm3);       // mm3: I15 I13 I11 I09
	    pand_m2r(lb,mm4);       // mm4: I14 I12 I10 I08

	    // --- process first 4 output pixels ---

	    // construct mm5 with: I13 I11 I09 I07

	    movq_r2r(mm3,mm5);
	    movq_r2r(mm1,mm6);
	    psllq_i2r(16,mm5);
	    psrlq_i2r(48,mm6);
	    por_r2r(mm6,mm5);

	    //  sum filter in mm5:     I13 I11 I09 I07

	    paddw_r2r(mm4,mm5);  // 2* I14 I12 I10 I08
	    paddw_r2r(mm4,mm5);
	    paddw_r2r(mm3,mm5);  //    I15 I13 I11 I09

	    paddw_m2r(a2,mm5);
	    psrlw_i2r(2,mm5); // divide by 4

	    // --- process next 4 output pixels ---

	    // construct mm0 with: I05 I03 I01 I-1

	    movq_m2r(*sp1_m1,mm7);
	    pand_m2r(hb,mm7);       // mm7: I-1 xxx xxx xxx
	    psrlq_i2r(56,mm7);

	    movq_r2r(mm1,mm0);
	    psllq_i2r(16,mm0);
	    por_r2r(mm7,mm0);

	    // sum filter in mm0:      I05 I03 I01 I-1

	    paddw_r2r(mm2,mm0); // 2*  I06 I04 I02 I00
	    paddw_r2r(mm2,mm0);
	    paddw_r2r(mm1,mm0); //     I07 I05 I03 I01

	    paddw_m2r(a2,mm0);
	    psrlw_i2r(2,mm0);  // divide by 4


	    // build 8 output pixels and save

	    //packuswb_r2r(mm5,mm0);

	    psllq_i2r(8,mm5);
	    psllq_i2r(8,mm0);

	    por_m2r(*dp,mm0);
	    por_m2r(*dp2,mm5);

	    movq_r2m(mm0,*dp);
	    movq_r2m(mm5,*dp2);
	  }
      }

    emms();

    delete[] line;
  }


  static void LowPass_Binomial_MMX (Bitmap<short>& destbm, const Bitmap<short>& srcbm)
  {
    assert(sizeof(short)==2);

    destbm.Create(srcbm.AskWidth(),srcbm.AskHeight() ,4,8,4); //4,4,4);   STRANGE: ,4,4,4 does sometimes produce different result

    const short*const* src = srcbm.AskFrame();
    volatile short*const* dst = destbm.AskFrame();
    int w = srcbm.AskWidth();
    int h = srcbm.AskHeight();

    volatile short* line = new short[srcbm.AskAlignedWidth()+32];
    volatile short* l = &line[16];

    uint64 a2 = 0x00020002L; a2 = a2 | (a2<<32);

    for (int y=0;y<h;y++)
      {
	// Column transform

	for (int x=0;x<w;x+=4)
	  {
	    uint64* sp_m1 = (uint64*)&src[y-1][x];
	    uint64* sp_0  = (uint64*)&src[y  ][x];
	    uint64* sp_p1 = (uint64*)&src[y+1][x];
	    uint64* dp    = (uint64*)&l[x];

#if 0
	    for (int i=0;i<1;i++)
	      ((int16*)dp)[i] = (((int16*)sp_m1)[i] + 2*((int16*)sp_0)[i] + ((int16*)sp_p1)[i] + 2)>>2;
#endif

#if 1
	    movq_m2r(*sp_0,mm1);
#if 1
	    paddw_r2r(mm1,mm1);
	    paddw_m2r(*sp_m1,mm1);
	    paddw_m2r(*sp_p1,mm1);

	    // divide by 4

	    paddw_m2r(a2,mm1);
	    psraw_i2r(2,mm1);

	    // store to line buffer
#endif
	    movq_r2m(mm1,*dp);
#endif
	  }

	l[-1] = l[0];
	l[w]  = l[w-1];

	// Row transform

#if 1
	{
	  // initialize

	  uint64* s0 = (uint64*)&l[-4];
	  uint64* s1 = (uint64*)&l[0];

	  movq_m2r(*s0,mm1);
	  movq_m2r(*s1,mm2);
	}
#endif

	for (int x=0;x<w;x+=4)
	  {
	    uint64* sp_p1 = (uint64*)&l[x+4];
	    uint64* dp    = (uint64*)&dst[y][x];

#if 0
	    uint64* sp_p0 = (uint64*)&l[x];
	    for (int i=0;i<1;i++)
	      ((int16*)dp)[i] = ((int16*)sp_p0)[i];

	    //*dp = sp_p1[-1];
#endif

	    // load next 4 input values

#if 1
	    movq_r2r(mm1,mm0);
	    movq_r2r(mm2,mm1);
	    movq_m2r(*sp_p1,mm2);
#if 1
	    // prepare left vector

	    movq_r2r(mm0,mm5);
	    psrlq_i2r(48,mm5);

	    movq_r2r(mm1,mm6);
	    psllq_i2r(16,mm6);
	    por_r2r(mm5,mm6);

	    // prepare right vector

	    movq_r2r(mm2,mm3);
	    psllq_i2r(48,mm3);

	    movq_r2r(mm1,mm4);
	    psrlq_i2r(16,mm4);
	    por_r2r(mm3,mm4);

	    // add filter values

	    paddw_r2r(mm1,mm4);
	    paddw_r2r(mm6,mm4);
	    paddw_r2r(mm1,mm4);

	    // divide by 4

	    paddw_m2r(a2,mm4);
	    psraw_i2r(2,mm4);

	    movq_r2m(mm4,*dp);
#endif
	    //movq_r2m(mm1,*dp);
#endif
	  }
      }

    emms();

    delete[] line;
  }
#endif


  template <class T> void LowPass_Binomial_scalar (Bitmap<T>& outbm, const Bitmap<T>& inbm)
  {
    int w,h;
    outbm.Create(w=inbm.AskWidth(),h=inbm.AskHeight(),4,4,4); //8,8,8);

    const T*const* ip = inbm.AskFrame();
    T*const* op = outbm.AskFrame();

    T* line = new T[w+32];
    T* l=line+16;

    const int round=2;

    for (int y=0;y<h;y++)
      {
	for (int x=0;x<w;x++)
	  l[x] = (ip[y-1][x] + ip[y][x]+ip[y][x] + ip[y+1][x] + round)>>2;

	l[-1]=l[0];
	l[w]=l[w-1];

	for (int x=0;x<w;x++)
	  op[y][x] = (l[x-1] + l[x]+l[x] + l[x+1] + round)>>2;
      }

    delete[] line;
  }

  static void LowPass_Binomial_Downsample_scalar (Bitmap<Pixel>& outbm, const Bitmap<Pixel>& inbm)
  {
    int w,h;
    outbm.Create(((w=inbm.AskWidth())+1)/2,((h=inbm.AskHeight())+1)/2,8,8,8);

    const Pixel*const* ip = inbm.AskFrame();
    Pixel*const* op = outbm.AskFrame();

    Pixel* line = new Pixel[w+32];
    Pixel* l=line+16;

    const int round=2;

    for (int y=0;y<h;y+=2)
      {
	for (int x=0;x<w;x++)
	  l[x] = (ip[y-1][x] + ip[y][x]+ip[y][x] + ip[y+1][x] + round)>>2;

	l[-1]=l[0];
	l[w]=l[w-1];

	for (int x=0;x<w;x+=2)
	  op[y][x] = (l[x-1] + l[x]+l[x] + l[x+1] + round)>>2;
      }

    delete[] line;
  }


  void LowPass_Binomial (Bitmap<Pixel>& dest, const Bitmap<Pixel>& img)
  {
    CPU_Capabilities* cpu = CPU_Capabilities::AskCapabilities();

#if ENABLE_MMX
    if (cpu->HasMMX())
      {
	LowPass_Binomial_MMX (dest, img);
	return;
      }
#endif

    LowPass_Binomial_scalar (dest, img);
  }

#include <iostream>
  using namespace std;

  void Compare(const Bitmap<short>& d1,const Bitmap<short>& d2)
  {
    int w = d1.AskWidth();
    int h = d1.AskHeight();

    for (int y=0;y<h;y++)
      for (int x=0;x<w;x++)
	if (d1.AskFrame()[y][x] != d2.AskFrame()[y][x])
	  {
	    cout << "DIFF: " << x << ";" << y << " "
		 << d1.AskFrame()[y][x] << " "
		 << d2.AskFrame()[y][x] << endl;
	  }
  }

  void LowPass_Binomial (Bitmap<short>& dest, const Bitmap<short>& img)
  {
    CPU_Capabilities* cpu = CPU_Capabilities::AskCapabilities();

#if 0
    Bitmap<short> dm,ds;
    LowPass_Binomial_MMX   (dm, img);
    LowPass_Binomial_scalar(ds, img);
    Compare(dm,ds);
#endif

#if 0 //ENABLE_MMX
    if (cpu->HasMMX())
      {
	LowPass_Binomial_MMX (dest, img);

	return;
      }
#endif

    LowPass_Binomial_scalar (dest, img);
  }

  void LowPass_Binomial_Downsample(Bitmap<Pixel>& dest, const Bitmap<Pixel>& img)
  {
    CPU_Capabilities* cpu = CPU_Capabilities::AskCapabilities();

#if ENABLE_MMX
    if (cpu->HasMMX())
      {
	LowPass_Binomial_Downsample_MMX (dest, img);
	return;
      }
#endif

    LowPass_Binomial_Downsample_scalar (dest, img);
  }
}

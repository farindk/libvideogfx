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

#include "libvideogfx/graphics/filters/gauss_mmx.hh"
#include "libvideogfx/arch/mmx.h"
#include <assert.h>

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

  void LowPass_3x3Gauss_MMX(Bitmap<Pixel>& destbm, const Bitmap<Pixel>& srcbm)
  {
    assert(false); // "Use of \"LowPass_3x3Gauss_MMX()\" is deprecated. Use LowPass_Binormial_Decimate() instead"

    destbm.Create((srcbm.AskWidth()+1)/2,(srcbm.AskHeight()+1)/2 ,8);

    const Pixel*const* src = srcbm.AskFrame();
    Pixel*const* dst = destbm.AskFrame();
    int w = srcbm.AskWidth();
    int h = srcbm.AskHeight();

    uint8* line = new Pixel[w+32];
    uint8* l = &line[16];

    uint64 hb = 0xFF00FF00L; hb = hb | (hb<<32);  // do it the awkward way to be compilable with Windows
    uint64 lb = 0x00FF00FFL; lb = lb | (lb<<32);

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

	    psrlw_i2r(2,mm0);  // divide by 4


	    // build 8 output pixels and save

	    packuswb_r2r(mm5,mm0);

	    movq_r2m(mm0,*dp);
	  }
      }

    emms();

    delete[] line;
  }

}

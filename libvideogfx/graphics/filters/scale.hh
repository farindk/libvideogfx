/*********************************************************************
  libvideogfx/graphics/filters/scale.hh

  purpose:
    Bitmap scaling with a large set if different filters.
    Based on public domain third party code.

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmail.com

  modifications:
    18/Jul/2012 - Dirk Farin - wrapper class to third party code
 ********************************************************************************
    LibVideoGfx - video processing library
    Copyright (C) 2012  Dirk Farin

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

#ifndef LIBVIDEOGFX_GRAPHICS_FILTERS_SCALE_HH
#define LIBVIDEOGFX_GRAPHICS_FILTERS_SCALE_HH

#include <libvideogfx/graphics/datatypes/bitmap.hh>
#include <libvideogfx/graphics/datatypes/image.hh>

#include <libvideogfx/graphics/filters/resampler.h>
#include <limits>


namespace videogfx {

  class Scaler
  {
  public:
    Scaler();
    ~Scaler();

    enum BoundaryMode
      {
        Boundary_Wrap    = Resampler::BOUNDARY_WRAP,
        Boundary_Reflect = Resampler::BOUNDARY_REFLECT,
        Boundary_Clamp   = Resampler::BOUNDARY_CLAMP
      };

    enum Filter
      {
        Filter_Box,
        Filter_Tent,
        Filter_Bell,
        Filter_BSpline,
        Filter_Mitchell,
        Filter_Lanczos3,
        Filter_Lanczos4,
        Filter_Lanczos6,
        Filter_Lanczos12,
        Filter_Blackman,
        Filter_Kaiser,
        Filter_Gaussian,
        Filter_CatmullRom,
        Filter_QuadraticInterp,
        Filter_QuadraticApprox,
        Filter_QuadraticMix
      };

    void setBoundaryMode(enum BoundaryMode mode);
    void setFilter(enum Filter mode);

    template <class Pel> void Scale(/* */ Bitmap<Pel>& dst,
                                    const Bitmap<Pel>& src,
                                    int newWidth, int newHeight);

    /* Scales all bitmaps of the image, including alpha plane.
     */
    template <class Pel> void Scale(/* */ Image<Pel>& dst,
                                    const Image<Pel>& src,
                                    int newWidth, int newHeight);

  private:
    Resampler* mResampler;

    int mInWidth, mOutWidth;
    int mInHeight,mOutHeight;

    Resampler::Boundary_Op mBoundaryOp;
    const char* mFilter;


    void disposeResampler();
    void initResampler(int oldWidth, int oldHeight, int newWidth, int newHeight,
                       Resampler::Sample clipMin, Resampler::Sample clipMax);
  };


  // ===========================================================================
  //                              IMPLEMENTATION
  // ===========================================================================


  template <class Pel> void Scaler::Scale(Bitmap<Pel>& dst,const Bitmap<Pel>& src,
                                          int newWidth, int newHeight)
  {
    initResampler(src.getWidth(),src.getHeight(), newWidth,newHeight,
                  std::numeric_limits<Pel>::min(),
                  std::numeric_limits<Pel>::max() );

    dst.Create(newWidth, newHeight);

    Resampler::Sample* line = new Resampler::Sample[mInWidth];

    int inY=0;
    for (int y=0;y<mOutHeight;)
      {
        const Resampler::Sample* outLine = mResampler->get_line();
        if (outLine)
          {
            Pel* p = dst.AskFrame()[y];
            for (int x=0;x<mOutWidth;x++)
              p[x] = outLine[x];

            y++;
          }
        else
          {
            const Pel* p = src.AskFrame()[inY];
            for (int x=0;x<mInWidth;x++)
              line[x] = p[x];

            bool success = mResampler->put_line(line);
            assert(success);

            inY++;
          }
      }

    delete[] line;
  }


  template <class Pel> void Scaler::Scale(Image<Pel>& dst,const Image<Pel>& src,
                                          int newWidth, int newHeight)
  {
    ImageParam inParam = src.AskParam();
    ImageParam outParam = inParam;

    dst.Create(newWidth, newHeight, inParam.colorspace, inParam.chroma);
    if (inParam.has_alpha)
      {
        Bitmap<Pel> alpha;
        alpha.Create(newWidth, newHeight);
        dst.ReplaceBitmap(Bitmap_Alpha, alpha);
      }

    for (int ch=0;ch<4;ch++)
      {
        if (ch==3 && inParam.has_alpha==false) continue;
        if (ch< 3 && ch>=inParam.nColorChannels()) continue;

        const Bitmap<Pel>& inBM  = src.AskBitmap((BitmapChannel)ch);
        /* */ Bitmap<Pel>& outBM = dst.AskBitmap((BitmapChannel)ch);

        initResampler(inBM .getWidth(),inBM .getHeight(),
                      outBM.getWidth(),outBM.getHeight(),
                      std::numeric_limits<Pel>::min(),
                      std::numeric_limits<Pel>::max() );


        Resampler::Sample* line = new Resampler::Sample[mInWidth];

        int inY=0;
        for (int y=0;y<mOutHeight;)
          {
            const Resampler::Sample* outLine = mResampler->get_line();
            if (outLine)
              {
                Pel* p = outBM.AskFrame()[y];
                for (int x=0;x<mOutWidth;x++)
                  p[x] = outLine[x];

                y++;
              }
            else
              {
                const Pel* p = inBM.AskFrame()[inY];
                for (int x=0;x<mInWidth;x++)
                  line[x] = p[x];

                bool success = mResampler->put_line(line);
                assert(success);

                inY++;
              }
          }

        delete[] line;
      }
  }
}

#endif


#include "libvideogfx/graphics/filters/scale.hh"

namespace videogfx
{

  Scaler::Scaler()
  {
    mResampler=NULL;
    
    mInWidth =mOutWidth =0;
    mInHeight=mOutHeight=0;
    
    mBoundaryOp = Resampler::BOUNDARY_CLAMP;

    mFilter = NULL;
    setFilter(Filter_Lanczos4);
  }
  

  Scaler::~Scaler()
  {
    disposeResampler();
  }


  void Scaler::setBoundaryMode(enum BoundaryMode mode)
  {
    if ((Resampler::Boundary_Op)mode != mBoundaryOp) disposeResampler();

    mBoundaryOp = (Resampler::Boundary_Op)mode;
  }

  void Scaler::setFilter(enum Filter mode)
  {
    const char* oldFilter = mFilter;

    switch (mode)
      {
      case Filter_Box:         mFilter = "box";        break;
      case Filter_Tent:        mFilter = "tent";       break;
      case Filter_Bell:        mFilter = "bell";       break;
      case Filter_BSpline:     mFilter = "b-spline";   break;
      case Filter_Mitchell:    mFilter = "mitchell";   break;
      case Filter_Lanczos3:    mFilter = "lanczos3";   break;
      case Filter_Lanczos4:    mFilter = "lanczos4";   break;
      case Filter_Lanczos6:    mFilter = "lanczos6";   break;
      case Filter_Lanczos12:   mFilter = "lanczos12";  break;
      case Filter_Blackman:    mFilter = "blackman";   break;
      case Filter_Kaiser:      mFilter = "kaiser";     break;
      case Filter_Gaussian:    mFilter = "gaussian";   break;
      case Filter_CatmullRom:  mFilter = "catmullrom"; break;
      case Filter_QuadraticInterp: mFilter = "quadratic_interp"; break;
      case Filter_QuadraticApprox: mFilter = "quadratic_approx"; break;
      case Filter_QuadraticMix:    mFilter = "quadratic_mix";    break;
      }

    if (mFilter != oldFilter) disposeResampler();
  }


  void Scaler::disposeResampler()
  {
    if (mResampler)
      {
        delete mResampler;
        mResampler=NULL;
      }
  }


  void Scaler::initResampler(int oldWidth, int oldHeight, int newWidth, int newHeight,
                             Resampler::Sample clipMin, Resampler::Sample clipMax)
  {
    if (oldWidth  != mInWidth  ||
        oldHeight != mInHeight ||
        newWidth  != mOutWidth ||
        newHeight != mOutHeight)
      {
        disposeResampler();
      }

    mInWidth  = oldWidth;
    mInHeight = oldHeight;
    mOutWidth  = newWidth;
    mOutHeight = newHeight;

    if (mResampler==NULL)
      {
        mResampler = new Resampler(mInWidth,mInHeight,
                                   mOutWidth,mOutHeight,
                                   mBoundaryOp,
                                   clipMin, clipMax,
                                   mFilter,
                                   NULL,
                                   NULL,
                                   1.0f,1.0f, 0.0f, 0.0f);
      }
    else
      {
        mResampler->restart();
      }
  }


}

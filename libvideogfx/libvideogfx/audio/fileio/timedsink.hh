/********************************************************************************
  audio/fileio/timedsink.hh
    Generic API for timed output.

  purpose:

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
   14/Apr/2002 - Dirk Farin
     - new interface definition
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

#ifndef LIBVIDEOGFX_AUDIO_FILEIO_TIMEDSINK_HH
#define LIBVIDEOGFX_AUDIO_FILEIO_TIMEDSINK_HH

#include <libvideogfx/types.hh>


namespace videogfx {

  class TimedPresentationSink
  {
  public:
    virtual ~TimedPresentationSink() { }

    // If more input data can be fed into the object.
    virtual bool  CanAcceptMorePresentationData() const { return true; }

    // Wether object holds data which is not presented yet.
    virtual bool  PresentationDataPending() const { return false; }

    // Absolute time (msecs) when next available data should be presented.
    virtual int64 DataPresentationTime() const { return 0; }

    // Present data which is already available in the object upto the given time.
    virtual void  PresentData(int64 now) { }
  };

}

#endif

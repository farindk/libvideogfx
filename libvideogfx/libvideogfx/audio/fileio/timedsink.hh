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

  struct Timestamp
  {
    int64 timestamp;
    int   timerID;   /* ID of the reference timer this timestamp relates to.
			This is application dependent.
			A timerID<0 denotes an unknown timestamp. */
  };

  class TimedPresentationSink
  {
  public:
    virtual ~TimedPresentationSink() { }

    // Begin new sequential output with new time-base.
    //virtual void  Reset() { }

    // If "true", there is plenty of space for data to be buffered. This is only a hint!
    virtual bool  EnoughSpaceForMoreData() const { return true; }

    // Whether object holds data which is not presented yet.
    virtual bool  PresentationDataPending() const { return false; }

    // Absolute time when next available data should be presented.
    virtual Timestamp NextDataPresentationTime() const = 0;
    virtual Timestamp LastDataPresentationTime() const = 0;

    /* Present a data-unit which is already available in the object.
       The reference timer has to be the same as that returned by NextDataPresentationTime(). */
    virtual void  PresentData(Timestamp now) { }

    // Wether the sink presents the data at real-time. Savers are non-realtime for example.
    virtual bool  IsRealtimeSink() const { return false; }
  };

}

#endif

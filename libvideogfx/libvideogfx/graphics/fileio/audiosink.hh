/********************************************************************************
  graphics/fileio/audiosink.hh
    Generic audio data sink.

  purpose:

  notes:

  to do:

  author(s):
   - Dirk Farin, farin@ti.uni-mannheim.de

  modifications:
   14/Apr/2002 - Dirk Farin
     - new interface definition
 ********************************************************************************
    Copyright (C) 2002  Dirk Farin

    This program is distributed under GNU Public License (GPL) as
    outlined in the COPYING file that comes with the source distribution.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ********************************************************************************/

#ifndef LIBVIDEOGFX_GRAPHICS_FILEIO_AUDIOSINK_HH
#define LIBVIDEOGFX_GRAPHICS_FILEIO_AUDIOSINK_HH


struct AudioParam
{
  AudioParam() : n_channels(2), rate(44100), bits_per_sample(16), little_endian(true) { }

  int  n_channels;
  int  rate;
  int  bits_per_sample;
  bool little_endian;
};


class AudioSink
{
public:
  virtual ~AudioSink() { }

  virtual void SetParam(const AudioParam& p) { }
  virtual AudioParam AskParam() const { return AudioParam(); }
};

#endif

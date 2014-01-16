/********************************************************************************
  fileio/inputstream_istr.hh

  purpose:
    An input stream which reads data from a C++ "istream".

  notes:

  to do:

  author(s):
   - Dirk Farin, farin@ti.uni-mannheim.de

  modifications:
   ??/???/1998 - Dirk Farin - first implementation
 ********************************************************************************
    Copyright (C) 1999  Dirk Farin

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

#ifndef LIBVIDEOGFX_UTILITY_BITSTREAM_INPUTSTREAM_ISTR_HH
#define LIBVIDEOGFX_UTILITY_BITSTREAM_INPUTSTREAM_ISTR_HH

#include <libvideogfx/utility/bitstream/inputstream.hh>

#include <iostream>

namespace videogfx {

  class InputStream_IStream : public InputStream
  {
  public:
    InputStream_IStream();
    InputStream_IStream(std::istream& istr);
    ~InputStream_IStream();

    void SetIStream(std::istream& istr) { d_istr = &istr; }

    bool   IsEOF() const;
  
    bool   IsFiniteStream() const { return true; }
    uint64 AskStreamLength() const;
  
    bool   MaySeek() const { return true; }
    uint64 AskPosition() const;
    uint64 SetPosition(uint64);

  private:
    uint32 MyFillBuffer(uint8* mem,uint32 maxlen);

  private:
    std::istream* d_istr;
  };

}

#endif

/********************************************************************************
  utility/bitstream/inputstream.hh
    Generic input stream base class.

  purpose:

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
   23/Feb/2002 - Dirk Farin
     - first implementation
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

#ifndef LIBVIDEOGFX_UTILITY_BITSTREAM_INPUTSTREAM_HH
#define LIBVIDEOGFX_UTILITY_BITSTREAM_INPUTSTREAM_HH

#include <libvideogfx/types.hh>

namespace videogfx {

  class InputStream
  {
  public:
    InputStream();
    virtual ~InputStream();

    /* Fill buffer 'mem' with up to 'maxlen' bytes. Number of bytes written is
       returned. This may be less than requested but never less than 'minlen'.
       the full buffer is filled. Only if the input does not contain enough
       bytes to fill at least 'minlen' bytes into the buffer, less bytes are returned. */
    uint32 FillBuffer(uint8* mem,uint32 maxlen,uint32 minlen=1);

    /* Push back the given bytes to the input queue. You may push back
       as many bytes as you want and call this function as often as you like to. */
    void   Pushback(uint8* mem,int n_bytes);

    virtual bool   IsEOF() const = 0;

    /* If the stream is of finite length, you may call AskStreamLength().
       It is however not guaranteed that the size of the stream will not
       grow any more.

       These methods are provided to show a progress bar that displays
       where you are when playing from a file. Displaying a progress
       bar not always makes sense. If you are displaying a live video
       signal, it is quite useless.
    */
    virtual bool   IsFiniteStream() const = 0;
    virtual uint64 AskStreamLength() const = 0;


    /* These methods allow seeking in the input stream if this is possible.
       When the stream is located on a random access medium, seeking
       will normally be possible. On the other hand, when playing a
       stream that is received online via a network connection, seeking
       is not possible.

       Even when seeking is possible, it is not guaranteed that you can
       seek to any arbitrary position. This may be the case when you buffer
       for example the last 5 minutes of a live stream. So you may seek
       back up to 5 minutes but not more. Although this application is
       quite exotic you should check the result of "SetCurrentPosition()",
       which is the position that the class decides to set the current
       position really to.
    */
    virtual bool   MaySeek() const = 0;
    virtual uint64 AskPosition() const = 0;
    virtual uint64 SetPosition(uint64) = 0;

  protected:
    virtual uint32 MyFillBuffer(uint8* mem,uint32 maxlen) = 0;
    uint32  BytesInPushbackQueue() const { return d_pushback_length; }
    void    RemoveBytesFromPushbackQueue(int n);
    void    ClearPushbackQueue() { RemoveBytesFromPushbackQueue(BytesInPushbackQueue()); }

  private:
    uint8* d_pushback_stack;
    uint32 d_pushback_length;
    uint32 d_pushback_size;
  };

}

#endif

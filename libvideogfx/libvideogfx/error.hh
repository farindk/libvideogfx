/********************************************************************************
  error.hh

  purpose:
    Error handling stuff: exception handling and error message display.

  notes:

  to do:

  author(s):
   - Dirk Farin, dirk.farin@gmx.de

  modifications:
   29/Sep/1999 - Dirk Farin
    - ShowMessage() can now be given a Excpt_Base argument. This allows
      the MessageDisplay to selectively enable/disable the display of
      some error classes.
   20/Sep/1999 - Dirk Farin
    - complete cleanup, ShowNote() removed
   03/Jan/1999 - Dirk Farin
    - ShowNote()
   28/Dec/1998 - Dirk Farin
    - new implementation based on exceptions
   15/Nov/1998 - Dirk Farin
    - first implementation
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

#ifndef LIBVIDEOGFX_ERROR_HH
#define LIBVIDEOGFX_ERROR_HH

// We need: __ASSERT_FUNCTION
#  include <assert.h>

namespace videogfx {

  /* Severity of a message or an exception. Not all severity levels are allowed
     in exceptions. "ErrSev_Note" must not be used in exceptions as the normal
     program flow should not be interrupted in this case.
  */

  enum ErrorSeverity {
    ErrSev_Note,    // Things that are worth noting but that do not have any implications for execution.
    ErrSev_Warning, // Explains why some things may not be as expected and warn about things that may go wrong.
    ErrSev_Error,   // The usual stuff like "file not found".
    ErrSev_Critical,// Several unexpected errors occurred so something is going seriously wrong.
    ErrSev_Assertion,// This should never never never go wrong. Even if the input is completely damaged.
  };



  /* A MessageDisplay is the place where warnings and errors will be displayed on.
   */

  class MessageDisplay
  {
  public:
    virtual ~MessageDisplay() { }

    virtual void ShowMessage(ErrorSeverity,const char* text) = 0;
    virtual void ShowMessage(const class Excpt_Base&) = 0;


    // Message output on the standard display.

    static void Show(ErrorSeverity,const char* text);
    static void Show(const class Excpt_Base&);
    static void SetStandardDisplay(MessageDisplay*);

  private:
    static MessageDisplay* std_msgdisplay;
  };






  class Excpt_Base
  {
  public:
    Excpt_Base(ErrorSeverity);
    virtual ~Excpt_Base() { }

    virtual int GetText(char*,int maxChars) const = 0;

    enum ErrorSeverity m_severity;
  };


  class Excpt_Text : public Excpt_Base
  {
  public:
    Excpt_Text(ErrorSeverity sev);  // create with empty text
    Excpt_Text(ErrorSeverity sev,const char* text);

    void SetText(const char*);
    void AppendText(const char*);
    int GetText(char*,int maxChars) const;

  private:
    const static unsigned int c_MaxTextLen = 500;
    char d_text[c_MaxTextLen+1];
  };


  class Excpt_Assertion : public Excpt_Text
  {
  public:
    Excpt_Assertion(const char* expr,const char* file, const char* function,int line);
  };

  class Excpt_NotImplemented : public Excpt_Text
  {
  public:
    Excpt_NotImplemented(const char* file, int line);
  };

}

#ifdef NDEBUG
#  define Assert(expr)
#  define AssertDescr(expr,descr)
#else
#  if defined(__ASSERT_FUNCTION) && defined(__STRING)
#    define Assert(expr) \
  (void)(expr ? 0 : (throw Excpt_Assertion(__STRING(expr),__FILE__,__ASSERT_FUNCTION,__LINE__),1) );
#    define AssertDescr(expr,descr) \
 (void)(expr ? 0 : (throw Excpt_Assertion(descr,__FILE__,__ASSERT_FUNCTION,__LINE__),1) );
#  else
#    define Assert(expr) \
 (void)(expr ? 0 : (throw Excpt_Assertion("no string information",__FILE__, \
                                          "no function information",__LINE__),1) );
#    define AssertDescr(expr,descr) \
 (void)(expr ? 0 : (throw Excpt_Assertion(descr,__FILE__,"no function information",__LINE__),1) );
#  endif
#endif

#define NotImplemented throw Excpt_NotImplemented(__FILE__,__LINE__);


#endif

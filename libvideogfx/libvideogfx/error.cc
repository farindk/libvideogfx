/********************************************************************************
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

#include "error.hh"
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <iostream>

namespace videogfx {
  using namespace std;

  static class MessageDisplay_cerr : public MessageDisplay
  {
  public:
    void ShowMessage(ErrorSeverity severity,const char* text)
    {
      cout.flush(); /* Flush program output. Do this for better sync of
                       debug and error display. */

      switch(severity)
        {
          //case ErrSev_Empty:     cerr << "EMPTY-ERROR ! This should not occur !\n"; exit(10); break;
        case ErrSev_Note:      cerr << "Note: ";    break;
        case ErrSev_Warning:   cerr << "Warning: "; break;
        case ErrSev_Error:     cerr << "Error: ";   break;
        case ErrSev_Critical:  cerr << "CRITICAL ERROR: "; break;
        case ErrSev_Assertion: cerr << "ASSERTION FAILED: "; break;
        }

      cerr << text << endl;
    }

    void ShowMessage(const Excpt_Base& e)
    {
      char buf[1000]; // TODO: replace me
      e.GetText(buf,1000);

      ShowMessage(e.m_severity , buf);
    }
  } msgdisplay_cerr;


  void MessageDisplay::Show(ErrorSeverity sev,const char* text)
  {
    assert(std_msgdisplay);
    std_msgdisplay->ShowMessage(sev,text);
  }

  void MessageDisplay::Show(const Excpt_Base& excpt)
  {
    assert(std_msgdisplay);
    std_msgdisplay->ShowMessage(excpt);
  }

  void MessageDisplay::SetStandardDisplay(MessageDisplay* disp)
  {
    assert(disp);
    std_msgdisplay = disp;
  }

  MessageDisplay* MessageDisplay::std_msgdisplay = &msgdisplay_cerr;





  Excpt_Base::Excpt_Base(ErrorSeverity sev)
    : m_severity(sev)
  {
    assert(m_severity != ErrSev_Note);
  }



  Excpt_Text::Excpt_Text(ErrorSeverity sev)
    : Excpt_Base(sev)
  {
    d_text[0] = 0;
  }


  Excpt_Text::Excpt_Text(ErrorSeverity sev,const char* text)
    : Excpt_Base(sev)
  {
    assert(strlen(text) <= c_MaxTextLen);
    strcpy(d_text,text);
  }


  void Excpt_Text::SetText(const char* text)
  {
    assert(strlen(text) <= c_MaxTextLen);
    strcpy(d_text,text);
  }


  void Excpt_Text::AppendText(const char* text)
  {
    assert(strlen(text)+strlen(d_text) <= c_MaxTextLen);
    strcat(d_text,text);
  }


  int Excpt_Text::GetText(char* buf,int maxChars) const
  {
    if (strlen(d_text)==0)
      {
	const char* notext = "<unspecified error>";
	assert(maxChars > strlen(notext));
	strcpy(buf,notext);
	return strlen(notext);
      }

    if (strlen(d_text)<maxChars)
      strcpy(buf,d_text);
    else
      {
	strncpy(buf,d_text,maxChars-5);
	buf[maxChars-5]=0;
	strcat(buf," ...");
	return maxChars;
      }

    return strlen(buf);
  }



  Excpt_Assertion::Excpt_Assertion(const char* expr,const char* file, const char* function,int line)
    : Excpt_Text(ErrSev_Assertion)
  {
    char buf[1000];   // TODO: replace me
    sprintf(buf,
	    "file '%s', '%s', line %d (%s).\n",
	    file,function,line,expr);

    SetText(buf);
  }


  Excpt_NotImplemented::Excpt_NotImplemented(const char* file, int line)
    : Excpt_Text(ErrSev_Critical)
  {
    char buf[1000];
    sprintf(buf,"NOT-IMPLEMENTED-YET point reached in file '%s', line %d.\n",file,line);
    SetText(buf);
  }

}

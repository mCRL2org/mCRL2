// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file xstceditor.h

#ifndef MCRL2XI_XSTCEDITOR_H_
#define MCRL2XI_XSTCEDITOR_H_

#ifdef MCRL2_WITH_WXSTC

#include <wx/stc/stc.h>
class xStcEditor: public wxStyledTextCtrl
{
  public:
    xStcEditor(wxWindow* parent, wxWindowID id);
    void OnxStcUIUpdate(wxStyledTextEvent& /*event*/);
    void OnCopy(wxCommandEvent& /*event*/);
    void OnPaste(wxCommandEvent& /*event*/);
    void OnSelectAll(wxCommandEvent& /*event*/);
    /* Editor focus is determined by repaint, since EVT focus make the carot invisible */
    void Focus(wxStyledTextEvent& /* event*/);
    void OnRedo(wxCommandEvent& /*event*/);
    void OnUndo(wxCommandEvent& /*event*/);
    void OnCut(wxCommandEvent& /*event*/);
    void OnDelete(wxCommandEvent& /*event*/);
    void SetFileInUse(wxString filename);
    wxString GetFileInUse();
    int getWrapWordCode()
    {
      return wxSTC_WRAP_WORD;
    }

    int getWrapNoneCode()
    {
      return wxSTC_WRAP_NONE;
    }

    void SetInsertionPoint(long pos )
    {
      wxStyledTextCtrl::GotoPos(pos);
    }

    void Replace(long from, long to, const wxString& value)
    {
      SetSelection( from, to );
      ReplaceSelection(value);
    }

  private:
    wxString fileInUse;
    //void OnLoseFocus(wxFocusEvent& evt);
    //void OnGainFocus(wxFocusEvent& evt);

    DECLARE_EVENT_TABLE()
};
#else

#include "mytextctrl.h"

class xStcEditor: public myTextControl
{
  public:
    xStcEditor(wxWindow* parent, wxWindowID id);

    /* Copy and paste events are dealth with in myTextControl
     */
    void OnDelete(wxCommandEvent& /*event*/);
    void SetFileInUse(wxString filename);
    wxString GetFileInUse();

    /* Function for compatibility with wxSTC*/
    void SetSavePoint();
    wxString GetText();
    int GetWrapMode();
    void SetWrapMode(int);
    int getWrapWordCode();
    int getWrapNoneCode();

    int GetSelectionStart();
    int GetSelectionEnd();
    int FindText(int begin, int end, wxString s);
    int GetTextLength();

  private:
    wxString fileInUse;
    void OnTextChange(wxCommandEvent& /* ev */);
    void OnSetFocus(wxFocusEvent& /*evt*/);
    void OnKillFocus(wxFocusEvent& /*evt*/);

    DECLARE_EVENT_TABLE()
};
#endif



/* Events that can be send by xStcEditor */
DECLARE_EVENT_TYPE(wxEVT_UPDATE_EDITOR_FOCUS, 7777)
DECLARE_EVENT_TYPE(wxEVT_SETSTATUSTEXT, 7777)

#define EVT_UPDATE_EDITOR_FOCUS(id, fn) \
  DECLARE_EVENT_TABLE_ENTRY( \
                             wxEVT_UPDATE_EDITOR_FOCUS, id, wxID_ANY, \
                             (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
                             (wxObject *) NULL \
                           ),

#define EVT_SETSTATUSTEXT(id, fn) \
  DECLARE_EVENT_TABLE_ENTRY( \
                             wxEVT_SETSTATUSTEXT, id, wxID_ANY, \
                             (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
                             (wxObject *) NULL \
                           ),

#endif

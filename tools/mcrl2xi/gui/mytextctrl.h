// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mytextctrl.h

#ifndef MCRL2XI_MYTEXTCONTROL_H_
#define MCRL2XI_MYTEXTCONTROL_H_

#include "wx/wx.h"

class myTextControl : public wxTextCtrl
{
  public:
    myTextControl(wxWindow* parent,
                  wxWindowID id,
                  const wxString& value = wxEmptyString,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = 0,
                  const wxValidator& validator = wxDefaultValidator,
                  const wxString& name = wxTextCtrlNameStr);
  private:
    void OnSetFocus(wxFocusEvent& /*evt*/);
    void OnKillFocus(wxFocusEvent& /*evt*/);

    void OnCopy(wxCommandEvent& /*event*/);
    void OnPaste(wxCommandEvent& /*event*/);
    void OnSelectAll(wxCommandEvent& /*event*/);
    void OnRedo(wxCommandEvent& /*event*/);
    void OnUndo(wxCommandEvent& /*event*/);
    void OnCut(wxCommandEvent& /*event*/);
    void OnDelete(wxCommandEvent& /*event*/);

    DECLARE_EVENT_TABLE()
};

/* Events that can be send by xStcEditor */
DECLARE_EVENT_TYPE(wxEVT_MYTXTCTRL_FOCUS, 7777)

#define EVT_MYTXTCTRL_FOCUS(id, fn) \
  DECLARE_EVENT_TABLE_ENTRY( \
                             wxEVT_MYTXTCTRL_FOCUS, id, wxID_ANY, \
                             (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
                             (wxObject *) NULL \
                           ),

#endif

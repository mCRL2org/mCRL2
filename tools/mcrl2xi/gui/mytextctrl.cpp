// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mytextctrl.cpp

#include "mytextctrl.h"

DEFINE_EVENT_TYPE(wxEVT_MYTXTCTRL_FOCUS)

BEGIN_EVENT_TABLE(myTextControl, wxTextCtrl)
  EVT_SET_FOCUS(myTextControl::OnSetFocus)
  EVT_KILL_FOCUS(myTextControl::OnKillFocus)

  EVT_MENU(wxID_COPY,        myTextControl::OnCopy)
  EVT_MENU(wxID_SELECTALL,   myTextControl::OnSelectAll)
  EVT_MENU(wxID_PASTE,       myTextControl::OnPaste)
  EVT_MENU(wxID_UNDO,        myTextControl::OnUndo)
  EVT_MENU(wxID_REDO,        myTextControl::OnRedo)
  EVT_MENU(wxID_CUT,         myTextControl::OnCut)
  EVT_MENU(wxID_CLEAR,       myTextControl::OnDelete)

END_EVENT_TABLE()

myTextControl::myTextControl(wxWindow* parent,
                             wxWindowID id,
                             const wxString& value,
                             const wxPoint& pos ,
                             const wxSize& size ,
                             long style ,
                             const wxValidator& validator ,
                             const wxString& name)
  :wxTextCtrl(parent, id, value, pos, size, style, validator, name)
{
};

void myTextControl::OnSetFocus(wxFocusEvent& /*evt*/)
{
  wxCommandEvent eventCustom(wxEVT_MYTXTCTRL_FOCUS);
  /* Send pointer of txtctrl window */
  eventCustom.SetClientData(this);
  wxPostEvent(this->GetParent(), eventCustom);
}

void myTextControl::OnKillFocus(wxFocusEvent& /*evt*/)
{
  wxCommandEvent eventCustom(wxEVT_MYTXTCTRL_FOCUS);
  /* Send NO txtctrl window */
  eventCustom.SetClientData(NULL);
  wxPostEvent(this->GetParent(), eventCustom);
}

void myTextControl::OnCopy(wxCommandEvent& /*event*/)
{
  Copy();
};

void myTextControl::OnPaste(wxCommandEvent& /*event*/)
{
  Paste();
};

void myTextControl::OnSelectAll(wxCommandEvent& /*event*/)
{
  SetSelection(0, static_cast<long>(GetValue().Length()));
};

void myTextControl::OnRedo(wxCommandEvent& /*event*/)
{
  Redo();
};

void myTextControl::OnUndo(wxCommandEvent& /*event*/)
{
  Undo();
};

void myTextControl::OnCut(wxCommandEvent& /*event*/)
{
  Cut();
};

void myTextControl::OnDelete(wxCommandEvent& /*event*/)
{
  long int i, j;
  GetSelection(&i,&j);
  if (i != j)
  {
    Remove(i,j);
  }
  else
  {
    Remove(i,i+1);
  }
};

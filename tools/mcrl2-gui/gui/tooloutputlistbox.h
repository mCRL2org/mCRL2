// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tooloutputlistbox.h


#ifndef MCRL2_TOOLOutPutTextCtrl_H_
#define MCRL2_TOOLOutPutTextCtrl_H_

#include <wx/filedlg.h>
#include <wx/clipbrd.h>
#include <wx/dataobj.h>
#include <wx/textfile.h>
#include <wx/textctrl.h>


#define ID_RUN_LISTBOX  1502
#define ID_GO_BACK_TO_CONFIGURATION 1504
#define ID_RUN_AND_NOT_CLEAR 1505
#define ID_RUN_AND_CLEAR 1506
#define ID_SELECT_NONE 1508

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE(wxEVT_MY_RUN_PROCESS, 7777)
END_DECLARE_EVENT_TYPES()

DEFINE_EVENT_TYPE(wxEVT_MY_RUN_PROCESS)

// it may also be convenient to define an event table macro for this event type
#define EVT_MY_RUN_PROCESS(id, fn) \
  DECLARE_EVENT_TABLE_ENTRY( \
                             wxEVT_MY_RUN_PROCESS, id, wxID_ANY, \
                             (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
                             (wxObject *) NULL \
                           ),

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE(wxEVT_UPDATE_FOCUS, 7777)
END_DECLARE_EVENT_TYPES()

DEFINE_EVENT_TYPE(wxEVT_UPDATE_FOCUS)

// it may also be convenient to define an event table macro for this event type
#define EVT_UPDATE_FOCUS(id, fn) \
  DECLARE_EVENT_TABLE_ENTRY( \
                             wxEVT_UPDATE_FOCUS, id, wxID_ANY, \
                             (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
                             (wxObject *) NULL \
                           ),


class OutPutTextCtrlBase : public wxTextCtrl
{
  public:
    OutPutTextCtrlBase(wxWindow* parent, wxWindowID id, const wxPoint& pos =
                         wxDefaultPosition, const wxSize& size = wxDefaultSize) :
      wxTextCtrl(parent, id, wxEmptyString ,pos, size, wxTE_MULTILINE | wxTE_RICH2 | wxTE_RICH)
    {
      /*wxFont font(wxNORMAL_FONT->GetPointSize(), wxMODERN, wxFONTSTYLE_NORMAL,
          wxLIGHT, false);
      this->SetFont(font); */
    }

    void
    CopyLine()
    {
      // Write selected items to the clipboard
      if (wxTheClipboard->Open())
      {
        this->Copy();
      }
    }

    void
    Save()
    {
      wxFileDialog* fd = new wxFileDialog(this, wxT("Choose a file"), wxT(""),
                                          wxT(""), wxT("*.*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);
      if (fd->ShowModal() == wxID_OK)
      {
        wxString sfile = fd->GetPath();

        if (wxFile::Exists(sfile))
        {
          wxRemoveFile(sfile);
        }

        this->SaveFile(sfile, wxTEXT_TYPE_ANY);

      }
    }

    void
    SelectAll()
    {
      // If both parameters are equal to -1 all text in the control is selected.
      this->SetSelection(-1 , -1);
    }

    void OnFocus(wxFocusEvent& /*event*/)
    {
      wxCommandEvent eventCustom(wxEVT_UPDATE_FOCUS);
      /* Send pointer of focused window */
      eventCustom.SetClientData(this);
      wxPostEvent(this->GetParent(), eventCustom);
    }

    void OnKillFocus(wxFocusEvent& /*event*/)
    {
      wxCommandEvent eventCustom(wxEVT_UPDATE_FOCUS);
      /* Send NO Focus */
      eventCustom.SetClientData(NULL);
      wxPostEvent(this->GetParent(), eventCustom);
    }


    DECLARE_EVENT_TABLE()
};
BEGIN_EVENT_TABLE(OutPutTextCtrlBase, wxTextCtrl)
  EVT_SET_FOCUS(OutPutTextCtrlBase::OnFocus)
  EVT_KILL_FOCUS(OutPutTextCtrlBase::OnKillFocus)
END_EVENT_TABLE()

class ToolOutPutTextCtrlBase : public OutPutTextCtrlBase
{
  public:

    ToolOutPutTextCtrlBase(wxWindow* parent, wxWindowID id, const wxPoint& pos =
                             wxDefaultPosition, const wxSize& size = wxDefaultSize) :
      OutPutTextCtrlBase(parent, id, pos, size)
    {}

    void
    Run()
    {
      if (cognizance)
      {
        wxCommandEvent eventCustom(wxEVT_MY_PROCESS_RUN);
        wxPostEvent(cognizance, eventCustom);
      }
    }

  protected:
    wxEvtHandler* cognizance;

};

class TOutputTextCtrlMenu: public wxMenu
{
  public:
    ToolOutPutTextCtrlBase* p;

    TOutputTextCtrlMenu(ToolOutPutTextCtrlBase* parent)
    {
      this->Append(wxID_SELECTALL, wxT("Select All...\tCtrl-A"));
      this->Append(wxID_COPY, wxT("Copy Selection...\tCtrl-C"));
      this->Append(wxID_SAVE, wxT("Save...\tCtrl-S"));
      this->AppendSeparator();
      this->Append(wxID_CLEAR, wxT("Clear output"));

      p = parent;

    };

  protected:
    void
    OnClear(wxCommandEvent& /*event*/)
    {
      p->Clear();
    }

    void
    OnCopyLine(wxCommandEvent& /*event*/)
    {
      p->CopyLine();
    }

    void
    OnSave(wxCommandEvent& /*event*/)
    {
      p->Save();
    }

    void
    OnSelectAll(wxCommandEvent& /*event*/)
    {
      p->SelectAll();
    }

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(TOutputTextCtrlMenu, wxMenu)
  EVT_MENU(wxID_CLEAR, TOutputTextCtrlMenu::OnClear)
  EVT_MENU(wxID_SAVE, TOutputTextCtrlMenu::OnSave)
  EVT_MENU(wxID_COPY, TOutputTextCtrlMenu::OnCopyLine)
  EVT_MENU(wxID_SELECTALL, TOutputTextCtrlMenu::OnSelectAll)
END_EVENT_TABLE()

class OutPutTextCtrl : public OutPutTextCtrlBase
{
  public:
    OutPutTextCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos =
                     wxDefaultPosition, const wxSize& size = wxDefaultSize)
      :
      OutPutTextCtrlBase(parent, id, pos, size)
    {}

    void
    OnRightClick(wxMouseEvent& /*event*/)
    {

      TOutputTextCtrlMenu* m = new TOutputTextCtrlMenu((ToolOutPutTextCtrlBase*) this);
      PopupMenu(m);
    }

    DECLARE_EVENT_TABLE()

};
BEGIN_EVENT_TABLE(OutPutTextCtrl, OutPutTextCtrlBase)
  EVT_RIGHT_DOWN(OutPutTextCtrl::OnRightClick)
END_EVENT_TABLE()

class ToolOutputTextCtrlMenu : public TOutputTextCtrlMenu
{
  public:
    ToolOutPutTextCtrlBase* p;

    ToolOutputTextCtrlMenu(ToolOutPutTextCtrlBase* parent) :
      TOutputTextCtrlMenu(parent)
    {
      this->PrependSeparator();
      this->Prepend(ID_GO_BACK_TO_CONFIGURATION, wxT("Go Back to Configuration"));
      this->Prepend(ID_RUN_AND_NOT_CLEAR, wxT("Re-Run [Keep Output]"));
      this->Prepend(ID_RUN_AND_CLEAR, wxT("Re-Run"));

      p = parent;
    }

    void
    OnGoBackToConfiguration(wxCommandEvent& /*event*/)
    {
      ((wxAuiNotebook*)(p->GetParent()->GetParent()))->SetSelection(0);
    }

    void
    OnRun(wxCommandEvent& /*event*/)
    {
      p->Run();
    }

    void
    OnRunAndClear(wxCommandEvent& /*event*/)
    {
      p->Clear();
      p->Run();
    }

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(ToolOutputTextCtrlMenu, TOutputTextCtrlMenu)
  EVT_MENU(ID_GO_BACK_TO_CONFIGURATION, ToolOutputTextCtrlMenu::OnGoBackToConfiguration)
  EVT_MENU(ID_RUN_AND_NOT_CLEAR, ToolOutputTextCtrlMenu::OnRun)
  EVT_MENU(ID_RUN_AND_CLEAR, ToolOutputTextCtrlMenu::OnRunAndClear)
END_EVENT_TABLE()

class ToolOutPutTextCtrl : public ToolOutPutTextCtrlBase
{
  public:

    ToolOutPutTextCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos =
                         wxDefaultPosition, const wxSize& size = wxDefaultSize) :
      ToolOutPutTextCtrlBase(parent, id, pos, size)
    {
    }

    void
    OnRightClick(wxMouseEvent& /*event*/)
    {

      ToolOutputTextCtrlMenu* m = new ToolOutputTextCtrlMenu(this);
      PopupMenu(m);
    }

    void
    SetRunCognizance(wxEvtHandler* dest)
    {
      cognizance = dest;
    }

    DECLARE_EVENT_TABLE()
};
BEGIN_EVENT_TABLE(ToolOutPutTextCtrl, ToolOutPutTextCtrlBase)
  EVT_RIGHT_DOWN(ToolOutPutTextCtrl::OnRightClick)
END_EVENT_TABLE()

#endif /* TOOLOutPutTextCtrl_H_ */

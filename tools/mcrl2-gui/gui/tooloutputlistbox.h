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


#define ID_CLEAR_TEXTCTRL  1500
#define ID_SAVE_TEXTCTRL   1501
#define ID_RUN_LISTBOX  1502
#define ID_COPY_LINES_TO_CLIPBOARD 1503
#define ID_GO_BACK_TO_CONFIGURATION 1504
#define ID_RUN_AND_NOT_CLEAR 1505
#define ID_RUN_AND_CLEAR 1506
#define ID_SELECT_ALL 1507
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
  OutPutTextCtrlBase(wxWindow *parent, wxWindowID id, const wxPoint& pos =
          wxDefaultPosition, const wxSize& size = wxDefaultSize) :
        wxTextCtrl(parent, id, wxEmptyString ,pos, size, wxTE_MULTILINE)
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
    wxFileDialog *fd = new wxFileDialog(this, wxT("Choose a file"), wxT(""),
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
	  this->SetSelection( -1 , -1 );
  }

  void OnFocus(wxFocusEvent &/*event*/){
    wxCommandEvent eventCustom(wxEVT_UPDATE_FOCUS);
    /* Send pointer of focused window */
    eventCustom.SetClientData(this);
    wxPostEvent(this->GetParent(), eventCustom);
  }

  void OnKillFocus(wxFocusEvent &/*event*/){
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
END_EVENT_TABLE ()

class ToolOutPutTextCtrlBase : public OutPutTextCtrlBase
{
  public:

    ToolOutPutTextCtrlBase(wxWindow *parent, wxWindowID id, const wxPoint& pos =
        wxDefaultPosition, const wxSize& size = wxDefaultSize) :
          OutPutTextCtrlBase(parent, id, pos, size)
    {}

    void
    Run()
    {
      if(cognizance){
        wxCommandEvent eventCustom(wxEVT_MY_PROCESS_RUN);
        wxPostEvent(cognizance, eventCustom);
      }
    }

  protected:
    wxEvtHandler *cognizance;

};

class TOutputTextCtrlMenu: public wxMenu
{
  public:
    ToolOutPutTextCtrlBase *p;

    TOutputTextCtrlMenu(ToolOutPutTextCtrlBase *parent)
    {
      this->Append(ID_SELECT_ALL, wxT("Select All...\tCtrl-A"));
      this->Append(ID_COPY_LINES_TO_CLIPBOARD, wxT("Copy Selection...\tCtrl-C"));
      this->Append(ID_SAVE_TEXTCTRL, wxT("Save...\tCtrl-S"));
      this->AppendSeparator();
      this->Append(ID_CLEAR_TEXTCTRL, wxT("Clear output"));

      p = parent;

    };

  protected:
    void
    OnClear(wxCommandEvent &/*event*/)
    {
      p->Clear();
    }

    void
    OnCopyLine(wxCommandEvent &/*event*/)
    {
      p->CopyLine();
    }

    void
    OnSave(wxCommandEvent &/*event*/)
    {
      p->Save();
    }

    void
    OnSelectAll(wxCommandEvent &/*event*/)
    {
      p->SelectAll();
    }

    DECLARE_EVENT_TABLE()
};

  BEGIN_EVENT_TABLE(TOutputTextCtrlMenu, wxMenu)
    EVT_MENU(ID_CLEAR_TEXTCTRL, TOutputTextCtrlMenu::OnClear )
    EVT_MENU(ID_SAVE_TEXTCTRL, TOutputTextCtrlMenu::OnSave )
    EVT_MENU(ID_COPY_LINES_TO_CLIPBOARD, TOutputTextCtrlMenu::OnCopyLine)
    EVT_MENU(ID_SELECT_ALL, TOutputTextCtrlMenu::OnSelectAll)
  END_EVENT_TABLE ()

class OutPutTextCtrl : public OutPutTextCtrlBase
{
  public:
    OutPutTextCtrl(wxWindow *parent, wxWindowID id, const wxPoint& pos =
        wxDefaultPosition, const wxSize& size = wxDefaultSize)
    :
      OutPutTextCtrlBase(parent, id, pos, size)
    {}

      void
      OnRightClick(wxMouseEvent& /*event*/)
      {

        TOutputTextCtrlMenu *m = new TOutputTextCtrlMenu( (ToolOutPutTextCtrlBase*) this);
        PopupMenu(m);
      }

      void
      onKeyDown(wxKeyEvent& evt)
      {

        //std::cout << "Pressed key {" << evt.GetKeyCode() << "}\n";

        switch (evt.GetKeyCode())
        {
          case 65: //65 == a or A
            if (evt.ControlDown())
            {
              SelectAll();
            }
            break;
          case 67: //67 == c or C
            if (evt.ControlDown())
            {
              CopyLine();
            }
            break;
          case 83: //83 == s or S
            if (evt.ControlDown())
            {
              Save();
            }
            break;
        }

        evt.Skip();

      }


      DECLARE_EVENT_TABLE()

};
BEGIN_EVENT_TABLE(OutPutTextCtrl, OutPutTextCtrlBase)
  EVT_KEY_DOWN( OutPutTextCtrl::onKeyDown)
  EVT_RIGHT_DOWN( OutPutTextCtrl::OnRightClick )
END_EVENT_TABLE ()

class ToolOutputTextCtrlMenu : public TOutputTextCtrlMenu
{
  public:
    ToolOutPutTextCtrlBase *p;

    ToolOutputTextCtrlMenu(ToolOutPutTextCtrlBase *parent) :
      TOutputTextCtrlMenu( parent )
    {
      this->PrependSeparator();
      this->Prepend(ID_GO_BACK_TO_CONFIGURATION, wxT("Go Back to Configuration"));
      this->Prepend(ID_RUN_AND_NOT_CLEAR, wxT("Re-Run [Keep Output]"));
      this->Prepend(ID_RUN_AND_CLEAR, wxT("Re-Run"));

      p = parent;
    }

    void
    OnGoBackToConfiguration(wxCommandEvent &/*event*/)
    {
      ((wxAuiNotebook *) (p->GetParent()->GetParent()))->SetSelection(0);
    }

    void
    OnRun(wxCommandEvent &/*event*/)
    {
      p->Run();
    }

    void
    OnRunAndClear(wxCommandEvent &/*event*/)
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
END_EVENT_TABLE ()

class ToolOutPutTextCtrl : public ToolOutPutTextCtrlBase
{
  public:

    ToolOutPutTextCtrl(wxWindow *parent, wxWindowID id, const wxPoint& pos =
        wxDefaultPosition, const wxSize& size = wxDefaultSize) :
      ToolOutPutTextCtrlBase(parent, id, pos, size)
    {
    }

    void
    OnRightClick(wxMouseEvent& /*event*/)
    {

      ToolOutputTextCtrlMenu *m = new ToolOutputTextCtrlMenu(this);
      PopupMenu(m);
    }

  void
  SetRunCognizance(wxEvtHandler *dest)
    {
      cognizance = dest;
    }

  DECLARE_EVENT_TABLE()
};
BEGIN_EVENT_TABLE(ToolOutPutTextCtrl, ToolOutPutTextCtrlBase)
  EVT_KEY_DOWN( OutPutTextCtrl::onKeyDown)
  EVT_RIGHT_DOWN( ToolOutPutTextCtrl::OnRightClick )
END_EVENT_TABLE ()

#endif /* TOOLOutPutTextCtrl_H_ */

/*
 * tooloutputlistbox.h
 *
 *  Created on: Jun 18, 2010
 *      Author: fstapper
 */

#ifndef MCRL2_TOOLOUTPUTLISTBOX_H_
#define MCRL2_TOOLOUTPUTLISTBOX_H_

#include <wx/filedlg.h>
#include <wx/listbox.h>
#include <wx/clipbrd.h>
#include <wx/dataobj.h>

#define ID_CLEAR_LISTBOX  1500
#define ID_SAVE_LISTBOX   1501
#define ID_RUN_LISTBOX  1502
#define ID_COPY_LINES_TO_CLIPBOARD 1503
#define ID_GO_BACK_TO_CONFIGURATION 1504
#define ID_RUN 1505
#define ID_RUN_AND_CLEAR 1506

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

class ToolOutputListBoxBase : public wxListBox
{
  public:

    ToolOutputListBoxBase(wxWindow *parent, wxWindowID id, const wxPoint& pos =
        wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0,
        const wxString choices[] = (const wxString *) NULL, long style =
            wxLB_EXTENDED, const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxListBoxNameStr) :
      wxListBox(parent, id, pos, size, n, choices, style, validator, name)
    {

      wxFont font(wxNORMAL_FONT->GetPointSize(), wxMODERN, wxFONTSTYLE_NORMAL,
          wxLIGHT, false);
      this->SetFont(font);
    }

    void
    CopyLine()
    {
      // Write selected items to the clipboard
      if (wxTheClipboard->Open())
      {
        wxArrayInt selection;
        this->GetSelections(selection);

        wxString s;
        for (size_t i = 0; i < selection.Count(); ++i)
        {
          s << this->GetString((int) selection.Item(i)) << wxT("\n");
        }

        wxTheClipboard->SetData(new wxTextDataObject(s));
        wxTheClipboard->Close();
      }
    }

    void
    Save()
    {
      wxFileDialog *fd = new wxFileDialog(this, wxT("Choose a file"), wxT(""),
          wxT(""), wxT("*.*"), wxSAVE | wxOVERWRITE_PROMPT, wxDefaultPosition);
      if (fd->ShowModal() == wxID_OK)
      {
        wxString sfile = fd->GetPath();

        if (wxFile::Exists(sfile))
        {
          wxRemoveFile(sfile);
        }

        wxFile *f = new wxFile(sfile, wxFile::write);

        for (unsigned int i = 0; i < this->GetCount(); ++i)
        {
          f->Write(this->GetString(i));
          f->Write(wxT("\n"));
        }
      }
    }

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

class ToolOutputListBoxMenu : public wxMenu
{
  public:
    ToolOutputListBoxBase *p;

    ToolOutputListBoxMenu(ToolOutputListBoxBase *parent) :
      wxMenu()
    {
      this->Append(ID_RUN, wxT("Re-Run"));
      this->Append(ID_RUN_AND_CLEAR, wxT("Re-Run and Clear Output"));
      this->Append(ID_GO_BACK_TO_CONFIGURATION, wxT("Go Back to Configuration"));
      this->AppendSeparator();
      this->Append(ID_COPY_LINES_TO_CLIPBOARD, wxT("Copy Selection...\tCtrl-C"));
      this->Append(ID_SAVE_LISTBOX, wxT("Save...\tCtrl-S"));
      this->AppendSeparator();
      this->Append(ID_CLEAR_LISTBOX, wxT("Clear output"));

      p = parent;
    }

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
    OnGoBackToConfiguration(wxCommandEvent &/*event*/)
    {
      ((wxAuiNotebook *) (p->GetParent()))->SetSelection(0);
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

BEGIN_EVENT_TABLE(ToolOutputListBoxMenu, wxMenu)
  EVT_MENU(ID_CLEAR_LISTBOX, ToolOutputListBoxMenu::OnClear )
  EVT_MENU(ID_SAVE_LISTBOX, ToolOutputListBoxMenu::OnSave )
  EVT_MENU(ID_COPY_LINES_TO_CLIPBOARD, ToolOutputListBoxMenu::OnCopyLine)
  EVT_MENU(ID_GO_BACK_TO_CONFIGURATION, ToolOutputListBoxMenu::OnGoBackToConfiguration)
  EVT_MENU(ID_RUN, ToolOutputListBoxMenu::OnRun)
  EVT_MENU(ID_RUN_AND_CLEAR, ToolOutputListBoxMenu::OnRunAndClear)
END_EVENT_TABLE ()

class ToolOutputListBox : public ToolOutputListBoxBase
{
  public:

    ToolOutputListBox(wxWindow *parent, wxWindowID id, const wxPoint& pos =
        wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0,
        const wxString choices[] = (const wxString *) NULL, long style =
            wxLB_EXTENDED, const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxListBoxNameStr) :
      ToolOutputListBoxBase(parent, id, pos, size, n, choices, style,
          validator, name)
    {
    }

    void
    OnCopyLine()
    {
      this->CopyLine();
    }

    void
    OnSave()
    {
      this->Save();
    }

    void
    OnRightClick(wxMouseEvent& /*event*/)
    {

      ToolOutputListBoxMenu *m = new ToolOutputListBoxMenu(this);
      PopupMenu(m);
    }

    void
    onKeyDown(wxKeyEvent& evt)
    {
      //std::cout << "Pressed key {" << evt.GetKeyCode() << "}\n";

      switch (evt.GetKeyCode())
      {
        case 67: //67 == c or C
          if (evt.ControlDown())
          {
            OnCopyLine();
          }
          break;
        case 83: //83 == s or S
          if (evt.ControlDown())
          {
            OnSave();
          }
          break;
      }

      evt.Skip();

    }

  void
  SetRunCognizance(wxEvtHandler *dest)
    {
      cognizance = dest;
    }

  DECLARE_EVENT_TABLE()
};
BEGIN_EVENT_TABLE(ToolOutputListBox, wxListBox)
  EVT_KEY_DOWN( ToolOutputListBox::onKeyDown)
  EVT_RIGHT_DOWN( ToolOutputListBox::OnRightClick )
END_EVENT_TABLE ()

#endif /* TOOLOUTPUTLISTBOX_H_ */

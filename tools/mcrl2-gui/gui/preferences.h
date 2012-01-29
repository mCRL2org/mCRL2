// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file preferences.h


#ifndef MCRL2_GUI_PREFERENCES_H_
#define MCRL2_GUI_PREFERENCES_H_

#include <mimemanager.h>
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/filepicker.h>

#include <iterator>
#include <map>

using namespace std;

class CustomDialog : public wxDialog
{
  private:
    MimeManager* m_mm;
    wxFilePickerCtrl* fp;
    wxTextCtrl* tc;

  public:
    CustomDialog(const wxString& title, wxString ext, wxString cmd, MimeManager* mm)
      : wxDialog(NULL, -1, title, wxDefaultPosition, 
#ifdef __WIN32__
	  wxSize(490, 160)
#else
	  wxSize(490, 140)
#endif
	  )
    {

      m_mm = mm;

      wxPanel* panel = new wxPanel(this, -1);

      wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
      wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);

      new wxStaticBox(panel, wxID_ANY , wxT("Mapping"),
                      wxPoint(5, 5), wxSize(450, 85));
      new wxStaticText(panel, -1,
                       wxT("Extension"), wxPoint(15, 25));
      tc = new wxTextCtrl(panel, wxID_ANY , ext,
                          wxPoint(95, 25));

      if (title.compare(wxT("Edit")) == 0)
      {
        tc->Enable(false);
      }


      new wxStaticText(panel,wxID_ANY,
                       wxT("Command"), wxPoint(15, 55));

      fp = new wxFilePickerCtrl(panel, wxID_ANY,  cmd,
                                wxT("Select a program"), wxT("*.*"),wxPoint(90, 55), wxDefaultSize,
                                wxFLP_USE_TEXTCTRL | wxFLP_OPEN);

      fp->SetMinSize(wxSize(300,30));
      fp->SetTextCtrlProportion(6);
      fp->SetSize(wxSize(350,30));

      wxButton* okButton = new wxButton(this, wxID_OK, wxT("Ok"),
                                        wxDefaultPosition, wxSize(115, 30));
      wxButton* closeButton = new wxButton(this, wxID_CANCEL, wxT("Cancel"),
                                           wxDefaultPosition, wxSize(115, 30));
      hbox->Add(okButton, 1);
      hbox->Add(closeButton, 1, wxLEFT, 5);

      vbox->Add(panel, 1);
#ifdef __WIN32__
	  vbox->AddSpacer(10);
#endif
      vbox->Add(hbox, 0, wxALIGN_CENTER,5);

      SetSizer(vbox);
      Centre();
      ShowModal();

      Destroy();

    }

    void OnOkClick(wxCommandEvent& /*evt*/)
    {
      m_mm->removeExtensionMapping(tc->GetLineText(0));
      m_mm->addExtensionMapping(tc->GetLineText(0) , fp->GetTextCtrlValue());
      EndModal(wxID_OK);
    }
    ;

    void OnCancelClick(wxCommandEvent& /*evt*/)
    {
      EndModal(wxID_CANCEL);
    }

    DECLARE_EVENT_TABLE()

};

BEGIN_EVENT_TABLE(CustomDialog, wxDialog)
  EVT_BUTTON(wxID_OK, CustomDialog::OnOkClick)
  EVT_BUTTON(wxID_CANCEL, CustomDialog::OnCancelClick)
END_EVENT_TABLE()

class Preferences: public wxDialog
{
  public:
    Preferences():wxDialog(NULL, wxID_ANY, wxT("Preferences"), wxDefaultPosition, wxSize(500, 300))
    {

      wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
      SetSizer(sizer);

      wxStaticBoxSizer* box = new wxStaticBoxSizer(wxHORIZONTAL, this, wxT("Editors associated to file extensions"));
      sizer->AddSpacer(10);
      sizer->Add(box, 1, wxEXPAND|wxLEFT|wxRIGHT, 3);
      sizer->AddSpacer(5);


      listview = new wxListView(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VRULES|wxLC_HRULES|wxBORDER_SUNKEN);

      listview->InsertColumn(0, wxT("Extension"), wxLIST_FORMAT_CENTRE);
      listview->InsertColumn(1, wxT("Command"), wxLIST_FORMAT_LEFT);

      map<wxString,wxString> mapping = mm.getExtensionCommandMapping();
      map<wxString,wxString>::iterator b = mapping.begin();

      for (map<wxString,wxString>::iterator i = mapping.begin(); i != mapping.end(); ++i)
      {
        listview->InsertItem(distance(b, i), i->first);
        listview->SetItem(distance(b, i), 1, i->second);
      }

      box->AddSpacer(10);
      box->Add(listview, 1, wxALIGN_LEFT|wxEXPAND, 5);
      box->AddSpacer(10);
      sizer = new wxBoxSizer(wxVERTICAL);
      box->Add(sizer);
      sizer->Add(new wxButton(this, wxID_NEW), 0, wxTOP, 5);

      m_edit_button = new wxButton(this, wxID_EDIT);
      m_delete_button = new wxButton(this, wxID_DELETE);

      sizer->Add(m_edit_button, 0, wxTOP, 5);
      sizer->Add(m_delete_button, 0, wxTOP, 5);


      sizer = new wxBoxSizer(wxHORIZONTAL);
      sizer->Add(new wxButton(this, wxID_SAVE), 0, wxRIGHT, 5);
      sizer->Add(new wxButton(this, wxID_CANCEL), 0, wxRIGHT, 5);

      GetSizer()->Add(sizer, 0, wxALL|wxRIGHT, 3);

      m_edit_button->Enable(false);
      m_delete_button->Enable(false);

      CentreOnParent();

      ShowModal();
      Destroy();
    }

    void UpdateItems()
    {
      listview->ClearAll();
      listview->InsertColumn(0, wxT("Extension"), wxLIST_FORMAT_CENTRE);
      listview->InsertColumn(1, wxT("Command"), wxLIST_FORMAT_LEFT);

      map<wxString,wxString> mapping = mm.getExtensionCommandMapping();
      map<wxString,wxString>::iterator b = mapping.begin();

      for (map<wxString,wxString>::iterator i = mapping.begin(); i != mapping.end(); ++i)
      {
        listview->InsertItem(distance(b, i), i->first);
        listview->SetItem(distance(b, i), 1, i->second);
      }

    }

    void OnNewClick(wxCommandEvent& /*evt*/)
    {
      long i = listview->GetFocusedItem();
      if (i != -1)
      {
        map<wxString, wxString>  m = mm.getExtensionCommandMapping();
        CustomDialog
        *cd =
          new CustomDialog(
          wxT("New"),
          wxT(""),
          wxT(""),
          &mm);
        cd->Show(true);
        UpdateItems();
      }
    }

    void OnDeleteClick(wxCommandEvent& /*evt*/)
    {
      long i = listview->GetFocusedItem();

      if (i != -1)
      {
        wxString s = listview->GetItemText(i);
        wxMessageDialog* dial = new wxMessageDialog(NULL,
            wxT("Are you sure to remove the command associated for \"") + s + wxT("\"?"), wxT("Question"),
            wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
        if (dial->ShowModal() == wxID_YES)
        {
          mm.removeExtensionMapping(s);

          UpdateItems();
        }
      }
    }

    void OnEditClick(wxCommandEvent& /*evt*/)
    {
      long i = listview->GetFocusedItem();
      if (i != -1)
      {
        map<wxString, wxString>  m = mm.getExtensionCommandMapping();
        CustomDialog
        *cd =
          new CustomDialog(
          wxT("Edit"),
          listview->GetItemText(i),
          m.find(listview->GetItemText(i))->second,
          &mm);
        cd->Show(true);
        UpdateItems();
      }
    }

    void OnSaveClick(wxCommandEvent& /*evt*/)
    {
      mm.saveExtensionMapping();
      EndModal(wxID_SAVE);
    }

    void OnCancelClick(wxCommandEvent& /*evt*/)
    {
      EndModal(wxID_CANCEL);
    }

    /*void OnListBoxClick(wxListEvent& evt){
      cout << "blaat" << endl;
      UpdateButtons();
    }*/

  private:

    void OnSelect(wxListEvent& /*evt*/)
    {
      m_edit_button->Enable(true);
      m_delete_button->Enable(true);
    }

    void OnDeSelect(wxListEvent& /*evt*/)
    {
      m_edit_button->Enable(false);
      m_delete_button->Enable(false);
    }

    wxListView* listview;

    wxButton* m_edit_button;
    wxButton* m_delete_button;

    MimeManager mm;

    DECLARE_EVENT_TABLE()

};

BEGIN_EVENT_TABLE(Preferences, wxDialog)
  EVT_BUTTON(wxID_NEW, Preferences::OnNewClick)
  EVT_BUTTON(wxID_DELETE, Preferences::OnDeleteClick)
  EVT_BUTTON(wxID_EDIT, Preferences::OnEditClick)
  EVT_BUTTON(wxID_SAVE, Preferences::OnSaveClick)
  EVT_BUTTON(wxID_CANCEL, Preferences::OnCancelClick)

  EVT_LIST_ITEM_SELECTED(wxID_ANY, Preferences::OnSelect)
  EVT_LIST_ITEM_DESELECTED(wxID_ANY, Preferences::OnDeSelect)

END_EVENT_TABLE()


#endif /* MCRL2_GUI_PREFERENCES_H_ */

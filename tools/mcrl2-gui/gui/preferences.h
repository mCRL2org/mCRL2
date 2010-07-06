/*
 * preferences.h
 *
 *  Created on: Jul 5, 2010
 *      Author: fstapper
 */

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
    wxFilePickerCtrl *fp;
    wxTextCtrl *tc;

public:
    CustomDialog(const wxString & title, wxString ext, wxString cmd, MimeManager *mm)
           : wxDialog(NULL, -1, title, wxDefaultPosition, wxSize(400, 140))
    {

      m_mm = mm;

      wxPanel *panel = new wxPanel(this, -1);

        wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
        wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);

        new wxStaticBox(panel, -1, wxT("Mapping"),
            wxPoint(5, 5), wxSize(390, 85));
        new wxStaticText(panel, -1,
            wxT("Extension"), wxPoint(15, 25));
        tc = new wxTextCtrl(panel, -1, ext,
            wxPoint(95, 25));

        if(title.compare(wxT("Edit")) == 0){
          tc->Enable(false);
        }


        new wxStaticText(panel, -1,
            wxT("Command"), wxPoint(15, 55));

        fp = new wxFilePickerCtrl(panel, wxID_ANY,  cmd,
            wxT("Select a program"), wxT("*.*"), wxPoint(95 , 55), wxDefaultSize,
            wxFLP_USE_TEXTCTRL | wxFLP_OPEN );

        fp->SetSize(wxSize(250,10));

        //fp->SetLabel(wxString(
        //    (*i).m_flag.c_str(), wxConvUTF8));

        wxButton *okButton = new wxButton(this, wxID_OK, wxT("Ok"),
            wxDefaultPosition, wxSize(95, 30));
        wxButton *closeButton = new wxButton(this, wxID_CANCEL, wxT("Cancel"),
            wxDefaultPosition, wxSize(95, 30));

        hbox->Add(okButton, 1);
        hbox->Add(closeButton, 1, wxLEFT, 5);

        vbox->Add(panel, 1);
        vbox->Add(hbox, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

        SetSizer(vbox);

        Centre();
        ShowModal();

        Destroy();

    }

    void OnOkClick(wxCommandEvent& evt){
      m_mm->removeExtensionMapping( tc->GetLineText(0) );
      m_mm->addExtensionMapping( tc->GetLineText(0) , fp->GetTextCtrlValue() );
      EndModal(wxID_OK);
    }
    ;

    void OnCancelClick(wxCommandEvent& evt){
      EndModal(wxID_CANCEL);
    }

    DECLARE_EVENT_TABLE()

};

BEGIN_EVENT_TABLE(CustomDialog, wxDialog)
  EVT_BUTTON(wxID_OK, CustomDialog::OnOkClick)
  EVT_BUTTON(wxID_CANCEL, CustomDialog::OnCancelClick)
END_EVENT_TABLE ()

class Preferences: public wxDialog{
  public:
    Preferences():wxDialog(NULL, wxID_ANY, wxT("Settings"), wxDefaultPosition, wxSize(500, 300))
    {

      wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
      SetSizer(sizer);
      sizer->AddSpacer(30);

      listview = new wxListView(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                            wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VRULES|wxLC_HRULES);

      listview->InsertColumn(0, wxT("Extension"), wxLIST_FORMAT_CENTRE);
      listview->InsertColumn(1, wxT("Command"), wxLIST_FORMAT_LEFT);

      map<wxString,wxString> mapping = mm.getExtensionCommandMapping();
      map<wxString,wxString>::iterator b = mapping.begin();

      for( map<wxString,wxString>::iterator i = mapping.begin(); i != mapping.end(); ++i ){
        listview->InsertItem( distance(b, i), i->first );
        listview->SetItem(distance(b, i), 1, i->second );
      }

      sizer->Add(listview, 1, wxEXPAND|wxLEFT|wxRIGHT, 3);

      sizer = new wxBoxSizer(wxHORIZONTAL);
      sizer->Add(new wxButton(this, wxID_NEW), 0, wxRIGHT, 5);
      sizer->Add(new wxButton(this, wxID_EDIT, wxT("Edit")), 0, wxRIGHT, 5);
      sizer->Add(new wxButton(this, wxID_DELETE), 0, wxRIGHT, 5);

      GetSizer()->Add(sizer, 0, wxALL|wxALIGN_LEFT|wxEXPAND, 3);

      CentreOnParent();

      ShowModal();
      Destroy();
    }

    void UpdateItems(){
      listview->ClearAll();
      listview->InsertColumn(0, wxT("Extension"), wxLIST_FORMAT_CENTRE);
      listview->InsertColumn(1, wxT("Command"), wxLIST_FORMAT_LEFT);

      map<wxString,wxString> mapping = mm.getExtensionCommandMapping();
      map<wxString,wxString>::iterator b = mapping.begin();

      for( map<wxString,wxString>::iterator i = mapping.begin(); i != mapping.end(); ++i ){
        listview->InsertItem( distance(b, i), i->first );
        listview->SetItem(distance(b, i), 1, i->second );
      }

    }

    void OnNewClick(wxCommandEvent& evt){
      long i = listview->GetFocusedItem();
      if( i != -1 ){
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

    void OnDeleteClick(wxCommandEvent& evt){
      long i = listview->GetFocusedItem();

      if( i != -1 ){
        wxString s = listview->GetItemText(i);
        wxMessageDialog *dial = new wxMessageDialog(NULL,
            wxT("Are you sure to remove the command associated for \"") + s + wxT("\"?"), wxT("Question"),
            wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
        if (dial->ShowModal() == wxID_YES)
        {
          cout << s.mb_str(wxConvUTF8) << endl;

          mm.removeExtensionMapping(s);

          UpdateItems();
        }
      }
    }

    void OnEditClick(wxCommandEvent& evt){
      long i = listview->GetFocusedItem();
      if( i != -1 ){
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


  private:
    wxListView *listview;

    MimeManager mm;

    DECLARE_EVENT_TABLE()

};

BEGIN_EVENT_TABLE(Preferences, wxDialog)
  EVT_BUTTON(wxID_NEW, Preferences::OnNewClick)
  EVT_BUTTON(wxID_DELETE, Preferences::OnDeleteClick)
  EVT_BUTTON(wxID_EDIT, Preferences::OnEditClick)
 END_EVENT_TABLE ()


#endif /* MCRL2_GUI_PREFERENCES_H_ */

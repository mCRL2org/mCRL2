// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file savepicdialog.cpp
/// \brief Implements the SavePicDialog class

#include "wx.hpp" // precompiled headers

#include "savepicdialog.h"
#include <algorithm>
#include <wx/spinctrl.h>
#include <wx/statline.h>
#include <wx/statusbr.h>
#include "glcanvas.h"
#include "utils.h"

using namespace std;

// For compatibility with older wxWidgets versions (pre 2.8)
#if (wxMINOR_VERSION < 8)
# define wxFD_SAVE wxSAVE
#endif

BEGIN_EVENT_TABLE(SavePicDialog,wxDialog)
  EVT_SPINCTRL(SavePicDialog::myID_W_SPIN,SavePicDialog::onSpin)
  EVT_SPINCTRL(SavePicDialog::myID_H_SPIN,SavePicDialog::onSpin)
  EVT_CHECKBOX(SavePicDialog::myID_AR_CHECK,SavePicDialog::onARCheck)
  EVT_BUTTON(wxID_OK,SavePicDialog::OnOK)
  EVT_BUTTON(SavePicDialog::myID_F_BUTTON,SavePicDialog::onChangeFile)
  EVT_CHOICE(SavePicDialog::myID_FT_CHOICE,SavePicDialog::onChoice)
END_EVENT_TABLE()

SavePicDialog::SavePicDialog(wxWindow* parent,wxStatusBar* sb,GLCanvas* glc,
    wxFileName filename)
  : wxDialog(parent,-1,wxT("Save Picture"),wxDefaultPosition)
{
  statusbar = sb;
  glcanvas = glc;

  w_spin = new wxSpinCtrl(this,myID_W_SPIN);
  h_spin = new wxSpinCtrl(this,myID_H_SPIN);
  w_spin->SetRange(0,100000);
  h_spin->SetRange(0,100000);
  updateAspectRatio();

  ar_check = new wxCheckBox(this, myID_AR_CHECK,
      wxT("Maintain aspect ratio"));
  ar_check->SetValue(true);

  wxArrayString fts;

  int png_id = 0;

  wxList handlers = wxImage::GetHandlers();
#if (wxUSE_STL == 1)
  for (wxList::const_iterator i = handlers.begin(); i != handlers.end(); ++i)
  {
    const wxString extension = static_cast < wxImageHandler* > (*i)->GetExtension();
#else
  for (wxNode* node = handlers.GetFirst(); node != NULL; node = node->GetNext())
  {
    const wxString extension = static_cast < wxImageHandler* > (node->GetData())->GetExtension();
#endif
    if (extension != wxT("ani") && extension != wxT("cur") && extension != wxT("ico"))
    {
#if (wxUSE_STL == 1)
      const wxString name = static_cast < wxImageHandler* > (*i)->GetName();
      const wxString type = static_cast < wxImageHandler* > (*i)->GetMimeType();
#else
      const wxString name = static_cast < wxImageHandler* > (node->GetData())->GetName();
      const wxString type = static_cast < wxImageHandler* > (node->GetData())->GetMimeType();
#endif
      if (extension == wxT("jpg"))
      {
        fts.Add(name + wxT(" (.jpg .jpeg)"));
      }
      else if (extension == wxT("tif"))
      {
        fts.Add(name + wxT(" (.tif .tiff)"));
      }
      else
      {
        fts.Add(name + wxT(" (.") + extension + wxT(")"));
      }

      if (extension == wxT("png"))
      {
        png_id = f_exts.Count();
      }

      f_exts.Add(extension);
      f_types.push_back(type);
    }
  }

  ft_choice = new wxChoice(this,myID_FT_CHOICE,wxDefaultPosition,wxDefaultSize,
    fts);
  ft_choice->SetSelection(png_id);

  f_name.Assign(filename);
  f_name.SetExt(f_exts.Item(ft_choice->GetSelection()));
  f_text = new wxStaticText(this,-1,wxT(""),wxDefaultPosition,
      wxSize(300,-1),wxST_NO_AUTORESIZE);
  update_file_name();
  f_button = new wxButton(this,myID_F_BUTTON,wxT("Change..."));

  wxFlexGridSizer* controlSizer = new wxFlexGridSizer(5,3,0,0);
  // Row 0
  controlSizer->Add(new wxStaticText(this,-1,wxT("Width:")),0,
    wxEXPAND|wxALL,5);
  controlSizer->Add(w_spin,0,wxEXPAND|wxALL,5);
  controlSizer->AddSpacer(0);
  // Row 1
  controlSizer->Add(new wxStaticText(this,-1,wxT("Height:")),0,
    wxEXPAND|wxALL,5);
  controlSizer->Add(h_spin,0,wxEXPAND|wxALL,5);
  controlSizer->AddSpacer(0);
  // Row 2
  controlSizer->Add(ar_check,0,wxEXPAND|wxALL,5);
  controlSizer->AddSpacer(0);
  controlSizer->AddSpacer(0);
  // Row 3
  controlSizer->Add(new wxStaticText(this,wxID_ANY,wxT("File:")),0,
    wxEXPAND|wxALL,5);
  controlSizer->Add(f_text,0,wxEXPAND|wxALL,5);
  controlSizer->Add(f_button,0,wxEXPAND|wxALL,5);
  // Row 4
  controlSizer->Add(new wxStaticText(this,wxID_ANY,wxT("File type:")),0,
    wxEXPAND|wxALL,5);
  controlSizer->Add(ft_choice,0,wxEXPAND|wxALL,5);
  controlSizer->AddSpacer(0);

  wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
  mainSizer->Add(controlSizer,0,wxEXPAND|wxALL,5);
  mainSizer->Add(new wxStaticLine(this,-1),0,wxEXPAND|wxALL,5);
  mainSizer->Add(CreateButtonSizer(wxOK|wxCANCEL),0,wxEXPAND|wxALL,5);

  mainSizer->Fit(this);
  SetSizer(mainSizer);
  Layout();
}

SavePicDialog::~SavePicDialog()
{
}

void SavePicDialog::updateAspectRatio()
{
  if (w_spin->GetValue() == 0)
  {
    int w,h;
    glcanvas->GetClientSize(&w,&h);
    w_spin->SetValue(w);
    h_spin->SetValue(h);
  }
  else if (ar_check->IsChecked())
  {
    update_h_spin();
  }
}

void SavePicDialog::update_h_spin()
{
  int w,h;
  glcanvas->GetClientSize(&w,&h);
  double ar = double(h)/double(w);
  h_spin->SetValue(Utils::round_to_int(w_spin->GetValue() * ar));
}

void SavePicDialog::update_w_spin()
{
  int w,h;
  glcanvas->GetClientSize(&w,&h);
  double ar = double(w)/double(h);
  w_spin->SetValue(Utils::round_to_int(h_spin->GetValue() * ar));
}

void SavePicDialog::onARCheck(wxCommandEvent &event)
{
  if (event.IsChecked())
  {
    update_h_spin();
  }
}

void SavePicDialog::onSpin(wxSpinEvent& event)
{
  if (ar_check->IsChecked())
  {
    if (event.GetId() == myID_W_SPIN)
    {
      update_h_spin();
    }
    else
    {
      update_w_spin();
    }
  }
}


void SavePicDialog::onChangeFile(wxCommandEvent& event)
{
  wxString new_file = wxFileSelector(wxT("Select a file"),f_name.GetPath(),
    f_name.GetFullName(),wxT(""),wxT("*.*"),wxFD_SAVE,this);
  if (!new_file.empty())
  {
    f_name.Assign(new_file);
    // if extension of new filename indicates a supported file type, set the
    // choice control to that file type (the user apparently wants that file
    // type)
    wxString ext = f_name.GetExt();
    // allow some aliases
    if (ext.Lower() == wxT("jpeg"))
    {
      ext = wxT("jpg");
    }
    if (ext.Lower() == wxT("tiff"))
    {
      ext = wxT("tif");
    }
    int index = f_exts.Index(ext,false);
    if (index != wxNOT_FOUND)
    {
      ft_choice->SetSelection(index);
    }
    else
    {
      // extension is not supported, so make the user-defined extension part of
      // the base filename and append the extension of the currently selected
      // file type
      f_name.SetName(f_name.GetFullName());
      f_name.SetExt(f_exts.Item(ft_choice->GetSelection()));
    }
    update_file_name();
  }
}

void SavePicDialog::onChoice(wxCommandEvent& event)
{
  wxString newext = f_exts.Item(event.GetSelection());
  wxString oldext = f_name.GetExt().Lower();
  if (newext != oldext &&
      !(newext == wxT("jpg") && oldext == wxT("jpeg")) &&
      !(newext == wxT("tif") && oldext == wxT("tiff")))
  {
    f_name.SetExt(newext);
    update_file_name();
  }
}

void SavePicDialog::OnOK(wxCommandEvent& /*event*/)
{
  Hide();
  GetParent()->SetCursor(wxCursor(wxCURSOR_WAIT));
  GetParent()->Disable();
  GetParent()->Refresh();
  GetParent()->Update();
  glcanvas->display();

  int w = w_spin->GetValue();
  int h = h_spin->GetValue();

  statusbar->SetStatusText(wxT("Collecting picture data..."));
  statusbar->Update();
  unsigned char* data = glcanvas->getPictureData(w,h);
  wxImage img(w,h,data);

  // order of image pixels is row major from bottom to top, but wxWidgets
  // assumes it to be from top to bottom, so we mirror the image vertically
  statusbar->SetStatusText(wxT("Mirroring image..."));
  statusbar->Update();
  img = img.Mirror(false);

  statusbar->SetStatusText(wxT("Saving image to file..."));
  statusbar->Update();
  if (!img.SaveFile(f_name.GetFullPath(),f_types[ft_choice->GetSelection()]))
  {
    statusbar->SetStatusText(wxT("Save picture failed"));
    statusbar->Update();
  }
  else
  {
    statusbar->SetStatusText(wxT("Done"));
    statusbar->Update();
    wxMessageDialog msgDialog(GetParent(),wxT("The picture was saved to file:\n\n") +
        f_name.GetFullPath(),wxT("Picture saved"),wxOK|wxICON_INFORMATION);
    msgDialog.ShowModal();
  }
  statusbar->SetStatusText(wxT(""));
  statusbar->Update();
  GetParent()->Enable();
  GetParent()->SetCursor(wxNullCursor);
}

void SavePicDialog::update_file_name()
{
  wxString text = f_name.GetFullPath();
  if (text.Len() > MAX_LABEL_LENGTH)
  {
    text = text.Left((MAX_LABEL_LENGTH - 3) / 2)
      + wxT("...")
      + text.Right((MAX_LABEL_LENGTH - 3) / 2 + (MAX_LABEL_LENGTH - 3) % 2);
  }
  f_text->SetLabel(text);
}

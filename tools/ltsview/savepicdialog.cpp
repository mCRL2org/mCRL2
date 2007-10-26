// Author(s): Bas Ploeger and Carst Tankink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file savepicdialog.cpp
/// \brief Add your file description here.

#include "savepicdialog.h"
#include <wx/statline.h>

// For compatibility with older wxWidgets versions (pre 2.8)
#if (wxMINOR_VERSION < 8)
# define wxFD_SAVE wxSAVE
#endif

BEGIN_EVENT_TABLE(SavePicDialog,wxDialog)
  EVT_COMMAND_SCROLL_THUMBTRACK(myID_R_SLIDER,SavePicDialog::onSlider)
  EVT_COMMAND_SCROLL_CHANGED(myID_R_SLIDER,SavePicDialog::onSlider)
  EVT_BUTTON(myID_F_BUTTON,SavePicDialog::onChangeFile)
  EVT_CHOICE(myID_FT_CHOICE,SavePicDialog::onChoice)
  EVT_BUTTON(wxID_OK,SavePicDialog::OnOK)
END_EVENT_TABLE()

SavePicDialog::SavePicDialog(wxWindow* parent,wxStatusBar* sb,GLCanvas* glc,
    wxFileName filename)
: wxDialog(parent,-1,wxT("Save Picture"),wxDefaultPosition) {
  statusbar = sb;
  glcanvas = glc;
  int w,h,w_max,h_max;
  glcanvas->GetClientSize(&w,&h);
  glcanvas->getMaxViewportDims(&w_max,&h_max);
  ar = float(w)/float(h);
  r_slider = new wxSlider(this,myID_R_SLIDER,w,1,(w_max<=h_max)?w_max:
    int(ar*h_max));
  r_text = new wxStaticText(this,-1,wxString::Format(wxT("%dx%d"),w,h));

  wxArrayString fts;

  int png_id = 0;

  wxList handlers = wxImage::GetHandlers();
#if (wxUSE_STL == 1)
  for (wxList::const_iterator i = handlers.begin(); i != handlers.end(); ++i) {
    const wxString extension = static_cast < wxImageHandler* > (*i)->GetExtension();
#else
  for (wxNode* node = handlers.GetFirst(); node != NULL; node = node->GetNext()) {
    const wxString extension = static_cast < wxImageHandler* > (node->GetData())->GetExtension();
#endif
    if (extension != wxT("ani") && extension != wxT("cur") && extension != wxT("ico")) {
#if (wxUSE_STL == 1)
      const wxString name = static_cast < wxImageHandler* > (*i)->GetName();
      const long int type = static_cast < wxImageHandler* > (*i)->GetType();
#else
      const wxString name = static_cast < wxImageHandler* > (node->GetData())->GetName();
      const long int type = static_cast < wxImageHandler* > (node->GetData())->GetType();
#endif

      if (extension != wxT("jpg")) {
        fts.Add(name + wxT(" (.jpg .jpeg)"));
      }
      else if (extension == wxT("tif")) {
        fts.Add(name + wxT(" (.tif .tiff)"));
      }
      else {
        fts.Add(name + wxT(" (.") + extension + wxT(")"));
      }
      if (extension == wxT("png")) {
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
  /*if (dir == wxEmptyString) {
    f_name.AssignHomeDir();
  }
  else {
    f_name.AssignDir(dir);
  }
  if (filename == wxEmptyString) {
    f_name.SetName(wxT("picture"));
  }
  else {
    f_name.SetFullName(filename);
  }*/
  f_name.SetExt(f_exts.Item(ft_choice->GetSelection()));
  f_text = new wxStaticText(this,-1,wxT(""),wxDefaultPosition,wxSize(300,-1));
  f_text->SetLabel(f_name.GetFullPath());
  
  f_button = new wxButton(this,myID_F_BUTTON,wxT("Change..."));

  wxFlexGridSizer* controlSizer = new wxFlexGridSizer(3,3,0,0);
  controlSizer->Add(new wxStaticText(this,-1,wxT("Image size:")),0,
    wxEXPAND|wxALL,5);
  controlSizer->Add(r_slider,0,wxEXPAND|wxALL,5);
  controlSizer->Add(r_text,0,wxEXPAND|wxALL,5);
  controlSizer->Add(new wxStaticText(this,wxID_ANY,wxT("File name:")),0,
    wxEXPAND|wxALL,5);
  controlSizer->Add(f_text,0,wxEXPAND|wxALL,5);
  controlSizer->Add(f_button,0,wxEXPAND|wxALL,5);
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

SavePicDialog::~SavePicDialog() {
}

void SavePicDialog::onSlider(wxScrollEvent& /*event*/) {
  r_text->SetLabel(wxString::Format(wxT("%dx%d"),r_slider->GetValue(),
    int(r_slider->GetValue()/ar)));
}

void SavePicDialog::onChangeFile(wxCommandEvent& /*event*/) {
  wxString new_file = wxFileSelector(wxT("Select a file"),f_name.GetPath(),
    f_name.GetFullName(),wxT(""),wxT("*.*"),wxFD_SAVE,this);
  if (!new_file.empty()) {
    f_name.Assign(new_file);
    // if extension of new filename indicates a supported file type, set the
    // choice control to that file type (the user apparently wants that file
    // type)
    wxString ext = f_name.GetExt();
    // allow some aliases
    if (ext.Lower() == wxT("jpeg")) {
      ext = wxT("jpg");
    }
    if (ext.Lower() == wxT("tiff")) {
      ext = wxT("tif");
    }
    int index = f_exts.Index(ext,false);
    if (index != wxNOT_FOUND) {
      ft_choice->SetSelection(index);
    }
    else {
      // extension is not supported, so make the user-defined extension part of
      // the base filename and append the extension of the currently selected
      // file type
      f_name.SetName(f_name.GetFullName());
      f_name.SetExt(f_exts.Item(ft_choice->GetSelection()));
    }
    f_text->SetLabel(f_name.GetFullPath());
  }
}

void SavePicDialog::onChoice(wxCommandEvent& event) {
  wxString newext = f_exts.Item(event.GetSelection());
  wxString oldext = f_name.GetExt().Lower();
  if (newext != oldext &&
      !(newext == wxT("jpg") && oldext == wxT("jpeg")) &&
      !(newext == wxT("tif") && oldext == wxT("tiff"))) {
    f_name.SetExt(newext);
    f_text->SetLabel(f_name.GetFullPath());
  }
}

int SavePicDialog::getImageWidth() {
  return r_slider->GetValue();
}

int SavePicDialog::getImageHeight() {
  return int(r_slider->GetValue()/ar);
}

wxString SavePicDialog::getFileName() {
  return f_name.GetFullPath();
}

long SavePicDialog::getFileType() {
  return f_types[ft_choice->GetSelection()];
}

void SavePicDialog::OnOK(wxCommandEvent& /*event*/) {
  Hide();
  GetParent()->SetCursor(wxCursor(wxCURSOR_WAIT));
  GetParent()->Disable();
  GetParent()->Refresh();
  GetParent()->Update();
  glcanvas->display();

  int w = r_slider->GetValue();
  int h = int(w/ar);
  
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
  img.SaveFile(f_name.GetFullPath(),f_types[ft_choice->GetSelection()]);
  
  statusbar->SetStatusText(wxT("Done"));
  statusbar->Update();
  GetParent()->Enable();
  GetParent()->SetCursor(wxNullCursor);
  wxMessageDialog msgDialog(GetParent(),wxT("The picture was saved to file:\n\n") + 
      f_name.GetFullPath(),wxT("Picture saved"),wxOK|wxICON_INFORMATION);
  msgDialog.ShowModal();
  
  statusbar->SetStatusText(wxT(""));
  statusbar->Update();
}

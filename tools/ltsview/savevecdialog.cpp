// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file savevecdialog.cpp
/// \brief Implements the SaveVecDialog class

#include "wx.hpp" // precompiled headers

#include "savevecdialog.h"
#include <vector>
#include <wx/statline.h>
#include <wx/statusbr.h>
#include <gl2ps.h>
#include "glcanvas.h"

using namespace std;

// For compatibility with older wxWidgets versions (pre 2.8)
#if (wxMINOR_VERSION < 8)
# define wxFD_SAVE wxSAVE
#endif

BEGIN_EVENT_TABLE(SaveVecDialog,wxDialog)
  EVT_BUTTON(wxID_OK,SaveVecDialog::OnOK)
  EVT_BUTTON(SaveVecDialog::myID_F_BUTTON,SaveVecDialog::onChangeFile)
  EVT_CHOICE(SaveVecDialog::myID_FT_CHOICE,SaveVecDialog::onChoice)
END_EVENT_TABLE()

SaveVecDialog::SaveVecDialog(wxWindow* parent,wxStatusBar* sb,GLCanvas* glc,
                             wxFileName filename)
  : wxDialog(parent,-1,wxT("Save Picture"),wxDefaultPosition)
{
  statusbar = sb;
  glcanvas = glc;

  f_formats.push_back(GL2PS_PS);
  f_formats.push_back(GL2PS_EPS);
  f_formats.push_back(GL2PS_PDF);
  f_formats.push_back(GL2PS_SVG);

  f_desc.push_back(wxT("PostScript"));
  f_desc.push_back(wxT("Encapsulated PostScript"));
  f_desc.push_back(wxT("Portable Document Format"));
  f_desc.push_back(wxT("Scalable Vector Graphics"));

  f_exts.push_back(wxT("ps"));
  f_exts.push_back(wxT("eps"));
  f_exts.push_back(wxT("pdf"));
  f_exts.push_back(wxT("svg"));

  wxArrayString fts;
  unsigned int default_id = 0;
  for (unsigned int i = 0; i < f_desc.size(); ++i)
  {
    fts.Add(f_desc[i] + wxT(" (.") + f_exts[i] + wxT(")"));
    if (f_exts[i] == wxT("svg"))
    {
      default_id = i;
    }
  }

  ft_choice = new wxChoice(this,myID_FT_CHOICE,wxDefaultPosition,
                           wxDefaultSize,fts);
  ft_choice->SetSelection(default_id);

  f_name.Assign(filename);
  f_name.SetExt(f_exts[ft_choice->GetSelection()]);
  f_text = new wxStaticText(this,-1,wxT(""),wxDefaultPosition,wxSize(300,-1));
  update_file_name();
  f_button = new wxButton(this,myID_F_BUTTON,wxT("Change..."));

  bg_check = new wxCheckBox(this, wxID_ANY, wxT("Include background colour"));
  lscape_check = new wxCheckBox(this, wxID_ANY, wxT("Landscape paper orientation"));
  ps3_check = new wxCheckBox(this, wxID_ANY, wxT("PostScript level 3"));
  blend_check = new wxCheckBox(this, wxID_ANY, wxT("Enable blending (enable for transparency)"));
  cull_check = new wxCheckBox(this, wxID_ANY, wxT("Remove hidden polygons (disable for transparency)"));
  bbox_check = new wxCheckBox(this, wxID_ANY, wxT("Tight bounding box"));

  bg_check->SetValue(false);
  lscape_check->SetValue(false);
  ps3_check->SetValue(true);
  blend_check->SetValue(true);
  cull_check->SetValue(true);
  bbox_check->SetValue(true);

  wxFlexGridSizer* controlSizer = new wxFlexGridSizer(2,3,0,0);
  // Row 0
  controlSizer->Add(new wxStaticText(this,wxID_ANY,wxT("File:")),0,
                    wxEXPAND|wxALL,5);
  controlSizer->Add(f_text,0,wxEXPAND|wxALL,5);
  controlSizer->Add(f_button,0,wxEXPAND|wxALL,5);
  // Row 1
  controlSizer->Add(new wxStaticText(this,wxID_ANY,wxT("File type:")),0,
                    wxEXPAND|wxALL,5);
  controlSizer->Add(ft_choice,0,wxEXPAND|wxALL,5);
  controlSizer->AddSpacer(0);

  wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
  mainSizer->Add(controlSizer,0,wxEXPAND|wxALL,5);
  mainSizer->Add(bg_check,0,wxEXPAND|wxALL,5);
  mainSizer->Add(lscape_check,0,wxEXPAND|wxALL,5);
  mainSizer->Add(ps3_check,0,wxEXPAND|wxALL,5);
  mainSizer->Add(blend_check,0,wxEXPAND|wxALL,5);
  mainSizer->Add(cull_check,0,wxEXPAND|wxALL,5);
  mainSizer->Add(bbox_check,0,wxEXPAND|wxALL,5);
  mainSizer->Add(new wxStaticLine(this,-1),0,wxEXPAND|wxALL,5);
  mainSizer->Add(CreateButtonSizer(wxOK|wxCANCEL),0,wxEXPAND|wxALL,5);

  mainSizer->Fit(this);
  SetSizer(mainSizer);
  Layout();
}

SaveVecDialog::~SaveVecDialog()
{
}

void SaveVecDialog::onChangeFile(wxCommandEvent& /*event*/)
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
    unsigned int i = 0;
    while (i < f_exts.size() && f_exts[i] != ext)
    {
      ++i;
    }
    if (i < f_exts.size())
    {
      ft_choice->SetSelection(i);
    }
    else
    {
      // extension is not supported, so make the user-defined extension part of
      // the base filename and append the extension of the currently selected
      // file type
      f_name.SetName(f_name.GetFullName());
      f_name.SetExt(f_exts[ft_choice->GetSelection()]);
    }
    update_file_name();
  }
}

void SaveVecDialog::onChoice(wxCommandEvent& event)
{
  wxString newext = f_exts[event.GetSelection()];
  wxString oldext = f_name.GetExt().Lower();
  if (newext != oldext)
  {
    f_name.SetExt(newext);
    update_file_name();
  }
}

void SaveVecDialog::OnOK(wxCommandEvent& /*event*/)
{
  wxMessageDialog msgQuestion(GetParent(),
                            wxT("Saving an vector image may create very large files.\nAre you sure you want to continue?"),
                            wxT("Question"), wxYES_NO | wxYES_DEFAULT | wxICON_QUESTION);
  if (msgQuestion.ShowModal() == wxID_NO )
  {
    return;
  }

  FILE* fp = fopen(f_name.GetFullPath().mb_str(wxConvUTF8), "wb");
  if (fp == NULL)
  {
    wxMessageDialog msgDialog(GetParent(),
                              wxT("Could not open file for writing:\n\n")
                              + f_name.GetFullPath(), wxT("Error writing file"),
                              wxOK | wxICON_ERROR);
    msgDialog.ShowModal();
    return;
  }
  Hide();
  GetParent()->SetCursor(wxCursor(wxCURSOR_WAIT));
  GetParent()->Disable();
  GetParent()->Refresh();
  GetParent()->Update();
  glcanvas->display();

  statusbar->SetStatusText(wxT("Saving image to file..."));
  statusbar->Update();

  GLint buffsize = 0;
  GLint begstate;
  GLint endstate = GL2PS_OVERFLOW;
  GLint options = GL2PS_SILENT | GL2PS_USE_CURRENT_VIEWPORT
                  | GL2PS_BEST_ROOT;

  if (bg_check->GetValue())
  {
    options |= GL2PS_DRAW_BACKGROUND;
  }
  if (lscape_check->GetValue())
  {
    options |= GL2PS_LANDSCAPE;
  }
  if (!ps3_check->GetValue())
  {
    options |= GL2PS_NO_PS3_SHADING;
  }
  if (!blend_check->GetValue())
  {
    options |= GL2PS_NO_BLENDING;
  }
  if (cull_check->GetValue())
  {
    options |= GL2PS_OCCLUSION_CULL;
  }
  if (bbox_check->GetValue())
  {
    options |= GL2PS_TIGHT_BOUNDING_BOX;
  }

  while (endstate == GL2PS_OVERFLOW)
  {
    buffsize += 1024*1024;
    begstate =
      gl2psBeginPage(f_name.GetFullName().mb_str(wxConvUTF8), "LTSView", NULL,
                     f_formats[ft_choice->GetSelection()], GL2PS_BSP_SORT, options,
                     GL_RGBA, 0, NULL, 0, 0, 0, buffsize, fp, "");
    if (begstate == GL2PS_ERROR)
    {
      break;
    }
    glcanvas->display();
    endstate = gl2psEndPage();
  }

  fclose(fp);

  if (begstate == GL2PS_ERROR || endstate == GL2PS_ERROR)
  {
    statusbar->SetStatusText(wxT("Save picture failed"));
    statusbar->Update();
    wxMessageDialog msgDialog(GetParent(),
                              wxT("Saving picture failed with an unknown error."),
                              wxT("Save picture failed"), wxOK | wxICON_ERROR);
    msgDialog.ShowModal();
  }
  else
  {
    statusbar->SetStatusText(wxT("Done"));
    statusbar->Update();
    wxMessageDialog msgDialog(GetParent(),
                              wxT("The picture was saved to file:\n\n") +
                              f_name.GetFullPath(), wxT("Picture saved"),
                              wxOK | wxICON_INFORMATION);
    msgDialog.ShowModal();
  }
  statusbar->SetStatusText(wxT(""));
  statusbar->Update();
  GetParent()->Enable();
  GetParent()->SetCursor(wxNullCursor);
  this->EndModal(wxOK);
}

void SaveVecDialog::update_file_name()
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

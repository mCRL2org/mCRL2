// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file savevecdialog.h
/// \brief Header file for SaveVecDialog class

#ifndef SAVEVECDIALOG_H
#define SAVEVECDIALOG_H

#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/statusbr.h>
#include <wx/statusbr.h>
#include <vector>
#include "glcanvas.h"

class SaveVecDialog : public wxDialog {
  public:
    static const int myID_F_BUTTON = wxID_HIGHEST + 1;
    static const int myID_FT_CHOICE = myID_F_BUTTON + 1;

    SaveVecDialog(wxWindow* parent,wxStatusBar* sb,GLCanvas* glc,
        wxFileName filename);
    ~SaveVecDialog();
    void onChangeFile(wxCommandEvent& event);
    void onChoice(wxCommandEvent& event);
    void OnOK(wxCommandEvent& event);
  private:
    GLCanvas*     glcanvas;
    wxStatusBar*  statusbar;
    std::vector<wxString> f_exts;
    std::vector<wxString> f_desc;
    std::vector<GLint> f_formats;
    wxChoice*     ft_choice;
    wxStaticText* f_text;
    wxStaticText* d_text;
    wxButton*     f_button;
    wxFileName    f_name;

    wxCheckBox*   bg_check;
    wxCheckBox*   lscape_check;
    wxCheckBox*   ps3_check;
    wxCheckBox*   blend_check;
    wxCheckBox*   cull_check;
    wxCheckBox*   bbox_check;

    DECLARE_EVENT_TABLE()
};
#endif

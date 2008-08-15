// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file savepicdialog.h
/// \brief Add your file description here.

#ifndef SAVEPICDIALOG_H
#define SAVEPICDIALOG_H

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/filename.h>
#include <wx/statusbr.h>
#include <vector>
#include "glcanvas.h"

const int myID_F_BUTTON = wxID_HIGHEST + 1;
const int myID_W_SPIN = myID_F_BUTTON + 1;
const int myID_H_SPIN = myID_W_SPIN + 1;
const int myID_AR_CHECK = myID_H_SPIN + 1;
const int myID_FT_CHOICE = myID_AR_CHECK + 1;

class SavePicDialog : public wxDialog {
  public:
    SavePicDialog(wxWindow* parent,wxStatusBar* sb,GLCanvas* glc,
        wxFileName filename);
    ~SavePicDialog();
    int  getImageWidth();
    int  getImageHeight();
    wxString getFileName();
    void onARCheck(wxCommandEvent& event);
    void onSpin(wxSpinEvent& event);
    void onChangeFile(wxCommandEvent& event);
    void onChoice(wxCommandEvent& event);
    void OnOK(wxCommandEvent& event);
    void updateAspectRatio();
  private:
    GLCanvas*     glcanvas;
    wxStatusBar*  statusbar;
    wxSpinCtrl*   w_spin;
    wxSpinCtrl*   h_spin;
    wxCheckBox*   ar_check;
    wxStaticText* r_text;
    wxChoice*     ft_choice;
    wxStaticText* f_text;
    wxStaticText* d_text;
    wxButton*     f_button;
    wxFileName    f_name;
    wxArrayString f_exts;
    std::vector< wxString >  f_types;

    void update_h_spin();
    void update_w_spin();

    DECLARE_EVENT_TABLE()
};
#endif

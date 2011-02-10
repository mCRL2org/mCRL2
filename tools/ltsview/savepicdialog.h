// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file savepicdialog.h
/// \brief Header file for SavePicDialog class

#ifndef SAVEPICDIALOG_H
#define SAVEPICDIALOG_H

#include <vector>
#include <wx/wx.h>
#include <wx/filename.h>

class GLCanvas;
class wxSpinCtrl;
class wxSpinEvent;
class wxStatusBar;

class SavePicDialog : public wxDialog
{
  public:
    static const int myID_F_BUTTON = wxID_HIGHEST + 1;
    static const int myID_W_SPIN = myID_F_BUTTON + 1;
    static const int myID_H_SPIN = myID_W_SPIN + 1;
    static const int myID_AR_CHECK = myID_H_SPIN + 1;
    static const int myID_FT_CHOICE = myID_AR_CHECK + 1;

    SavePicDialog(wxWindow* parent,wxStatusBar* sb,GLCanvas* glc,
                  wxFileName filename);
    ~SavePicDialog();
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
    wxButton*     f_button;
    wxFileName    f_name;
    wxArrayString f_exts;
    std::vector< wxString >  f_types;

    void update_h_spin();
    void update_w_spin();
    void update_file_name();

    static const unsigned int MAX_LABEL_LENGTH = 50;

    DECLARE_EVENT_TABLE()
};
#endif

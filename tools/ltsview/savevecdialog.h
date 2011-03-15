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

#include <vector>
#include <wx/wx.h>
#include <wx/filename.h>

extern "C" {
#ifdef __APPLE__
# include <OpenGL/gl.h>
# include <OpenGL/glu.h>
#else
# if defined(_WIN32_) || defined(_MSC_VER)
#  include <windows.h>
#  undef __in_range // For STLport
# endif
# include <GL/gl.h>
# include <GL/glu.h>
#endif
}


class GLCanvas;
class wxStatusBar;

class SaveVecDialog : public wxDialog
{
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
    wxButton*     f_button;
    wxFileName    f_name;

    wxCheckBox*   bg_check;
    wxCheckBox*   lscape_check;
    wxCheckBox*   ps3_check;
    wxCheckBox*   blend_check;
    wxCheckBox*   cull_check;
    wxCheckBox*   bbox_check;
    void update_file_name();

    static const unsigned int MAX_LABEL_LENGTH = 50;

    DECLARE_EVENT_TABLE()
};
#endif

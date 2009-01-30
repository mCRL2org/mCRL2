// Author(s): Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file infodialog.h
/// \brief LTS information dialog.

#ifndef INFODIALOG_H
#define INFODIALOG_H

#include <string>
#include <wx/wx.h>
#include <wx/listctrl.h>

class InfoDialog: public wxDialog {
  public:
    InfoDialog(wxWindow* parent);
    void setLTSInfo(int is, // Initial state
                    int ns, // Number of states
                    int nt, // Number of transitions
                    int nl // Number of labels/actions
                    );

  private:
    wxListCtrl* lts_info;
};

#endif // INFODIALOG_H

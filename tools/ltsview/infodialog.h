// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file infodialog.h
/// \brief Header file for information dialog class

#ifndef INFODIALOG_H
#define INFODIALOG_H
#include <string>
#include <vector>
#include <wx/wx.h>

class wxListCtrl;

class InfoDialog: public wxDialog
{
  public:
    InfoDialog(wxWindow* parent);
    void setLTSInfo(int ns,int nt,int nc,int nr);
    void setNumMarkedStates(int n);
    void setNumMarkedTransitions(int n);
    void setParameterName(int i,std::string p);
    void setParameterValue(int i,std::string v);
    void setParameterValues(int i, std::vector<std::string> vs);
    void setClusterStateNr(int i);
    void resetParameterNames();
    void resetParameterValues();
  private:
    wxListCtrl* lts_info;
    wxListCtrl* cluster_info;

    wxListCtrl* state_info;
    void setStateInfoColWidth(int col);
};

#endif

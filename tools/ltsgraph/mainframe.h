// Author(s): Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mainframe.h
/// \brief Main window declaration

#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <wx/wx.h>
#include <wx/frame.h>

#ifndef LTSGRAPH_H
  #include "ltsgraph.h"
#else
  class LTSGraph; // Forward declaration
#endif

#ifndef GLCANVAS_H
  #include "glcanvas.h"
#else
  class GLCanvas;
#endif

#ifndef ALGO_DIALOG_H
  #include "algodialog.h"
#else
  class AlgoDialog;
#endif

#ifndef SETTINGS_DIALOG_H
  #include "settingsdialog.h"
#else
  class SettingsDialog;
#endif

#include "infodialog.h"

class MainFrame : public wxFrame
{
  public:
    MainFrame(LTSGraph *owner);
    GLCanvas* getGLCanvas(); 
    
    void setLTSInfo(int is, int ns, int nt, int nl);

  private:
    LTSGraph *app;
    GLCanvas *glCanvas;
    AlgoDialog *algoDlg;
    SettingsDialog *settingsDlg;
    InfoDialog *infoDlg;

    void setupMenuBar();
    void setupMainArea();


    void onOpen(wxCommandEvent& event);
    void onQuit(wxCommandEvent& event);
    void onAlgo(wxCommandEvent& event);
    void onSettings(wxCommandEvent& event);
    void onInfo(wxCommandEvent& event);
    void onExport(wxCommandEvent& event);
    void onImport(wxCommandEvent& event);
    void onSelect(wxCommandEvent& event);
    void onColour(wxCommandEvent& event);

  DECLARE_EVENT_TABLE()
};

#endif // MAINFRAME_H

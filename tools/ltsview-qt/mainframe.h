// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mainframe.h
/// \brief Header file for the main LTSView window

#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <QObject>
#include <QString>
#include <QStringList>

#include "ltsmanager.h"
#include "visualizer.h"

#include <string>
#include <vector>
#include <wx/wx.h>
#include <wx/filename.h>

class wxCheckListBox;
class wxProgressDialog;
class InfoDialog;
class SimDialog;
class MarkDialog;
class SettingsDialog;
class GLCanvas;
class Settings;
class MarkManager;

class MainFrame : public QObject, public wxFrame
{
  Q_OBJECT

  public:
    MainFrame();
    GLCanvas* getGLCanvas() const;
    void loadTitle();

    void onActivateTool(wxCommandEvent& event);
    void onDisplay(wxCommandEvent& event);
    void onExit(wxCommandEvent& event);
    void onClose(wxCloseEvent& event);
    void onOpen(wxCommandEvent& event);
    void onOpenTrace(wxCommandEvent& event);
    void onSavePic(wxCommandEvent& event);
    void onSaveVec(wxCommandEvent& event);
    void onSaveText(wxCommandEvent& event);
    void onVisStyle(wxCommandEvent& event);
    void onFSMStyle(wxCommandEvent& event);
    void onResetView(wxCommandEvent& event);
    void onSettings(wxCommandEvent& event);
    void onInfo(wxCommandEvent& event);
    void onMark(wxCommandEvent& event);
    void onSim(wxCommandEvent& event);

    void onZoomInBelow(wxCommandEvent& event);
    void onZoomInAbove(wxCommandEvent& event);
    void onZoomOut(wxCommandEvent& event);
    void onGenerateBackTraceButton(wxCommandEvent& event);
    void onStartForceDirected(wxCommandEvent& event);
    void onStopForceDirected(wxCommandEvent& event);
    void onResetStatePositions(wxCommandEvent& event);

    void setFileInfo(wxFileName fn);
    void showMessage(std::string title,std::string text);

    void createProgressDialog(const std::string& title,const std::string& text);
    void updateProgressDialog(int val,std::string msg);

  public slots:
    void startRendering();
    void stopRendering();
    void createProgressDialog(QString title) { createProgressDialog(title.toStdString(), title.toStdString()); }
  protected slots:
    void loadingLts() { ensureProgressDialog(); updateProgressDialog(0, "Loading file"); }
    void rankingStates() { ensureProgressDialog(); updateProgressDialog(17, "Ranking states"); }
    void clusteringStates() { ensureProgressDialog(); updateProgressDialog(33, "Clustering states"); }
    void computingClusterInfo() { ensureProgressDialog(); updateProgressDialog(50, "Setting cluster info"); }
    void positioningClusters() { ensureProgressDialog(); updateProgressDialog(67, "Positioning clusters"); }
    void positioningStates() { ensureProgressDialog(); updateProgressDialog(83, "Positioning states"); }
    void hideProgressDialog() { ensureProgressDialog(); updateProgressDialog(100, ""); }
    void ensureProgressDialog() { if (progDialog == NULL) createProgressDialog("Structuring LTS"); }

  private slots:
    void setStatusBar(QString message) { GetStatusBar()->SetStatusText(wxString(message.toStdString().c_str(), wxConvUTF8)); GetStatusBar()->Update(); }

//    void  onIdle(wxIdleEvent &event);/* needed for computing the frame rate */
  private:
//    double previousTime; /* needed for computing the frame rate (FPS) */
//    int    frameCount; /* needed for computing the frame rate (FPS) */
    wxFileName        filename;
    GLCanvas*         glCanvas;
    wxChoice*         markAnyAllChoice;
    wxRadioButton*    markDeadlocksRadio;
    wxCheckListBox*   markStatesListBox;
    wxRadioButton*    markStatesRadio;
    wxCheckListBox*   markTransitionsListBox;
    wxRadioButton*    markTransitionsRadio;
    wxRadioButton*    nomarksRadio;
    wxProgressDialog* progDialog;
    SettingsDialog*   settingsDialog;
    InfoDialog*       infoDialog;
    MarkDialog*       markDialog;
    SimDialog*        simDialog;
    Settings*         settings;
    LtsManager*       ltsManager;
    MarkManager*      markManager;
    Visualizer*       visualizer;
    wxFlexGridSizer*  selSizer;
    wxMenu*           toolMenu;

    void setupMainArea();
    void setupMenuBar();


    DECLARE_EVENT_TABLE()
};
#endif

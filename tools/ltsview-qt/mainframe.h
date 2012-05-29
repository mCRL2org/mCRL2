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
class Mediator;
class Settings;
class Simulation;
class MarkManager;

class MainFrame : public QObject, public wxFrame
{
  Q_OBJECT

  public:
    MainFrame(Mediator* owner, Settings* ss, MarkManager *manager);
    void createProgressDialog(const std::string& title,const std::string& text);
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

    void setSim(Simulation* sim);
    void setFileInfo(wxFileName fn);
    void setMarkedStatesInfo(int number);
    void setMarkedTransitionsInfo(int number);
    void setNumberInfo(int ns,int nt,int nc,int nr);
    void showMessage(std::string title,std::string text);
    void startRendering();
    void stopRendering();
    void updateProgressDialog(int val,std::string msg);

    void setParameterNames(QStringList parameters);
    void setParameterValue(int parameter, QString value);
    void setParameterValues(int parameter, QStringList values);
    void setStatesInCluster(int n);
    void resetParameterValues();

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
    Mediator*         mediator;
    wxRadioButton*    nomarksRadio;
    wxProgressDialog* progDialog;
    SettingsDialog*   settingsDialog;
    InfoDialog*       infoDialog;
    MarkDialog*       markDialog;
    SimDialog*        simDialog;
    Settings*         settings;
    wxFlexGridSizer*  selSizer;
    wxMenu*           toolMenu;

    void setupMainArea();
    void setupMenuBar();


    DECLARE_EVENT_TABLE()
};
#endif

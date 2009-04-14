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
class SavePicDialog;
class SaveVecDialog;
class GLCanvas;
class Mediator;
class Settings;
class Simulation;

class MainFrame : public wxFrame {
  public:
    MainFrame(Mediator* owner,Settings* ss);
    void addMarkRule(wxString str,int mr);
    void createProgressDialog(const std::string title,const std::string text);
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
    void onRankStyle(wxCommandEvent& event);
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
    void replaceMarkRule(wxString str,int index);
    void resetMarkRules();
    void setActionLabels(std::vector< std::string > &labels);
    void setFileInfo(wxFileName fn);
    void setMarkedStatesInfo(int number);
    void setMarkedTransitionsInfo(int number);
    void setNumberInfo(int ns,int nt,int nc,int nr);
    void showMessage(std::string title,std::string text);
    void startRendering();
    void stopRendering();
    void updateProgressDialog(int val,std::string msg);

    void addParameter(int i,std::string par);
    void setParameterValue(int i,std::string value);
    void setParameterValues(int i, std::vector<std::string> values);
    void setClusterStateNr(int n);
    void resetParameters();
    void resetParameterValues();

    void reportError(std::string const& error);

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
    SavePicDialog*    savePicDialog;
    SaveVecDialog*    saveVecDialog;
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

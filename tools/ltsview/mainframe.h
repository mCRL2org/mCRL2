// Author(s): Bas Ploeger and Carst Tankink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mainframe.h
/// \brief Add your file description here.

#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <string>
#include <wx/wx.h>
#include <wx/checklst.h>
#include <wx/filename.h>
#include <wx/listctrl.h>
#include <wx/progdlg.h>

#include "infodialog.h"
#include "simdialog.h"
#include "markdialog.h"
#include "settingsdialog.h"
#include "savepicdialog.h"
#include "glcanvas.h"
#include "mediator.h"
#include "settings.h"


class MainFrame : public wxFrame {
  public:
    MainFrame(Mediator* owner,Settings* ss);
    void addMarkRule(wxString str);
    void createProgressDialog(const std::string title,const std::string text);
    GLCanvas* getGLCanvas() const;
    void loadTitle();

    void onAbout(wxCommandEvent &event);
    void onActivateTool(wxCommandEvent& event);
    void onAddMarkRuleButton(wxCommandEvent& event);
    void onDisplay(wxCommandEvent& event);
    void onExit(wxCommandEvent& event);
    void onClose(wxCloseEvent& event);
    void onOpen(wxCommandEvent& event);
    void onOpenTrace(wxCommandEvent& event);
    void onSavePic(wxCommandEvent& event);
    void onRankStyle(wxCommandEvent& event);
    void onVisStyle(wxCommandEvent& event);
    void onFSMStyle(wxCommandEvent& event);
    void onRemoveMarkRuleButton(wxCommandEvent& event);
    void onResetView(wxCommandEvent& event);
    void onSettings(wxCommandEvent& event);
    void onInfo(wxCommandEvent& event);
    void onMark(wxCommandEvent& event);
    void onSim(wxCommandEvent& event);
    void onMarkAnyAll(wxCommandEvent& event);
    void onMarkRadio(wxCommandEvent& event);
    void onMarkRuleActivate(wxCommandEvent& event);
    void onMarkRuleEdit(wxCommandEvent& event);
    void onMarkTransition(wxCommandEvent& event);
    
    void onZoomInBelow(wxCommandEvent& event);
    void onZoomInAbove(wxCommandEvent& event);
    void onZoomOut(wxCommandEvent& event);
    void onGenerateBackTraceButton(wxCommandEvent& event);
    void onStartForceDirected(wxCommandEvent& event);
    void onStopForceDirected(wxCommandEvent& event);
    void onResetStatePositions(wxCommandEvent& event);
  
    void setSim(Simulation* sim);
    void replaceMarkRule(int index, wxString str);
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

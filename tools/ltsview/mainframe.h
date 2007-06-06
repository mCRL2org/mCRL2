#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <string>
#include <wx/wx.h>
#include <wx/checklst.h>
#include <wx/filename.h>
#include <wx/listctrl.h>
#include <wx/progdlg.h>
#include "settingsdialog.h"
#include "savepicdialog.h"
#include "glcanvas.h"
#include "mediator.h"
#include "settings.h"
#include "simreader.h"

class MainFrame : public wxFrame, public simReader {
  public:
    MainFrame(Mediator* owner,Settings* ss);
    void  addMarkRule(wxString str);
    void  createProgressDialog(const std::string title,const std::string text);
    GLCanvas*  getGLCanvas() const;
    void  loadTitle();

    void        onAbout(wxCommandEvent &event);
    void  onActivateTool(wxCommandEvent& event);
    void  onAddMarkRuleButton(wxCommandEvent& event);
    void  onDisplay(wxCommandEvent& event);
    void  onExit(wxCommandEvent& event);
    void  onOpen(wxCommandEvent& event);
    void        onSavePic(wxCommandEvent& event);
    void  onRankStyle(wxCommandEvent& event);
    void  onVisStyle(wxCommandEvent& event);
    void  onRemoveMarkRuleButton(wxCommandEvent& event);
    void  onResetButton(wxCommandEvent& event);
    void  onResetView(wxCommandEvent& event);
    void  onSettings(wxCommandEvent& event);
    void  onMarkAnyAll(wxCommandEvent& event);
    void  onMarkRadio(wxCommandEvent& event);
    void  onMarkRuleActivate(wxCommandEvent& event);
    void  onMarkRuleEdit(wxCommandEvent& event);
    void  onMarkTransition(wxCommandEvent& event);
    
    // Simulation event handlers
    void        onSimStartButton(wxCommandEvent& event);
    void        onSimResetButton(wxCommandEvent& event);
    void        onSimStopButton(wxCommandEvent& event);
    void        onSimTransitionSelected(wxListEvent& event);
    void        onSimTransitionActivated(wxListEvent& event);
    void        onSimTriggerButton(wxCommandEvent& event);
    void        onSimUndoButton(wxCommandEvent& event);
    void        onSimStateSelected(wxListEvent& event);
  

    void  replaceMarkRule(int index, wxString str);
    void  resetMarkRules();
    void  setActionLabels(std::vector< std::string > &labels);
    void  setFileInfo(wxFileName fn);
    void  setMarkedStatesInfo(int number);
    void  setMarkedTransitionsInfo(int number);
    void  setNumberInfo(int ns,int nt,int nc,int nr);
    void  showMessage(std::string title,std::string text);
    void  startRendering();
    void  stopRendering();
    void  updateProgressDialog(int val,std::string msg);

    // Implemented for simReader interface
    virtual void refresh();
    virtual void selChange();
//    void  onIdle(wxIdleEvent &event);/* needed for computing the frame rate */
  private:
//    double previousTime; /* needed for computing the frame rate (FPS) */
//    int    frameCount; /* needed for computing the frame rate (FPS) */
    wxFileName        filename;
    GLCanvas*          glCanvas;
    wxChoice*          markAnyAllChoice;
    wxRadioButton*    markDeadlocksRadio;
    wxCheckListBox*   markStatesListBox;
    wxRadioButton*    markStatesRadio;
    wxCheckListBox*   markTransitionsListBox;
    wxRadioButton*    markTransitionsRadio;
    Mediator*          mediator;
    wxRadioButton*    nomarksRadio;
    wxStaticText*     ncLabel;
    wxStaticText*     nmsLabel;
    wxStaticText*     nmtLabel;
    wxStaticText*     nrLabel;
    wxStaticText*     nsLabel;
    wxStaticText*     ntLabel;
    wxProgressDialog* progDialog;
    SavePicDialog*     savePicDialog;
    SettingsDialog*   settingsDialog;
    Settings*         settings;

    // Buttons for simulation
    wxButton* simStartButton;
    wxButton* simResetButton;
    wxButton* simStopButton;
    wxButton* simTriggerButton;
    wxButton* simUndoButton;

    // List views used in simulation
    wxListView* simTransView; // Transition information
    wxListView* simStateView; // State information

    void setupMainArea();
    void setupMenuBar();
    void setupRightPanel(wxPanel* panel);
    void setupMarkPanel(wxPanel* panel);
    void setupSimPanel(wxPanel* panel);
//    void setupSettingsPanel(wxPanel* panel);

    DECLARE_EVENT_TABLE()
};
#endif

#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <string>
#include <wx/wx.h>
#include <wx/checklst.h>
#include <wx/filename.h>
#include <wx/progdlg.h>
#include "aterm1.h"
#include "settingsdialog.h"
#include "glcanvas.h"
#include "mediator.h"
#include "settings.h"

class MainFrame : public wxFrame {
  public:
    MainFrame(Mediator* owner,Settings* ss);
    void	addMarkRule(wxString str);
    void	createProgressDialog(const std::string title,const std::string text);
    GLCanvas*	getGLCanvas() const;
    void	loadTitle();
    void  onAbout(wxCommandEvent &event);
    void	onActivateTool(wxCommandEvent& event);
    void	onAddMarkRuleButton(wxCommandEvent& event);
    void	onDisplay(wxCommandEvent& event);
    void	onExit(wxCommandEvent& event);
    void	onOpen(wxCommandEvent& event);
    void  onSavePic(wxCommandEvent& event);
    void	onRankStyle(wxCommandEvent& event);
    void	onVisStyle(wxCommandEvent& event);
    void	onRemoveMarkRuleButton(wxCommandEvent& event);
    void	onResetButton(wxCommandEvent& event);
    void	onResetView(wxCommandEvent& event);
    void	onSettings(wxCommandEvent& event);
    void	onMarkAnyAll(wxCommandEvent& event);
    void	onMarkRadio(wxCommandEvent& event);
    void	onMarkRuleActivate(wxCommandEvent& event);
    void	onMarkRuleEdit(wxCommandEvent& event);
    void	onMarkTransition(wxCommandEvent& event);
    void	replaceMarkRule(int index, wxString str);
    void	resetMarkRules();
    void	setActionLabels(std::vector< ATerm > &labels);
    void	setFileInfo(wxFileName fn);
    void	setMarkedStatesInfo(int number);
    void	setMarkedTransitionsInfo(int number);
    void	setNumberInfo(int ns,int nt,int nc,int nr);
    void	showMessage(std::string title,std::string text);
    void	startRendering();
    void	stopRendering();
    void	updateProgressDialog(int val,std::string msg);
//    void  onIdle(wxIdleEvent &event);/* needed for computing the frame rate */
  private:
//    double previousTime; /* needed for computing the frame rate (FPS) */
//    int    frameCount; /* needed for computing the frame rate (FPS) */
    wxFileName        filename;
    GLCanvas*	        glCanvas;
    wxChoice*	        markAnyAllChoice;
    wxRadioButton*    markDeadlocksRadio;
    wxCheckListBox*   markStatesListBox;
    wxRadioButton*    markStatesRadio;
    wxCheckListBox*   markTransitionsListBox;
    wxRadioButton*    markTransitionsRadio;
    Mediator*	        mediator;
    wxRadioButton*    nomarksRadio;
    wxStaticText*     ncLabel;
    wxStaticText*     nmsLabel;
    wxStaticText*     nmtLabel;
    wxStaticText*     nrLabel;
    wxStaticText*     nsLabel;
    wxStaticText*     ntLabel;
    wxProgressDialog* progDialog;
    SettingsDialog*   settingsDialog;
    Settings*         settings;

    void setupMainArea();
    void setupMenuBar();
    void setupRightPanel(wxPanel* panel);
    void setupMarkPanel(wxPanel* panel);
//    void setupSettingsPanel(wxPanel* panel);

    DECLARE_EVENT_TABLE()
};
#endif

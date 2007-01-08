#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <string>
#include <wx/wx.h>
#include <wx/checklst.h>
#include <wx/filename.h>
#include <wx/progdlg.h>
#include <wx/spinctrl.h>
#include "aterm1.h"
#include "colorbutton.h"
#include "glcanvas.h"
#include "mediator.h"

class MainFrame : public wxFrame {
  public:
    MainFrame(Mediator* owner);
    void	addMarkRule(wxString str);
    void	createProgressDialog(const std::string title,const std::string text);
    Utils::RGB_Color getBackgroundColor() const;
    GLCanvas*	getGLCanvas() const;
    Utils::VisSettings getVisSettings() const;
    void	loadTitle();
    void  onAbout(wxCommandEvent &event);
    void	onActivateTool(wxCommandEvent& event);
    void	onAddMarkRuleButton(wxCommandEvent& event);
    void	onColorButton(wxCommandEvent& event);
    void	onDisplay(wxCommandEvent& event);
    void	onExit(wxCommandEvent& event);
    void	onOpen(wxCommandEvent& event);
    void  onSavePic(wxCommandEvent& event);
    void	onRankStyle(wxCommandEvent& event);
    void	onVisStyle(wxCommandEvent& event);
    void	onRemoveMarkRuleButton(wxCommandEvent& event);
    void	onResetButton(wxCommandEvent& event);
    void	onResetView(wxCommandEvent& event);
    void	onCommandSettingChanged(wxCommandEvent& event);
    void	onMarkAnyAll(wxCommandEvent& event);
    void	onMarkRadio(wxCommandEvent& event);
    void	onMarkRuleActivate(wxCommandEvent& event);
    void	onMarkRuleEdit(wxCommandEvent& event);
    void	onMarkTransition(wxCommandEvent& event);
    void	onSpinSettingChanged(wxSpinEvent& event);
    void	replaceMarkRule(int index, wxString str);
    void	resetMarkRules();
    void	setActionLabels(std::vector< ATerm > &labels);
    void	setBackgroundColor(Utils::RGB_Color c);
    void	setFileInfo(wxFileName fn);
    void	setMarkedStatesInfo(int number);
    void	setMarkedTransitionsInfo(int number);
    void	setNumberInfo(int ns,int nt,int nc,int nr);
    void  setVisSettings(Utils::VisSettings ss);
    void	showMessage(std::string title,std::string text);
    void	startRendering();
    void	stopRendering();
    void	updateProgressDialog(int val,std::string msg);
//    void  onIdle(wxIdleEvent &event);/* needed for computing the frame rate */
  private:
//    double previousTime; /* needed for computing the frame rate (FPS) */
//    int    frameCount; /* needed for computing the frame rate (FPS) */
    wxColorButton*    backgroundButton;
    wxSpinCtrl*	      branchrotationSpinCtrl;
    wxColorButton*    downEdgeButton;
    wxSpinCtrl*       ellipsoidSpinCtrl;
    wxFileName        filename;
    GLCanvas*	        glCanvas;
    wxSpinCtrl*	      innerbranchtiltSpinCtrl;
    wxColorButton*    interpolate1Button;
    wxColorButton*    interpolate2Button;
    wxCheckBox*	      longinterpolateCheckBox;
    wxChoice*	        markAnyAllChoice;
    wxColorButton*    markButton;
    wxRadioButton*    markDeadlocksRadio;
    wxCheckListBox*   markStatesListBox;
    wxRadioButton*    markStatesRadio;
    wxCheckListBox*   markTransitionsListBox;
    wxRadioButton*    markTransitionsRadio;
    Mediator*	        mediator;
    wxColorButton*    nodeButton;
    wxSpinCtrl*       nodesizeSpinCtrl;
    wxRadioButton*    nomarksRadio;
    wxStaticText*     numberOfClustersLabel;
    wxStaticText*     numberOfMarkedStatesLabel;
    wxStaticText*     numberOfMarkedTransitionsLabel;
    wxStaticText*     numberOfRanksLabel;
    wxStaticText*     numberOfStatesLabel;
    wxStaticText*     numberOfTransitionsLabel;
    wxSpinCtrl*	      outerbranchtiltSpinCtrl;
    wxSpinCtrl*	      qualitySpinCtrl;
    wxProgressDialog* progDialog;
    wxSpinCtrl*	      transparencySpinCtrl;
    wxColorButton*    upEdgeButton;

    void setupMainArea();
    void setupMenuBar();
    void setupRightPanel(wxPanel* panel);
    void setupMarkPanel(wxPanel* panel);
    void setupSettingsPanel(wxPanel* panel);
    wxColour  RGB_to_wxC(Utils::RGB_Color c) const;
    Utils::RGB_Color wxC_to_RGB(wxColour c) const;

    DECLARE_EVENT_TABLE()
};
#endif

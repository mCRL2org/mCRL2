#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <string>
#include <wx/toolbar.h>
#include <wx/sysopt.h>
#include <wx/wx.h>
#include <wx/filedlg.h>
#include <wx/colordlg.h>
#include <wx/progdlg.h>
#include <wx/notebook.h>
#include <wx/spinctrl.h>
#include <wx/bitmap.h>
#include <wx/event.h>
#include <wx/checklst.h>
#include "aterm/aterm2.h"
#include "spinctrlfloat.h"
#include "colorbutton.h"
#include "mediator.h"
#include "glcanvas.h"
#include "ids.h"

using namespace std;
using namespace IDs;

class MainFrame : public wxFrame
{
  public:
    MainFrame( Mediator* owner );
    void	addMarkRule( wxString str );
    void	createProgressDialog( const string title, const string text );
    GLCanvas*	getGLCanvas() const;
    VisSettings	getVisSettings() const;
    void	loadTitle();
    void	onActivateTool( wxCommandEvent& event );
    void	onAddMarkRuleButton( wxCommandEvent& event );
    void	onColorButton( wxCommandEvent& event );
    void	onExit( wxCommandEvent& event );
    void	onOpen( wxCommandEvent& event );
    void	onRankStyle( wxCommandEvent& event );
    void	onRemoveMarkRuleButton( wxCommandEvent& event );
    void	onResetButton( wxCommandEvent& event );
    void	onResetView( wxCommandEvent& event );
    void	onCommandSettingChanged( wxCommandEvent& event );
    void	onMarkAnyAll( wxCommandEvent& event );
    void	onMarkRadio( wxCommandEvent& event );
    void	onMarkRuleActivate( wxCommandEvent& event );
    void	onMarkRuleEdit( wxCommandEvent& event );
    void	onMarkTransition( wxCommandEvent& event );
    void	onSpinSettingChanged( wxSpinEvent& event );
    void	replaceMarkRule( int index, wxString str );
    void	resetMarkRules();
    void	setActionLabels( ATermList labels );
    void	setMarkedStatesInfo( int number );
    void	setMarkedTransitionsInfo( int number );
    void	setNumberInfo( int nstates, int ntransitions, int nclusters, int nranks );
    void	setVisSettings( VisSettings ss );
    void	showMessage( string title, string text );
    void	updateProgressDialog( int val, string msg );
  private:
    wxColorButton*    backgroundButton;
    wxSpinCtrlFloat*  backpointerSpinCtrl;
//    wxSpinCtrl*	      branchspreadSpinCtrl;
//    wxSpinCtrlFloat*  clusterheightSpinCtrl;
    wxString	      directory;
    wxColorButton*    downEdgeButton;
    wxString	      filename;
    GLCanvas*	      glCanvas;
    wxColorButton*    interpolate1Button;
    wxColorButton*    interpolate2Button;
//    wxCheckBox*	      levelDivCheckBox;
    wxCheckBox*	      longinterpolateCheckBox;
    wxChoice*	      markAnyAllChoice;
    wxColorButton*    markButton;
    wxRadioButton*    markDeadlocksRadio;
    wxCheckListBox*   markStatesListBox;
    wxRadioButton*    markStatesRadio;
    wxCheckListBox*   markTransitionsListBox;
    wxRadioButton*    markTransitionsRadio;
    Mediator*	      mediator;
    wxColorButton*    nodeButton;
    wxSpinCtrlFloat*  nodesizeSpinCtrl;
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
    wxToolBar*	      toolBar;
    wxSpinCtrl*	      transparencySpinCtrl;
    wxColorButton*    upEdgeButton;

    void setupMainArea();
    void setupMenuBar();
    void setupRightPanel( wxPanel* panel );
    void setupMarkPanel( wxPanel* panel );
    void setupSettingsPanel( wxPanel* panel );
    void setupToolBar();

    DECLARE_EVENT_TABLE()
};
#endif

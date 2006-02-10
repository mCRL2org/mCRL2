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
    void	createProgressDialog( const string title, const string text );
    GLCanvas*	getGLCanvas() const;
    VisSettings	getVisSettings() const;
    void	loadTitle();
    void	onActivateTool( wxCommandEvent& event );
    void	onAddMarkStateRuleButton( wxCommandEvent& event );
    void	onChoice( wxCommandEvent& event );
    void	onColorButton( wxCommandEvent& event );
    void	onOpen( wxCommandEvent& event );
    void	onRemoveMarkStateRuleButton( wxCommandEvent& event );
    void	onResetButton( wxCommandEvent& event );
    void	onResetView( wxCommandEvent& event );
    void	onCommandSettingChanged( wxCommandEvent& event );
    void	onSpinSettingChanged( wxSpinEvent& event );
    void	setVisSettings( VisSettings ss );
    void	setNumberInfo( int nstates, int ntransitions, int nclusters, int nranks );
    void	showMessage( string title, string text );
    void	updateProgressDialog( int val, string msg );
  private:
    wxColorButton*    backgroundButton;
    wxSpinCtrlFloat*  backpointerSpinCtrl;
    wxSpinCtrl*	      branchspreadSpinCtrl;
    wxSpinCtrlFloat*  clusterheightSpinCtrl;
    VisSettings	      defaultSettings;
    wxString	      directory;
    wxColorButton*    downEdgeButton;
    wxString	      filename;
    GLCanvas*	      glCanvas;
    wxColorButton*    interpolate1Button;
    wxColorButton*    interpolate2Button;
    wxCheckBox*	      levelDivCheckBox;
    wxCheckBox*	      longinterpolateCheckBox;
    wxColorButton*    markButton;
    Mediator*	      mediator;
    wxColorButton*    nodeButton;
    wxSpinCtrlFloat*  nodesizeSpinCtrl;
    wxStaticText*     numberOfClustersLabel;
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

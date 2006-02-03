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
#include "spinctrlfloat.h"
#include "icons.h"
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
    void	onSettingChanged( wxSpinEvent& event );
    void	setVisSettings( VisSettings ss );
    void	setNumberInfo( int nstates, int ntransitions, int nclusters, int nranks );
    void	showMessage( string title, string text );
    void	updateProgressDialog( int val, string msg );
  private:
    wxButton*	      backgroundButton;
    wxSpinCtrlFloat*    backpointerSpinCtrl;
    wxSpinCtrl*	      branchspreadSpinCtrl;
    wxSpinCtrlFloat*    clusterheightSpinCtrl;
    VisSettings	      defaultSettings;
    wxString	      directory;
    wxButton*	      downEdgeButton;
    wxString	      filename;
    GLCanvas*	      glCanvas;
    wxButton*	      interpolate1Button;
    wxButton*	      interpolate2Button;
    wxCheckBox*	      levelDivCheckBox;
    wxCheckBox*	      longinterpolateCheckBox;
    wxButton*	      markButton;
    Mediator*	      mediator;
    wxButton*	      nodeButton;
    wxSpinCtrlFloat*    nodesizeSpinCtrl;
    wxStaticText*     numberOfClustersLabel;
    wxStaticText*     numberOfRanksLabel;
    wxStaticText*     numberOfStatesLabel;
    wxStaticText*     numberOfTransitionsLabel;
    wxSpinCtrl*	      outerbranchtiltSpinCtrl;
    wxSpinCtrl*	      qualitySpinCtrl;
    wxProgressDialog* progDialog;
    wxToolBar*	      toolBar;
    wxSpinCtrl*	      transparencySpinCtrl;
    wxButton*	      upEdgeButton;

    void setupMainArea();
    void setupMenuBar();
    void setupRightArea( wxFlexGridSizer* sizer );
    void setupMarkPanel( wxPanel* panel );
    void setupSettingsPanel( wxPanel* panel );
    void setupToolBar();

    DECLARE_EVENT_TABLE()
};
#endif

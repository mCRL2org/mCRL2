#include "mainframe.h"

// Event table
BEGIN_EVENT_TABLE( MainFrame, wxFrame )
  EVT_TOOL  ( wxID_OPEN, MainFrame::onOpen )
  EVT_MENU  ( wxID_OPEN, MainFrame::onOpen )
  EVT_MENU  ( wxID_RESET, MainFrame::onResetView )
  EVT_TOOL  ( myID_PAN, MainFrame::onActivateTool )
  EVT_TOOL  ( myID_ROTATE, MainFrame::onActivateTool )
  EVT_TOOL  ( myID_SELECT, MainFrame::onActivateTool )
  EVT_TOOL  ( myID_ZOOM, MainFrame::onActivateTool )
  EVT_CHOICE( myID_RANK_STYLE, MainFrame::onChoice )
  EVT_BUTTON( myID_COLOR_BUTTON, MainFrame::onColorButton )
  EVT_SPINCTRL( myID_SETTINGS_CONTROL, MainFrame::onSettingChanged )
  EVT_BUTTON( wxID_RESET, MainFrame::onResetButton )
  EVT_BUTTON( myID_ADD_RULE, MainFrame::onAddMarkStateRuleButton )
  EVT_BUTTON( myID_REMOVE_RULE, MainFrame::onRemoveMarkStateRuleButton )
END_EVENT_TABLE()

MainFrame::MainFrame( Mediator* owner )
	 : wxFrame(NULL, wxID_ANY, wxT("LTSView"), wxDefaultPosition, wxSize( 1024, 768 ) )
{ 
  mediator = owner;
  directory = wxEmptyString;
  filename = wxEmptyString;
  progDialog = NULL;

  SetIcon( wxIcon( main_window_xpm ) );
  CentreOnScreen();

  setupMenuBar();
  setupToolBar();
  setupMainArea();
}

void MainFrame::setupMenuBar()
{
  // Set up the menu bar
  wxMenuBar* menuBar = new wxMenuBar;
  wxMenu* fileMenu = new wxMenu;
  wxMenu* viewMenu = new wxMenu;
  fileMenu->Append( wxID_OPEN, wxT("&Open...\tCtrl+O"),
      wxT("Load an LTS from file") );
  viewMenu->Append( wxID_RESET, wxT("&Reset viewpoint"),
      wxT("Set the viewpoint to the default position"));
  menuBar->Append( fileMenu, wxT("&File") );
  menuBar->Append( viewMenu, wxT("&View") );
  SetMenuBar( menuBar );
}

void MainFrame::setupToolBar()
{
  // Set up the toolbar
  wxSystemOptions::SetOption(wxT("msw.remap"), 0);
  toolBar = CreateToolBar();
  toolBar->AddTool( wxID_OPEN, wxT("Open"), wxIcon( file_open ),
      wxT("Open file") );
  toolBar->AddSeparator();
  toolBar->AddRadioTool( myID_SELECT, wxT("Select"), wxIcon( select_tool ),
      wxIcon( select_tool ), wxT("Select tool") );
  toolBar->AddRadioTool( myID_PAN,    wxT("Pan"), wxIcon( pan_tool ), wxIcon(
	pan_tool ), wxT("Pan tool") );
  toolBar->AddRadioTool( myID_ZOOM,   wxT("Zoom"), wxIcon( zoom_tool ), wxIcon(
	zoom_tool ), wxT("Zoom tool") );
  toolBar->AddRadioTool( myID_ROTATE, wxT("Rotate"), wxIcon( rotate_tool ),
      wxIcon( rotate_tool ), wxT("Rotate tool") );
  toolBar->Realize();
}

void MainFrame::setupMainArea()
{
  wxFlexGridSizer* mainSizer = new wxFlexGridSizer( 1, 2, 0, 0 );
  mainSizer->AddGrowableCol( 0 );
  mainSizer->AddGrowableRow( 0 );

  glCanvas = new GLCanvas( mediator, this, wxID_ANY );
  glCanvas->SetMinSize( wxSize( 800, 600 ) );

  wxFlexGridSizer* rightSizer = new wxFlexGridSizer( 2, 1, 0, 0 );
  rightSizer->AddGrowableRow( 1 );
  setupRightArea( rightSizer );
  
  mainSizer->Add( glCanvas, 1, wxALIGN_CENTER | wxEXPAND | wxALL, 0 );
  mainSizer->Add( rightSizer, 1, wxALIGN_CENTER | wxEXPAND | wxALL, 0 );
  mainSizer->Fit( this );
  SetSizer( mainSizer );
}

void MainFrame::setupRightArea( wxFlexGridSizer* sizer )
{
  int lflags = wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL;
  int rflags = wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxEXPAND | wxALL;
  
  // setup the top part (information box)
  wxFlexGridSizer* topSizer = new wxFlexGridSizer( 4, 2, 0, 0 );
  numberOfStatesLabel = new wxStaticText( this, wxID_ANY, wxT(""),
      wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
  numberOfTransitionsLabel = new wxStaticText( this, wxID_ANY, wxT(""),
      wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
  numberOfClustersLabel = new wxStaticText( this, wxID_ANY, wxT(""),
      wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
  numberOfRanksLabel = new wxStaticText( this, wxID_ANY, wxT(""),
      wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
  
  topSizer->AddGrowableCol( 1 );
  topSizer->Add( new wxStaticText( this, wxID_ANY, wxT("Number of states:") ), 0, lflags, 3 );
  topSizer->Add( numberOfStatesLabel, 0, rflags, 3 );
  topSizer->Add( new wxStaticText( this, wxID_ANY, wxT("Number of transitions:") ), 0, lflags, 3 );
  topSizer->Add( numberOfTransitionsLabel, 0, rflags, 3 );
  topSizer->Add( new wxStaticText( this, wxID_ANY, wxT("Number of clusters:") ), 0, lflags, 3 );
  topSizer->Add( numberOfClustersLabel, 0, rflags, 3 );
  topSizer->Add( new wxStaticText( this, wxID_ANY, wxT("Number of ranks:") ), 0, lflags, 3 );
  topSizer->Add( numberOfRanksLabel, 0, rflags, 3 );

  // setup the bottom part (notebook)
  wxNotebook* bottomNotebook = new wxNotebook( this, wxID_ANY );
  wxPanel* settingsPanel = new wxPanel( bottomNotebook, wxID_ANY );
  wxPanel* markPanel = new wxPanel( bottomNotebook, wxID_ANY );
  
  setupSettingsPanel( settingsPanel );
  setupMarkPanel( markPanel );
  
  bottomNotebook->AddPage( settingsPanel, wxT("Settings"), true );
  bottomNotebook->AddPage( markPanel, wxT("Mark"), false );
  
  sizer->Add( topSizer, 0, wxEXPAND | wxALL, 5 );
  sizer->Add( bottomNotebook, 0, wxEXPAND | wxALL, 5 );
}

void MainFrame::setupSettingsPanel( wxPanel* panel )
{
  wxStaticBoxSizer* parSizer = new wxStaticBoxSizer( wxVERTICAL, panel, wxT("Parameters") );

  wxFlexGridSizer* parsubSizer = new wxFlexGridSizer( 8, 3, 0, 0 );
  parsubSizer->AddGrowableCol( 1 );
  parsubSizer->AddGrowableCol( 2 );
  parsubSizer->AddGrowableRow( 0 );
  parsubSizer->AddGrowableRow( 1 );
  parsubSizer->AddGrowableRow( 2 );
  parsubSizer->AddGrowableRow( 3 );
  parsubSizer->AddGrowableRow( 4 );
  parsubSizer->AddGrowableRow( 5 );
  parsubSizer->AddGrowableRow( 6 );
  parsubSizer->AddGrowableRow( 7 );
  parSizer->Add( parsubSizer, 1, wxEXPAND | wxALL, 0 );
  
  int flags = wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL; 
  int border = 3;

  wxSize spinctrlSize( 50, -1 );

  nodesizeSpinCtrl = new wxSpinCtrlDbl( panel, myID_SETTINGS_CONTROL,
      wxEmptyString, wxDefaultPosition, spinctrlSize );
  nodesizeSpinCtrl->SetSizeHints( spinctrlSize, spinctrlSize );
  parsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("Node size:") ), 0, flags, border );
  parsubSizer->Add( nodesizeSpinCtrl, 0, flags, border );
  parsubSizer->AddSpacer( 0 );
  
  backpointerSpinCtrl = new wxSpinCtrlDbl( panel, myID_SETTINGS_CONTROL,
      wxEmptyString, wxDefaultPosition, spinctrlSize  );
  backpointerSpinCtrl->SetSizeHints( spinctrlSize, spinctrlSize );
  parsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("Backpointer curve:") ), 0, flags, border );
  parsubSizer->Add( backpointerSpinCtrl, 0, flags, border );
  parsubSizer->AddSpacer( 0 );
  
  clusterheightSpinCtrl = new wxSpinCtrlDbl( panel, myID_SETTINGS_CONTROL,
      wxEmptyString, wxDefaultPosition, spinctrlSize  );
  clusterheightSpinCtrl->SetSizeHints( spinctrlSize, spinctrlSize );
  parsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("Cluster height:") ), 0, flags, border );
  parsubSizer->Add( clusterheightSpinCtrl, 0, flags, border );
  parsubSizer->AddSpacer( 0 );
  /*
  parsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("Branch scale:") ), 0, flags, border );
  branchscaleSpinCtrl = new wxSpinCtrl( panel, myID_SETTINGS_CONTROL );
  parsubSizer->Add( branchscaleSpinCtrl, 0, flags, border );
  parsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("%") ), 0, flags, 0 );
  */
  branchspreadSpinCtrl = new wxSpinCtrl( panel, myID_SETTINGS_CONTROL );
  branchspreadSpinCtrl->SetMinSize( spinctrlSize );
  branchspreadSpinCtrl->SetMaxSize( spinctrlSize );
  parsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("Branch spread:") ), 0, flags, border );
  parsubSizer->Add( branchspreadSpinCtrl, 0, flags, border );
  parsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("%") ), 0, flags, 0 );
  /*
  parsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("Branch rotation:") ), 0, flags, border );
  branchrotationSpinCtrl = new wxSpinCtrl( panel, myID_SETTINGS_CONTROL );
  branchrotationSpinCtrl->SetRange( 0, 360 );
  parsubSizer->Add( branchrotationSpinCtrl, 0, flags, border );
  parsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("\302\272") ), 0, flags, 0 );
  */
  outerbranchtiltSpinCtrl = new wxSpinCtrl( panel, myID_SETTINGS_CONTROL,
      wxEmptyString, wxDefaultPosition );
  outerbranchtiltSpinCtrl->SetRange( 0, 90 );
  outerbranchtiltSpinCtrl->SetMinSize( spinctrlSize );
  outerbranchtiltSpinCtrl->SetMaxSize( spinctrlSize );
  parsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("Outer branch tilt:") ), 0, flags, border );
  parsubSizer->Add( outerbranchtiltSpinCtrl, 0, flags, border );
  parsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxString("\xc2\xb0", wxConvUTF8) ), 0, flags, 0 );
  
  qualitySpinCtrl = new wxSpinCtrl( panel, myID_SETTINGS_CONTROL, wxEmptyString,
      wxDefaultPosition );
  qualitySpinCtrl->SetRange( 4, 100 );
  qualitySpinCtrl->SetMinSize( spinctrlSize );
  qualitySpinCtrl->SetMaxSize( spinctrlSize );
  parsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("Rendering quality:") ), 0, flags, border );
  parsubSizer->Add( qualitySpinCtrl, 0, flags, border );
  parsubSizer->AddSpacer( 0 );
  /*
  qualitySlider = new wxSlider( panel, myID_SETTINGS_CONTROL, 4, 4, 100,
      wxDefaultPosition, wxSize( 100, -1 ), wxSL_HORIZONTAL | wxSL_LABELS |
      wxSL_AUTOTICKS );
  parsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("Rendering quality:") ), 0, flags, border );
  parsubSizer->Add( qualitySlider, 0, flags, border );
  parsubSizer->AddSpacer( 0 );
  */
  
  levelDivCheckBox = new wxCheckBox( panel, myID_SETTINGS_CONTROL,
	wxT("Show level dividers") );
  parsubSizer->Add( levelDivCheckBox, 0, flags, border );
  
  // Setup the Colors panel
  
  wxStaticBoxSizer* colSizer = new wxStaticBoxSizer( wxVERTICAL, panel, wxT("Colours") );
  
  wxFlexGridSizer* colsubSizer = new wxFlexGridSizer( 8, 3, 0, 0 );
  colsubSizer->AddGrowableRow( 0 );
  colsubSizer->AddGrowableRow( 1 );
  colsubSizer->AddGrowableRow( 2 );
  colsubSizer->AddGrowableRow( 3 );
  colsubSizer->AddGrowableRow( 4 );
  colsubSizer->AddGrowableRow( 5 );
  colsubSizer->AddGrowableRow( 6 );
  colsubSizer->AddGrowableRow( 7 );
  colSizer->Add( colsubSizer, 1, wxEXPAND | wxALL, 0 );
  
  /*transparencySpinCtrl = new wxSpinCtrl( panel, myID_SETTINGS_CONTROL );
  transparencySpinCtrl->SetSizeHints( wxSize(50,25), wxSize(50,25) );
  colsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("Transparency:") ), 0, flags, border );
  colsubSizer->Add( transparencySpinCtrl, 0, flags, border );
  colsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("%") ), 0, flags, 0 );*/

  wxSize btnSize( 25, 25 );
  
  backgroundButton = new wxButton( panel, myID_COLOR_BUTTON );
  backgroundButton->SetSizeHints( btnSize, btnSize );
  colsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("Background:") ), 0, flags, border );
  colsubSizer->Add( backgroundButton, 0, flags, border );
  colsubSizer->AddSpacer( 0 );

  nodeButton = new wxButton( panel, myID_COLOR_BUTTON );
  nodeButton->SetSizeHints( btnSize, btnSize );
  colsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("Node:") ), 0, flags, border );
  colsubSizer->Add( nodeButton, 0, flags, border );
  colsubSizer->AddSpacer( 0 );

  downEdgeButton = new wxButton( panel, myID_COLOR_BUTTON );
  downEdgeButton->SetSizeHints( btnSize, btnSize );
  colsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("Edge (down):") ), 0, flags, border );
  colsubSizer->Add( downEdgeButton, 0, flags, border );
  colsubSizer->AddSpacer( 0 );

  upEdgeButton = new wxButton( panel, myID_COLOR_BUTTON );
  upEdgeButton->SetSizeHints( btnSize, btnSize );
  colsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("Edge (up):") ), 0, flags, border );
  colsubSizer->Add( upEdgeButton, 0, flags, border );
  colsubSizer->AddSpacer( 0 );

  markButton = new wxButton( panel, myID_COLOR_BUTTON );
  markButton->SetSizeHints( btnSize, btnSize );
  colsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("Mark:") ), 0, flags, border );
  colsubSizer->Add( markButton, 0, flags, border );
  colsubSizer->AddSpacer( 0 );

  interpolate1Button = new wxButton( panel, myID_COLOR_BUTTON );
  interpolate1Button->SetSizeHints( btnSize, btnSize );
  interpolate2Button = new wxButton( panel, myID_COLOR_BUTTON );
  interpolate2Button->SetSizeHints( btnSize, btnSize );
  colsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("Interpolate:") ), 0, flags, border );
  colsubSizer->Add( interpolate1Button, 0, flags, border );
  colsubSizer->Add( interpolate2Button, 0, flags, border );
  
  longinterpolateCheckBox = new wxCheckBox( panel, myID_SETTINGS_CONTROL,
      wxT("Long interpolation") );
  colsubSizer->Add( longinterpolateCheckBox, 0, flags, border );

  wxFlexGridSizer* panelSizer = new wxFlexGridSizer( 4, 1, 0, 0 );
  panelSizer->AddGrowableCol( 0 );
  panelSizer->AddGrowableRow( 0 );
  panelSizer->AddGrowableRow( 1 );
  //panelSizer->AddGrowableRow( 2 );
  //panelSizer->AddGrowableRow( 3 );
  panelSizer->Add( parSizer, 1, wxEXPAND | wxALL, 5 );
  panelSizer->Add( colSizer, 1, wxEXPAND | wxALL, 5 );

  wxFlexGridSizer* rankstyleSizer = new wxFlexGridSizer( 1, 2, 0, 0 );
  rankstyleSizer->AddGrowableCol( 0 );
  rankstyleSizer->AddGrowableCol( 1 );
  //rankstyleSizer->AddGrowableRow( 0 );
  rankstyleSizer->Add( new wxStaticText( panel, wxID_ANY, wxT("Rank style:") ),
      1, flags, border );
  wxString choices[2] = { wxT("Iterative"), wxT("Cyclic") };
  wxChoice* rankstyleChoice = new wxChoice( panel, myID_RANK_STYLE,
      wxDefaultPosition, wxDefaultSize, 2, choices );
  rankstyleChoice->SetSelection( 0 );
  rankstyleSizer->Add( rankstyleChoice, 1, flags, border );
  
  panelSizer->Add( rankstyleSizer, 0, wxEXPAND | wxALL, border );
  panelSizer->Add( new wxButton( panel, wxID_RESET, wxT("Default settings")
	), 0, flags, border );
  panel->SetSizer( panelSizer );
}

void MainFrame::setupMarkPanel( wxPanel* panel )
{
  wxFlexGridSizer* markSizer = new wxFlexGridSizer( 6, 1, 0, 0 );
  markSizer->AddGrowableCol( 0 );
  markSizer->AddGrowableRow( 4 );
  markSizer->AddGrowableRow( 5 );

  int flags = wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL;
  int border = 3;
  
  markSizer->Add( new wxRadioButton( panel, wxID_ANY, wxT("No marks"),
	wxDefaultPosition, wxDefaultSize, wxRB_GROUP ), 0, flags, border );
  markSizer->Add( new wxRadioButton( panel, wxID_ANY, wxT("Mark deadlocks") ),
      0, flags, border );
  markSizer->Add( new wxRadioButton( panel, wxID_ANY, wxT("Mark states") ),
      0, flags, border );
  markSizer->Add( new wxRadioButton( panel, wxID_ANY, wxT("Mark transitions") ),
      0, flags, border );
  
  wxStaticBoxSizer* markStatesSizer = new wxStaticBoxSizer( wxVERTICAL, panel,
      wxT("Mark states") );
  wxString choices[2] = { wxT("Match all of the following"),
      wxT("Match any of the following") };
  wxChoice* anyallChoice = new wxChoice( panel, myID_MARK_ANYALL,
      wxDefaultPosition, wxDefaultSize, 2, choices );
  anyallChoice->SetSelection( 0 );
  markStatesSizer->Add( anyallChoice, 0, flags, border );
  
  markStatesSizer->Add( new wxListBox( panel, myID_MARK_RULES,
	wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_EXTENDED |
	wxLB_NEEDED_SB | wxLB_HSCROLL ), 1, flags | wxEXPAND, border );
  wxBoxSizer* addremoveSizer = new wxBoxSizer( wxHORIZONTAL );
  addremoveSizer->Add( new wxButton( panel, myID_ADD_RULE, wxT("Add") ), 0,
      flags, border );
  addremoveSizer->Add( new wxButton( panel, myID_REMOVE_RULE, wxT("Remove") ),
      0, flags, border );
  markStatesSizer->Add( addremoveSizer, 0, flags, border );
  
  wxStaticBoxSizer* markTransitionsSizer = new wxStaticBoxSizer( wxVERTICAL,
      panel, wxT("Mark transitions") );
  markTransitionsSizer->Add( new wxCheckListBox( panel, myID_MARK_TRANSITIONS,
	wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE | wxLB_SORT |
	wxLB_NEEDED_SB | wxLB_HSCROLL ), 1, flags | wxEXPAND, border );
  
  markSizer->Add( markStatesSizer, 0, wxEXPAND | wxALL, border );
  markSizer->Add( markTransitionsSizer, 0, wxEXPAND | wxALL, border );
  panel->SetSizer( markSizer );
}

GLCanvas* MainFrame::getGLCanvas() const
{
  return glCanvas;
}

void MainFrame::onOpen( wxCommandEvent& event )
{
  wxString filemask = wxT("FSM files (*.fsm)|*.fsm");
  wxFileDialog dialog( this, wxT("Open LTS"), directory, filename, filemask, wxOPEN );
  if ( dialog.ShowModal() == wxID_OK )
  {
    directory = dialog.GetDirectory();
    filename = dialog.GetFilename();
    mediator->openFile( string(dialog.GetPath().fn_str()) );
  }
}

void MainFrame::onActivateTool( wxCommandEvent& event )
{
  glCanvas->setActiveTool( event.GetId() );
}

void MainFrame::onChoice( wxCommandEvent& event )
{
  mediator->setRankStyle( string(event.GetString().fn_str()) );
}

void MainFrame::onResetView( wxCommandEvent& event )
{
  glCanvas->resetView();
}

void MainFrame::onColorButton( wxCommandEvent& event )
{
  wxButton* btn = (wxButton*)(event.GetEventObject());
  wxColourData coldat;
  coldat.SetColour( btn->GetBackgroundColour() );
  wxColourDialog* coldlg = new wxColourDialog( this, &coldat );
  if ( coldlg->ShowModal() == wxID_OK )
  {
    coldat = coldlg->GetColourData();
    btn->SetBackgroundColour( coldat.GetColour() );
    btn->Refresh();
    mediator->applySettings();
  }
}

void MainFrame::onSettingChanged( wxSpinEvent& event )
{
  mediator->applySettings();
}

void MainFrame::onResetButton( wxCommandEvent& event )
{
  mediator->applyDefaultSettings();
}

void MainFrame::onAddMarkStateRuleButton( wxCommandEvent& event )
{
  mediator->showMarkStateRuleDialog();
}

void MainFrame::onRemoveMarkStateRuleButton( wxCommandEvent& event )
{
  wxArrayInt selections;
  wxListBox* markstatesListBox = (wxListBox*) this->FindWindowById( myID_MARK_RULES );
  markstatesListBox->GetSelections( selections );
  for ( int i = selections.Count() - 1 ; i >= 0 ; --i )
    markstatesListBox->Delete( selections[i] );
}

VisSettings MainFrame::getVisSettings() const
{
  RGB_Color bgC =
  {
    backgroundButton->GetBackgroundColour().Red(),
    backgroundButton->GetBackgroundColour().Green(),
    backgroundButton->GetBackgroundColour().Blue()
  };
  RGB_Color downC =
  {
    downEdgeButton->GetBackgroundColour().Red(),
    downEdgeButton->GetBackgroundColour().Green(),
    downEdgeButton->GetBackgroundColour().Blue()
  };
  RGB_Color intC1 =
  {
    interpolate1Button->GetBackgroundColour().Red(),
    interpolate1Button->GetBackgroundColour().Green(),
    interpolate1Button->GetBackgroundColour().Blue()
  };
  RGB_Color intC2 =
  {
    interpolate2Button->GetBackgroundColour().Red(),
    interpolate2Button->GetBackgroundColour().Green(),
    interpolate2Button->GetBackgroundColour().Blue()
  };
  RGB_Color markC =
  {
    markButton->GetBackgroundColour().Red(),
    markButton->GetBackgroundColour().Green(),
    markButton->GetBackgroundColour().Blue()
  };
  RGB_Color stateC =
  {
    nodeButton->GetBackgroundColour().Red(),
    nodeButton->GetBackgroundColour().Green(),
    nodeButton->GetBackgroundColour().Blue()
  };
  RGB_Color upC =
  {
    upEdgeButton->GetBackgroundColour().Red(),
    upEdgeButton->GetBackgroundColour().Green(),
    upEdgeButton->GetBackgroundColour().Blue()
  };
  VisSettings result =
  {
    bgC,
    backpointerSpinCtrl->GetValue(),
//    branchrotationSpinCtrl->GetValue(),
//    branchscaleSpinCtrl->GetValue(),
    branchspreadSpinCtrl->GetValue(),
    clusterheightSpinCtrl->GetValue(),
    downC,
    intC1,
    intC2,
    levelDivCheckBox->GetValue(),
    longinterpolateCheckBox->GetValue(),
    markC,
    nodesizeSpinCtrl->GetValue(),
    outerbranchtiltSpinCtrl->GetValue(),
    qualitySpinCtrl->GetValue(),
    stateC,
    //transparencySpinCtrl->GetValue(),
    0,
    upC
  };
  return result;
}

void MainFrame::setVisSettings( VisSettings ss )
{
  backgroundButton->SetBackgroundColour( wxColour( ss.backgroundColor.r,
	ss.backgroundColor.g, ss.backgroundColor.b ) );
  nodeButton->SetBackgroundColour( wxColour( ss.stateColor.r,
	ss.stateColor.g, ss.stateColor.b ) );
  downEdgeButton->SetBackgroundColour( wxColour( ss.downEdgeColor.r,
	ss.downEdgeColor.g, ss.downEdgeColor.b ) );
  upEdgeButton->SetBackgroundColour( wxColour( ss.upEdgeColor.r,
	ss.upEdgeColor.g, ss.upEdgeColor.b ) );
  markButton->SetBackgroundColour( wxColour( ss.markedColor.r,
	ss.markedColor.g, ss.markedColor.b ) );
  interpolate1Button->SetBackgroundColour( wxColour( ss.interpolateColor1.r,
	ss.interpolateColor1.g, ss.interpolateColor1.b ) );
  interpolate2Button->SetBackgroundColour( wxColour( ss.interpolateColor2.r,
	ss.interpolateColor2.g, ss.interpolateColor2.b ) );
  longinterpolateCheckBox->SetValue( ss.longInterpolation );
  //transparencySpinCtrl->SetValue( ss.transparency );
  nodesizeSpinCtrl->SetValue( ss.nodeSize, 0.0, 100.0, 0.1 );
  backpointerSpinCtrl->SetValue( ss.backpointerCurve, 0.0, 100.0, 0.1 );
  clusterheightSpinCtrl->SetValue( ss.clusterHeight, 0.0, 100.0, 0.1 );
  branchspreadSpinCtrl->SetValue( ss.branchSpread );
  outerbranchtiltSpinCtrl->SetValue( ss.outerBranchTilt );
  qualitySpinCtrl->SetValue( ss.quality );
  levelDivCheckBox->SetValue( ss.levelDividers );
  //branchscaleSpinCtrl->SetValue( ss.branchScale );
  //branchrotationSpinCtrl->SetValue( ss.branchRotation );
}

void MainFrame::createProgressDialog( string title, string text )
{
  progDialog = new wxProgressDialog( (wxChar*)(title.c_str()),
      (wxChar*)(text.c_str()), 100, this, wxPD_APP_MODAL | wxPD_AUTO_HIDE | 
      wxPD_ELAPSED_TIME );
  progDialog->SetSize( wxSize( 400, 100 ) );
}

void MainFrame::updateProgressDialog( int val, string msg )
{
  if ( progDialog != NULL )
  {
    progDialog->Update( val, (wxChar*)(msg.c_str()) );
    if ( val == 100 )
    {
      delete progDialog;
      progDialog = NULL;
    }
  }
}

void MainFrame::showMessage( string title, string text )
{
  wxMessageDialog msgDialog( this, (wxChar*)(text.c_str()),
      (wxChar*)(title.c_str()), wxOK );
  msgDialog.ShowModal();
}

void MainFrame::loadTitle()
{
  if ( filename != wxT("") )
    SetTitle( filename + wxT(" - LTSView") );
  else
    SetTitle( wxT( "LTSView" ) );
}

void MainFrame::setNumberInfo( int nstates, int ntransitions, int nclusters, int nranks )
{
  numberOfStatesLabel->SetLabel( wxString::Format( wxT("%d"), nstates ) );
  numberOfTransitionsLabel->SetLabel( wxString::Format( wxT("%d"), ntransitions ) );
  numberOfClustersLabel->SetLabel( wxString::Format( wxT("%d"), nclusters ) );
  numberOfRanksLabel->SetLabel( wxString::Format( wxT("%d"), nranks ) );
}

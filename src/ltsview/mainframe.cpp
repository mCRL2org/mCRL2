#include "mainframe.h"
// include the icons
#include "icons/main_window.xpm"
#include "icons/file_open.xpm"
#include "icons/file_open_mask.xpm"
#include "icons/select_tool.xpm"
#include "icons/select_cursor_mask.xpm"
#include "icons/zoom_tool.xpm"
#include "icons/zoom_cursor_mask.xpm"
#include "icons/pan_tool.xpm"
#include "icons/pan_cursor_mask.xpm"
#include "icons/rotate_tool.xpm"
#include "icons/rotate_cursor_mask.xpm"

// Event table
BEGIN_EVENT_TABLE( MainFrame, wxFrame )
  EVT_TOOL  ( wxID_OPEN, MainFrame::onOpen )
  EVT_MENU  ( wxID_OPEN, MainFrame::onOpen )
  EVT_MENU  ( wxID_EXIT, MainFrame::onExit )
  EVT_MENU  ( wxID_RESET, MainFrame::onResetView )
  EVT_MENU  ( myID_DISPLAY_STATES, MainFrame::onDisplayStates )
  EVT_MENU  ( myID_DISPLAY_WIREFRAME, MainFrame::onDisplayWireframe )
  EVT_TOOL  ( myID_PAN, MainFrame::onActivateTool )
  EVT_TOOL  ( myID_ROTATE, MainFrame::onActivateTool )
  EVT_TOOL  ( myID_SELECT, MainFrame::onActivateTool )
  EVT_TOOL  ( myID_ZOOM, MainFrame::onActivateTool )
  EVT_CHOICE( myID_RANK_STYLE, MainFrame::onRankStyle )
  EVT_CHOICE( myID_VIS_STYLE, MainFrame::onVisStyle )
  EVT_BUTTON( myID_COLOR_BUTTON, MainFrame::onColorButton )
  EVT_SPINCTRL( myID_SETTINGS_CONTROL, MainFrame::onSpinSettingChanged )
  EVT_CHECKBOX( myID_SETTINGS_CONTROL, MainFrame::onCommandSettingChanged )
  EVT_BUTTON( wxID_RESET, MainFrame::onResetButton )
  EVT_RADIOBUTTON( myID_MARK_RADIOBUTTON, MainFrame::onMarkRadio )
  EVT_CHOICE( myID_MARK_ANYALL, MainFrame::onMarkAnyAll )
  EVT_LISTBOX_DCLICK( myID_MARK_RULES, MainFrame::onMarkRuleEdit )
  EVT_CHECKLISTBOX( myID_MARK_RULES, MainFrame::onMarkRuleActivate )
  EVT_CHECKLISTBOX( myID_MARK_TRANSITIONS, MainFrame::onMarkTransition )
  EVT_BUTTON( myID_ADD_RULE, MainFrame::onAddMarkRuleButton )
  EVT_BUTTON( myID_REMOVE_RULE, MainFrame::onRemoveMarkRuleButton )
END_EVENT_TABLE()

MainFrame::MainFrame( Mediator* owner )
	 : wxFrame(NULL, wxID_ANY, wxT("LTSView") )
{ 
  mediator = owner;
  directory = wxEmptyString;
  filename = wxEmptyString;
  progDialog = NULL;

  SetIcon( wxIcon( main_window ) );

  CreateStatusBar();
  setupMenuBar();
  setupToolBar();
  setupMainArea();
  
  SetSizeHints( GetSize() );
  CentreOnScreen();
}

void MainFrame::setupMenuBar()
{
  // Set up the menu bar
  wxMenuBar* menuBar = new wxMenuBar;
  wxMenu* fileMenu = new wxMenu;
  wxMenu* viewMenu = new wxMenu;
  
  fileMenu->Append( wxID_OPEN, wxT("&Open...\tCtrl+O"),
      wxT("Load an LTS from file") );
  fileMenu->AppendSeparator();
  fileMenu->Append( wxID_EXIT, wxT("E&xit\tCtrl+Q"), wxT("Exit application") );
    
  viewMenu->Append( wxID_RESET, wxT("&Reset viewpoint"),
      wxT("Set the viewpoint to the default position"));
  viewMenu->AppendSeparator();
  viewMenu->AppendCheckItem( myID_DISPLAY_STATES, wxT("Display &states"),
      wxT("Show/hide individual states") );
  viewMenu->AppendCheckItem( myID_DISPLAY_WIREFRAME, wxT("Display &wireframe"),
      wxT("Show as wireframe") );
  
  menuBar->Append( fileMenu, wxT("&File") );
  menuBar->Append( viewMenu, wxT("&View") );
  
  SetMenuBar( menuBar );
}

void MainFrame::setupToolBar()
{
  // Set up the toolbar
  wxSystemOptions::SetOption(wxT("msw.remap"), 0);
  toolBar = CreateToolBar();
  wxBitmap fileopenbmp( file_open );
  wxBitmap selectbmp( select_tool );
  wxBitmap panbmp( pan_tool );
  wxBitmap zoombmp( zoom_tool );
  wxBitmap rotatebmp( rotate_tool );
  fileopenbmp.SetMask( new wxMask( wxBitmap( file_open_mask ), wxColour( 255, 0, 0 ) ) );
  selectbmp.SetMask( new wxMask( wxBitmap( select_cursor_mask ), wxColour( 255, 0, 0 ) ) );
  panbmp.SetMask( new wxMask( wxBitmap( pan_cursor_mask ), wxColour( 255, 0, 0 ) ) );
  zoombmp.SetMask( new wxMask( wxBitmap( zoom_cursor_mask ), wxColour( 255, 0, 0 ) ) );
  rotatebmp.SetMask( new wxMask( wxBitmap( rotate_cursor_mask ), wxColour( 255, 0, 0 ) ) );
  
  toolBar->AddTool( wxID_OPEN, wxT("Open"), fileopenbmp, wxT("Open file") );
  toolBar->AddSeparator();
  toolBar->AddRadioTool( myID_SELECT, wxT("Select"), selectbmp, wxNullBitmap,
      wxT("Select tool") );
  toolBar->AddRadioTool( myID_PAN, wxT("Pan"), panbmp, wxNullBitmap,
      wxT("Pan tool") );
  toolBar->AddRadioTool( myID_ZOOM, wxT("Zoom"), zoombmp, wxNullBitmap,
      wxT("Zoom tool") );
  toolBar->AddRadioTool( myID_ROTATE, wxT("Rotate"), rotatebmp, wxNullBitmap,
      wxT("Rotate tool") );
  toolBar->Realize();
}

void MainFrame::setupMainArea()
{
  wxFlexGridSizer* mainSizer = new wxFlexGridSizer( 1, 2, 0, 0 );
  mainSizer->AddGrowableCol( 0 );
  mainSizer->AddGrowableRow( 0 );

  wxPanel* rightPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition,
      wxDefaultSize, wxRAISED_BORDER );
  setupRightPanel( rightPanel );

  int w; int h;
  rightPanel->GetSize( &w, &h );
  int attribList[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER };
  glCanvas = new GLCanvas( mediator, this, wxSize( h, h ), attribList );
  glCanvas->SetSizeHints( h, h );
  
  mainSizer->Add( glCanvas, 1, wxALIGN_CENTER | wxEXPAND | wxALL, 0 );
  mainSizer->Add( rightPanel, 1, wxALIGN_CENTER | wxEXPAND | wxALL, 0 );
  
  mainSizer->Fit( this );
  SetSizer( mainSizer );
  Layout();
}

void MainFrame::setupRightPanel( wxPanel* panel )
{
  wxFlexGridSizer* sizer = new wxFlexGridSizer( 2, 1, 0, 0 );
  sizer->AddGrowableRow( 1 );

  int lflags = wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL;
  int rflags = wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxEXPAND | wxALL;
  
  // setup the top part (information box)
  wxFlexGridSizer* topSizer = new wxFlexGridSizer( 6, 2, 0, 0 );
  numberOfStatesLabel = new wxStaticText( panel, wxID_ANY, wxEmptyString,
      wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
  numberOfTransitionsLabel = new wxStaticText( panel, wxID_ANY, wxEmptyString,
      wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
  numberOfClustersLabel = new wxStaticText( panel, wxID_ANY, wxEmptyString,
      wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
  numberOfRanksLabel = new wxStaticText( panel, wxID_ANY, wxEmptyString,
      wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
  numberOfMarkedStatesLabel = new wxStaticText( panel, wxID_ANY, wxEmptyString,
      wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
  numberOfMarkedTransitionsLabel = new wxStaticText( panel, wxID_ANY, wxEmptyString,
      wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
  
  topSizer->AddGrowableCol( 1 );
  topSizer->Add( new wxStaticText( panel, wxID_ANY, wxT("Number of states:") ),
      0, lflags, 3 );
  topSizer->Add( numberOfStatesLabel, 0, rflags, 3 );
  topSizer->Add( new wxStaticText( panel, wxID_ANY,
	wxT("Number of transitions:") ), 0, lflags, 3 );
  topSizer->Add( numberOfTransitionsLabel, 0, rflags, 3 );
  topSizer->Add( new wxStaticText( panel, wxID_ANY, wxT("Number of clusters:")
	), 0, lflags, 3 );
  topSizer->Add( numberOfClustersLabel, 0, rflags, 3 );
  topSizer->Add( new wxStaticText( panel, wxID_ANY, wxT("Number of ranks:") ),
      0, lflags, 3 );
  topSizer->Add( numberOfRanksLabel, 0, rflags, 3 );
  topSizer->Add( new wxStaticText( panel, wxID_ANY,
	wxT("Number of marked states:") ), 0, rflags, 3 );
  topSizer->Add( numberOfMarkedStatesLabel, 0, rflags, 3 );
  topSizer->Add( new wxStaticText( panel, wxID_ANY,
	wxT("Number of marked transitions:") ), 0, rflags, 3 );
  topSizer->Add( numberOfMarkedTransitionsLabel, 0, rflags, 3 );

  // setup the bottom part (notebook)
  wxNotebook* bottomNotebook = new wxNotebook( panel, wxID_ANY );
  wxPanel* settingsPanel = new wxPanel( bottomNotebook, wxID_ANY );
  wxPanel* markPanel = new wxPanel( bottomNotebook, wxID_ANY );
  
  setupSettingsPanel( settingsPanel );
  setupMarkPanel( markPanel );
  
  bottomNotebook->AddPage( settingsPanel, wxT("Settings"), true );
  bottomNotebook->AddPage( markPanel, wxT("Mark"), false );
  
  sizer->Add( topSizer, 0, wxEXPAND | wxALL, 5 );
  sizer->Add( bottomNotebook, 0, wxEXPAND | wxALL, 5 );
  sizer->Fit( panel );
  panel->SetSizer( sizer );
  panel->Layout();
}

void MainFrame::setupSettingsPanel( wxPanel* panel )
{
  wxStaticBoxSizer* parSizer = new wxStaticBoxSizer( wxVERTICAL, panel,
      wxT("Parameters") );

  int lflags = wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL; 
  int rflags = wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL; 
  int border = 3;

  wxFlexGridSizer* styleSizer = new wxFlexGridSizer( 2, 2, 0, 0 );
  styleSizer->AddGrowableCol( 0 );
  styleSizer->AddGrowableCol( 1 );
  styleSizer->Add( new wxStaticText( panel, wxID_ANY, wxT("Rank style:") ),
      1, lflags, border );
  wxString rs_choices[2] = { wxT("Iterative"), wxT("Cyclic") };
  wxChoice* rankstyleChoice = new wxChoice( panel, myID_RANK_STYLE,
      wxDefaultPosition, wxDefaultSize, 2, rs_choices );
  rankstyleChoice->SetSelection( 0 );
  styleSizer->Add( rankstyleChoice, 1, lflags, border );
  styleSizer->Add( new wxStaticText( panel, wxID_ANY,
	wxT("Visualisation style:") ), 1, lflags, border );
  wxString vs_choices[3] = { wxT("Cones"), wxT("Organic"), wxT("Atomium") };
  wxChoice* visstyleChoice = new wxChoice( panel, myID_VIS_STYLE,
      wxDefaultPosition, wxDefaultSize, 3, vs_choices );
  visstyleChoice->SetSelection( 0 );
  styleSizer->Add( visstyleChoice, 1, lflags, border );
  
  wxFlexGridSizer* parsubSizer = new wxFlexGridSizer( 7, 2, 0, 0 );
  parsubSizer->AddGrowableCol( 0 );
  for ( int i = 0 ; i < 5 ; ++i )
  {
    parsubSizer->AddGrowableRow( i );
  }
  parSizer->Add( parsubSizer, 1, wxEXPAND | wxALL, 0 );
  
  wxSize spinctrlSize( 50, -1 );

  nodesizeSpinCtrl = new wxSpinCtrlFloat( panel, myID_SETTINGS_CONTROL,
      0.0f, 1000.0f, 0.1f, 0.0f, wxDefaultPosition, spinctrlSize );
  nodesizeSpinCtrl->SetSizeHints( spinctrlSize, spinctrlSize );
  parsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("Node size:") ), 0, lflags, border );
  parsubSizer->Add( nodesizeSpinCtrl, 0, rflags, border );
  
  branchrotationSpinCtrl = new wxSpinCtrl( panel, myID_SETTINGS_CONTROL,
      wxEmptyString, wxDefaultPosition );
  branchrotationSpinCtrl->SetRange( 0, 360 );
  branchrotationSpinCtrl->SetSizeHints( spinctrlSize, spinctrlSize );
  parsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("Branch rotation:") ), 0, lflags, border );
  parsubSizer->Add( branchrotationSpinCtrl, 0, rflags, border );
  
  /*clusterheightSpinCtrl = new wxSpinCtrlFloat( panel, myID_SETTINGS_CONTROL,
      0.0f, 1000.0f, 0.1f, 0.0f, wxDefaultPosition, spinctrlSize  );
  clusterheightSpinCtrl->SetSizeHints( spinctrlSize, spinctrlSize );
  parsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("Cluster height:") ), 0, flags, border );
  parsubSizer->Add( clusterheightSpinCtrl, 0, flags, border );
  parsubSizer->AddSpacer( 0 );
  */
  /*branchscaleSpinCtrl = new wxSpinCtrl( panel, myID_SETTINGS_CONTROL );
  parsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("Branch scale:") ), 0, flags, border );
  parsubSizer->Add( branchscaleSpinCtrl, 0, flags, border );
  parsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("%") ), 0, flags, 0 );
  */
  /*branchspreadSpinCtrl = new wxSpinCtrl( panel, myID_SETTINGS_CONTROL );
  branchspreadSpinCtrl->SetMinSize( spinctrlSize );
  branchspreadSpinCtrl->SetMaxSize( spinctrlSize );
  parsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("Branch spread:") ), 0, flags, border );
  parsubSizer->Add( branchspreadSpinCtrl, 0, flags, border );
  parsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("%") ), 0, flags, 0 );
  */
  /*
  parsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("Branch rotation:") ), 0, flags, border );
  branchrotationSpinCtrl = new wxSpinCtrl( panel, myID_SETTINGS_CONTROL );
  branchrotationSpinCtrl->SetRange( 0, 360 );
  parsubSizer->Add( branchrotationSpinCtrl, 0, flags, border );
  parsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("\302\272") ), 0, flags, 0 );
  */
  innerbranchtiltSpinCtrl = new wxSpinCtrl( panel, myID_SETTINGS_CONTROL,
      wxEmptyString, wxDefaultPosition );
  innerbranchtiltSpinCtrl->SetRange( 0, 90 );
  innerbranchtiltSpinCtrl->SetMinSize( spinctrlSize );
  innerbranchtiltSpinCtrl->SetMaxSize( spinctrlSize );
  parsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("Inner branch tilt:") ), 0, lflags, border );
  parsubSizer->Add( innerbranchtiltSpinCtrl, 0, rflags, border );

  outerbranchtiltSpinCtrl = new wxSpinCtrl( panel, myID_SETTINGS_CONTROL,
      wxEmptyString, wxDefaultPosition );
  outerbranchtiltSpinCtrl->SetRange( 0, 90 );
  outerbranchtiltSpinCtrl->SetMinSize( spinctrlSize );
  outerbranchtiltSpinCtrl->SetMaxSize( spinctrlSize );
  parsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("Outer branch tilt:") ), 0, lflags, border );
  parsubSizer->Add( outerbranchtiltSpinCtrl, 0, rflags, border );
  
  qualitySpinCtrl = new wxSpinCtrl( panel, myID_SETTINGS_CONTROL, wxEmptyString,
      wxDefaultPosition );
  qualitySpinCtrl->SetRange(2,50);
  qualitySpinCtrl->SetMinSize( spinctrlSize );
  qualitySpinCtrl->SetMaxSize( spinctrlSize );
  parsubSizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Accuracy:")),0,lflags,
                   border);
  parsubSizer->Add( qualitySpinCtrl, 0, rflags, border );
  
  ellipsoidSpinCtrl = new wxSpinCtrlFloat( panel, myID_SETTINGS_CONTROL,
      0.1f, 10.0f, 0.1f, 0.0f, wxDefaultPosition, spinctrlSize );
  ellipsoidSpinCtrl->SetSizeHints( spinctrlSize, spinctrlSize );
  parsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("Ellipsoid threshold:") ), 0, lflags, border );
  parsubSizer->Add( ellipsoidSpinCtrl, 0, rflags, border );
  /*
  qualitySlider = new wxSlider( panel, myID_SETTINGS_CONTROL, 4, 4, 100,
      wxDefaultPosition, wxSize( 100, -1 ), wxSL_HORIZONTAL | wxSL_LABELS |
      wxSL_AUTOTICKS );
  parsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("Rendering quality:") ), 0, flags, border );
  parsubSizer->Add( qualitySlider, 0, flags, border );
  parsubSizer->AddSpacer( 0 );
  */
  
  /*levelDivCheckBox = new wxCheckBox( panel, myID_SETTINGS_CONTROL,
	wxT("Show level dividers") );
  parsubSizer->Add( levelDivCheckBox, 0, flags, border );
  */

  // Setup the Colors panel
  
  wxStaticBoxSizer* colSizer = new wxStaticBoxSizer( wxVERTICAL, panel, wxT("Colours") );
  
  wxFlexGridSizer* colsubSizer = new wxFlexGridSizer( 8, 4, 0, 0 );
  colsubSizer->AddGrowableCol( 0 );
  for ( int i = 0 ; i < 8 ; ++i )
  {
    colsubSizer->AddGrowableRow( i );
  }
  colSizer->Add( colsubSizer, 1, wxEXPAND | wxALL, 0 );
  
  transparencySpinCtrl = new wxSpinCtrl( panel, myID_SETTINGS_CONTROL );
  transparencySpinCtrl->SetSizeHints( spinctrlSize, spinctrlSize );
  colsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("Transparency:") ), 0, lflags, border );
  colsubSizer->AddSpacer( 0 );
  colsubSizer->Add( transparencySpinCtrl, 0, rflags, border );
  colsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("%") ), 0, lflags, 0 );

  wxSize btnSize( 25, 25 );

  backgroundButton = new wxColorButton( panel, this, myID_COLOR_BUTTON );
  backgroundButton->SetSizeHints( btnSize, btnSize );
  colsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("Background:") ), 0, lflags, border );
  colsubSizer->AddSpacer( 0 );
  colsubSizer->Add( backgroundButton, 0, rflags, border );
  colsubSizer->AddSpacer( 0 );

  nodeButton = new wxColorButton( panel, this, myID_COLOR_BUTTON );
  nodeButton->SetSizeHints( btnSize, btnSize );
  colsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("Node:") ), 0, lflags, border );
  colsubSizer->AddSpacer( 0 );
  colsubSizer->Add( nodeButton, 0, rflags, border );
  colsubSizer->AddSpacer( 0 );

  downEdgeButton = new wxColorButton( panel, this, myID_COLOR_BUTTON );
  downEdgeButton->SetSizeHints( btnSize, btnSize );
  colsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("Edge (down/up):") ), 0, lflags, border );
  colsubSizer->Add( downEdgeButton, 0, rflags, border );
  //colsubSizer->AddSpacer( 0 );

  upEdgeButton = new wxColorButton( panel, this, myID_COLOR_BUTTON );
  upEdgeButton->SetSizeHints( btnSize, btnSize );
  //colsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
  //	  wxT("Edge (up):") ), 0, flags, border );
  colsubSizer->Add( upEdgeButton, 0, rflags, border );
  colsubSizer->AddSpacer( 0 );

  markButton = new wxColorButton( panel, this, myID_COLOR_BUTTON );
  markButton->SetSizeHints( btnSize, btnSize );
  colsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("Mark:") ), 0, lflags, border );
  colsubSizer->AddSpacer( 0 );
  colsubSizer->Add( markButton, 0, rflags, border );
  colsubSizer->AddSpacer( 0 );

  interpolate1Button = new wxColorButton( panel, this, myID_COLOR_BUTTON );
  interpolate1Button->SetSizeHints( btnSize, btnSize );
  interpolate2Button = new wxColorButton( panel, this, myID_COLOR_BUTTON );
  interpolate2Button->SetSizeHints( btnSize, btnSize );
  colsubSizer->Add( new wxStaticText( panel, wxID_ANY, 
	wxT("Interpolate:") ), 0, lflags, border );
  colsubSizer->Add( interpolate1Button, 0, rflags, border );
  colsubSizer->Add( interpolate2Button, 0, rflags, border );
  colsubSizer->AddSpacer( 0 );
  
  longinterpolateCheckBox = new wxCheckBox( panel, myID_SETTINGS_CONTROL,
      wxT("Long interpolation") );
  colsubSizer->Add( longinterpolateCheckBox, 0, lflags, border );

  wxFlexGridSizer* panelSizer = new wxFlexGridSizer( 4, 1, 0, 0 );
  panelSizer->AddGrowableCol( 0 );
  panelSizer->AddGrowableRow( 0 );
  panelSizer->AddGrowableRow( 1 );
  //panelSizer->AddGrowableRow( 2 );
  //panelSizer->AddGrowableRow( 3 );
  panelSizer->Add( styleSizer, 0, wxEXPAND | wxALL, border );
  panelSizer->Add( parSizer, 1, wxEXPAND | wxALL, 5 );
  panelSizer->Add( colSizer, 1, wxEXPAND | wxALL, 5 );

  panelSizer->Add( new wxButton( panel, wxID_RESET, wxT("Default settings")
	), 0, lflags, border );
  panelSizer->Fit( panel );
  panel->SetSizer( panelSizer );
  panel->Layout();
}

void MainFrame::setupMarkPanel( wxPanel* panel )
{
  wxFlexGridSizer* markSizer = new wxFlexGridSizer( 6, 1, 0, 0 );
  markSizer->AddGrowableCol( 0 );
  markSizer->AddGrowableRow( 4 );
  markSizer->AddGrowableRow( 5 );

  int flags = wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL;
  int border = 3;
  
  nomarksRadio = new wxRadioButton( panel, myID_MARK_RADIOBUTTON,
      wxT("No marks"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
  nomarksRadio->SetValue( true );
  markDeadlocksRadio = new wxRadioButton( panel, myID_MARK_RADIOBUTTON,
      wxT("Mark deadlocks"));
  markStatesRadio = new wxRadioButton( panel, myID_MARK_RADIOBUTTON,
      wxT("Mark states") );
  markTransitionsRadio = new wxRadioButton( panel, myID_MARK_RADIOBUTTON,
      wxT("Mark transitions") );
    
  markSizer->Add( nomarksRadio, 0, flags, border );
  markSizer->Add( markDeadlocksRadio, 0, flags, border );
  markSizer->Add( markStatesRadio, 0, flags, border );
  markSizer->Add( markTransitionsRadio, 0, flags, border );
  
  wxStaticBoxSizer* markStatesSizer = new wxStaticBoxSizer( wxVERTICAL, panel,
      wxT("Mark states") );
  wxString choices[2] = { wxT("Match any of the following"),
      wxT("Match all of the following") };
  markAnyAllChoice = new wxChoice( panel, myID_MARK_ANYALL,
      wxDefaultPosition, wxDefaultSize, 2, choices );
  markAnyAllChoice->SetSelection( 0 );
  markStatesSizer->Add( markAnyAllChoice, 0, flags, border );
  
  markStatesListBox = new wxCheckListBox( panel, myID_MARK_RULES, wxDefaultPosition,
      wxDefaultSize, 0, NULL, wxLB_SINGLE | wxLB_NEEDED_SB | wxLB_HSCROLL );
  markStatesSizer->Add( markStatesListBox, 1, flags | wxEXPAND, border );
  wxBoxSizer* addremoveSizer = new wxBoxSizer( wxHORIZONTAL );
  addremoveSizer->Add( new wxButton( panel, myID_ADD_RULE, wxT("Add") ), 0,
      flags, border );
  addremoveSizer->Add( new wxButton( panel, myID_REMOVE_RULE, wxT("Remove") ),
      0, flags, border );
  markStatesSizer->Add( addremoveSizer, 0, flags, border );
  
  wxStaticBoxSizer* markTransitionsSizer = new wxStaticBoxSizer( wxVERTICAL,
      panel, wxT("Mark transitions") );
  markTransitionsListBox = new wxCheckListBox( panel, myID_MARK_TRANSITIONS,
      wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE | wxLB_SORT |
      wxLB_NEEDED_SB | wxLB_HSCROLL );
  markTransitionsSizer->Add( markTransitionsListBox, 1, flags | wxEXPAND, border );
  
  markSizer->Add( markStatesSizer, 0, wxEXPAND | wxALL, border );
  markSizer->Add( markTransitionsSizer, 0, wxEXPAND | wxALL, border );
  markSizer->Fit( panel );
  panel->SetSizer( markSizer );
  panel->Layout();
}

GLCanvas* MainFrame::getGLCanvas() const
{
  return glCanvas;
}

void MainFrame::setFileInfo( wxFileName fn )
{
  directory = fn.GetPath();
  filename  = fn.GetFullName();
}

void MainFrame::onOpen( wxCommandEvent& /*event*/ )
{
  wxString filemask = wxT("FSM files (*.fsm)|*.fsm");
  wxFileDialog* dialog = new wxFileDialog( this, wxT("Open LTS"), directory,
	filename, filemask, wxOPEN );
  dialog->CentreOnParent();
  if ( dialog->ShowModal() == wxID_OK )
  {
    directory = dialog->GetDirectory();
    filename  = dialog->GetFilename();
    mediator->openFile( string(dialog->GetPath().fn_str()) );
  }
  dialog->Close();
  dialog->Destroy();
}

void MainFrame::onExit( wxCommandEvent& /*event*/ )
{
  Close();
}

void MainFrame::onActivateTool( wxCommandEvent& event )
{
  glCanvas->setActiveTool( event.GetId() );
}

void MainFrame::onRankStyle( wxCommandEvent& event )
{
  if ( event.GetSelection() == 0 )
    mediator->setRankStyle( ITERATIVE );
  else
    mediator->setRankStyle( CYCLIC );
}

void MainFrame::onVisStyle( wxCommandEvent& event )
{
  if ( event.GetSelection() == 0 )
    mediator->setVisStyle( CONES );
  else if ( event.GetSelection() == 1 )
    mediator->setVisStyle( ORGANIC );
  else
    mediator->setVisStyle( ATOMIUM );
}

void MainFrame::onResetView( wxCommandEvent& /*event*/ )
{
  glCanvas->resetView();
}

void MainFrame::onDisplayStates( wxCommandEvent& /*event*/ )
{
  mediator->toggleDisplayStates();
}

void MainFrame::onDisplayWireframe( wxCommandEvent& /*event*/ )
{
  mediator->toggleDisplayWireframe();
}

void MainFrame::onColorButton( wxCommandEvent& /*event*/ )
{
  mediator->applySettings();
}

void MainFrame::onSpinSettingChanged( wxSpinEvent& /*event*/ )
{
  mediator->applySettings();
}

void MainFrame::onCommandSettingChanged( wxCommandEvent& /*event*/ )
{
  mediator->applySettings();
}

void MainFrame::onResetButton( wxCommandEvent& /*event*/ )
{
  mediator->applyDefaultSettings();
}

void MainFrame::onMarkRadio( wxCommandEvent& event )
{
  wxRadioButton* buttonClicked = (wxRadioButton*)event.GetEventObject();

  if ( buttonClicked == nomarksRadio )
    mediator->applyMarkStyle( NO_MARKS );
  else if ( buttonClicked == markDeadlocksRadio )
    mediator->applyMarkStyle( MARK_DEADLOCKS );
  else if ( buttonClicked == markStatesRadio )
    mediator->applyMarkStyle( MARK_STATES );
  else if ( buttonClicked == markTransitionsRadio )
    mediator->applyMarkStyle( MARK_TRANSITIONS );
}

void MainFrame::onMarkRuleActivate( wxCommandEvent& event )
{
  int i = event.GetInt();
  mediator->activateMarkRule( i, markStatesListBox->IsChecked( i ) );
  markStatesRadio->SetValue( true );
}

void MainFrame::onMarkRuleEdit( wxCommandEvent& event )
{
  mediator->editMarkRule( event.GetSelection() );
}

void MainFrame::onMarkAnyAll( wxCommandEvent& event )
{
  mediator->setMatchAnyMarkRule( event.GetSelection() == 0 );
  markStatesRadio->SetValue( true );
}

void MainFrame::onAddMarkRuleButton( wxCommandEvent& /*event*/ )
{
  mediator->addMarkRule();
}

void MainFrame::onRemoveMarkRuleButton( wxCommandEvent& /*event*/ )
{
  int sel_index = markStatesListBox->GetSelection();
  if ( sel_index != wxNOT_FOUND )
  {
    markStatesListBox->Delete( sel_index );
    mediator->removeMarkRule( sel_index );
    markStatesRadio->SetValue( true );
  }
}

void MainFrame::onMarkTransition( wxCommandEvent& event )
{
  int i = event.GetInt();
  if ( markTransitionsListBox->IsChecked( i ) )
    mediator->markAction( string( markTransitionsListBox->GetString( i
	    ).fn_str() ) );
  else
    mediator->unmarkAction( string( markTransitionsListBox->GetString( i
	    ).fn_str() ) );
  markTransitionsRadio->SetValue( true );
}

VisSettings MainFrame::getVisSettings() const
{
  RGB_Color downC = wxC_to_RGB( downEdgeButton->GetBackgroundColour() );
  RGB_Color intC1 = wxC_to_RGB( interpolate1Button->GetBackgroundColour() );
  RGB_Color intC2 = wxC_to_RGB( interpolate2Button->GetBackgroundColour() );
  RGB_Color markC = wxC_to_RGB( markButton->GetBackgroundColour() );
  RGB_Color stateC = wxC_to_RGB( nodeButton->GetBackgroundColour() );
  RGB_Color upC = wxC_to_RGB( upEdgeButton->GetBackgroundColour() );
  VisSettings result =
  {
    (100-transparencySpinCtrl->GetValue()) / 100.0f,
//    backpointerSpinCtrl->GetValue(),
    branchrotationSpinCtrl->GetValue(),
//    branchscaleSpinCtrl->GetValue(),
//    branchspreadSpinCtrl->GetValue(),
//    clusterheightSpinCtrl->GetValue(),
    downC,
    ellipsoidSpinCtrl->GetValue(),
    innerbranchtiltSpinCtrl->GetValue(),
    intC1,
    intC2,
//    levelDivCheckBox->GetValue(),
    longinterpolateCheckBox->GetValue(),
    markC,
    nodesizeSpinCtrl->GetValue(),
    outerbranchtiltSpinCtrl->GetValue(),
    qualitySpinCtrl->GetValue()*2,
    stateC,
    //0,
    upC
  };
  return result;
}

RGB_Color MainFrame::getBackgroundColor() const
{
  return wxC_to_RGB( backgroundButton->GetBackgroundColour() );
}

void MainFrame::setBackgroundColor( RGB_Color c )
{
  backgroundButton->SetBackgroundColour( RGB_to_wxC( c ) );
}

void MainFrame::setVisSettings( VisSettings ss )
{
  nodeButton->SetBackgroundColour( RGB_to_wxC( ss.stateColor ) );
  downEdgeButton->SetBackgroundColour( RGB_to_wxC( ss.downEdgeColor ) );
  ellipsoidSpinCtrl->SetValue( ss.ellipsoidThreshold );
  upEdgeButton->SetBackgroundColour( RGB_to_wxC( ss.upEdgeColor ) );
  markButton->SetBackgroundColour( RGB_to_wxC( ss.markedColor ) );
  innerbranchtiltSpinCtrl->SetValue( ss.innerBranchTilt );
  interpolate1Button->SetBackgroundColour( RGB_to_wxC( ss.interpolateColor1 ));
  interpolate2Button->SetBackgroundColour( RGB_to_wxC( ss.interpolateColor2 ));
  longinterpolateCheckBox->SetValue( ss.longInterpolation );
  transparencySpinCtrl->SetValue( 100 - (int)(ss.alpha * 100) );
  nodesizeSpinCtrl->SetValue( ss.nodeSize );
//  backpointerSpinCtrl->SetValue( ss.backpointerCurve );
//  clusterheightSpinCtrl->SetValue( ss.clusterHeight );
//  branchspreadSpinCtrl->SetValue( ss.branchSpread );
  outerbranchtiltSpinCtrl->SetValue( ss.outerBranchTilt );
  qualitySpinCtrl->SetValue( ss.quality/2 );
//  levelDivCheckBox->SetValue( ss.levelDividers );
  //branchscaleSpinCtrl->SetValue( ss.branchScale );
  branchrotationSpinCtrl->SetValue( ss.branchRotation );
}

void MainFrame::createProgressDialog( const string title, const string text )
{
  progDialog = new wxProgressDialog( wxString( title.c_str(), wxConvUTF8 ),
      wxString( text.c_str(), wxConvUTF8 ), 100, this, wxPD_APP_MODAL |
      wxPD_AUTO_HIDE | wxPD_ELAPSED_TIME );
  progDialog->SetSize( wxSize( 400, 100 ) );
  progDialog->CentreOnParent();
}

void MainFrame::updateProgressDialog( int val, string msg )
{
  if ( progDialog != NULL )
  {
    progDialog->Update( val, wxString( msg.c_str(), wxConvUTF8 ) );
    if ( val == 100 )
    {
      progDialog->Close();
      progDialog->Destroy();
      progDialog = NULL;
    }
  }
}

void MainFrame::showMessage( string title, string text )
{
  wxMessageDialog* msgDialog = new wxMessageDialog( this, wxString(
	text.c_str(), wxConvUTF8 ), wxString( title.c_str(), wxConvUTF8 ), wxOK );
  msgDialog->ShowModal();
  msgDialog->Close();
  msgDialog->Destroy();
}

void MainFrame::loadTitle()
{
  if ( filename != wxEmptyString )
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

void MainFrame::setMarkedStatesInfo( int number )
{
  numberOfMarkedStatesLabel->SetLabel( wxString::Format( wxT("%d"), number ) );
}

void MainFrame::setMarkedTransitionsInfo( int number )
{
  numberOfMarkedTransitionsLabel->SetLabel( wxString::Format( wxT("%d"), number ) );
}

void MainFrame::addMarkRule( wxString str )
{
  markStatesListBox->Append( str );
  markStatesListBox->Check( markStatesListBox->GetCount()-1, true );
  markStatesRadio->SetValue( true );
}

void MainFrame::replaceMarkRule( int index, wxString str )
{
  bool isChecked = markStatesListBox->IsChecked( index );
  markStatesListBox->SetString( index, str );
  markStatesListBox->Check( index, isChecked );
  markStatesRadio->SetValue( true );
}
  
void MainFrame::resetMarkRules()
{
  markStatesListBox->Clear();
  markAnyAllChoice->SetSelection( 0 );
  nomarksRadio->SetValue( true );
}

void MainFrame::setActionLabels( vector< ATerm > &labels )
{
  wxArrayString strLabels;
  strLabels.Alloc( labels.size() );
  for ( vector< ATerm >::iterator it = labels.begin() ; it != labels.end() ;
	++it )
  {
    strLabels.Add( wxString( ATwriteToString( *it ), wxConvLocal ) );
  }
  strLabels.Sort();
  markTransitionsListBox->Set( strLabels );
}

void MainFrame::startRendering()
{
  SetStatusText( wxT("Rendering...") );
  GetStatusBar()->Update();
}

void MainFrame::stopRendering()
{
  SetStatusText( wxT("") );
  GetStatusBar()->Update();
}

wxColour MainFrame::RGB_to_wxC( RGB_Color c ) const
{
  unsigned char r = static_cast<unsigned char>( round_to_int( c.r * 255.0f ) );
  unsigned char g = static_cast<unsigned char>( round_to_int( c.g * 255.0f ) );
  unsigned char b = static_cast<unsigned char>( round_to_int( c.b * 255.0f ) );
  return wxColour( r, g, b );
}

RGB_Color MainFrame::wxC_to_RGB( wxColour c ) const
{
  RGB_Color result = 
  { c.Red() / 255.0f, c.Green() / 255.0f, c.Blue() / 255.0f };
  return result;
}

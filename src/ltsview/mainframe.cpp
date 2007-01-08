#include "mainframe.h"
#include <wx/bitmap.h>
#include <wx/event.h>
#include <wx/filedlg.h>
#include <wx/notebook.h>
#include <wx/sysopt.h>
//#include <time.h>
#include "aterm2.h"
#include "ids.h"
#include "savepicdialog.h"
#include "icons/main_window.xpm"

// For compatibility with older wxWidgets versions (pre 2.8)
#if (wxMINOR_VERSION < 8)
# define wxFD_OPEN wxOPEN
#endif

using namespace std;
using namespace Utils;
using namespace IDs;
// Event table
BEGIN_EVENT_TABLE(MainFrame, wxFrame)
  EVT_TOOL  (wxID_OPEN, MainFrame::onOpen)
  EVT_MENU  (wxID_OPEN, MainFrame::onOpen)
  EVT_MENU  (myID_SAVEPIC, MainFrame::onSavePic)
  EVT_MENU  (wxID_EXIT, MainFrame::onExit)
  EVT_MENU  (wxID_RESET, MainFrame::onResetView)
  EVT_MENU  (myID_DISPLAY_STATES,MainFrame::onDisplay)
  EVT_MENU  (myID_DISPLAY_TRANSITIONS,MainFrame::onDisplay)
  EVT_MENU  (myID_DISPLAY_BACKPOINTERS,MainFrame::onDisplay)
  EVT_MENU  (myID_DISPLAY_WIREFRAME,MainFrame::onDisplay)
  EVT_MENU  (myID_PAN, MainFrame::onActivateTool)
  EVT_MENU  (myID_ROTATE, MainFrame::onActivateTool)
  EVT_MENU  (myID_SELECT, MainFrame::onActivateTool)
  EVT_MENU  (myID_ZOOM, MainFrame::onActivateTool)
  EVT_MENU  (wxID_ABOUT, MainFrame::onAbout)
  EVT_CHOICE(myID_RANK_STYLE, MainFrame::onRankStyle)
  EVT_CHOICE(myID_VIS_STYLE, MainFrame::onVisStyle)
  EVT_BUTTON(myID_COLOR_BUTTON, MainFrame::onColorButton)
  EVT_SPINCTRL(myID_SETTINGS_CONTROL, MainFrame::onSpinSettingChanged)
  EVT_CHECKBOX(myID_SETTINGS_CONTROL, MainFrame::onCommandSettingChanged)
  EVT_BUTTON(wxID_RESET, MainFrame::onResetButton)
  EVT_RADIOBUTTON(myID_MARK_RADIOBUTTON, MainFrame::onMarkRadio)
  EVT_CHOICE(myID_MARK_ANYALL, MainFrame::onMarkAnyAll)
  EVT_LISTBOX_DCLICK(myID_MARK_RULES, MainFrame::onMarkRuleEdit)
  EVT_CHECKLISTBOX(myID_MARK_RULES, MainFrame::onMarkRuleActivate)
  EVT_CHECKLISTBOX(myID_MARK_TRANSITIONS, MainFrame::onMarkTransition)
  EVT_BUTTON(myID_ADD_RULE, MainFrame::onAddMarkRuleButton)
  EVT_BUTTON(myID_REMOVE_RULE, MainFrame::onRemoveMarkRuleButton)
//  EVT_IDLE(MainFrame::onIdle)
END_EVENT_TABLE()

MainFrame::MainFrame(Mediator* owner) : wxFrame(NULL,wxID_ANY,wxT("LTSView")) {
//  previousTime = 0.0;
//  frameCount = 0;
  mediator = owner;
  progDialog = NULL;

  SetIcon(wxIcon(main_window));

  CreateStatusBar(2);
  setupMenuBar();
  setupMainArea();
  
  SetSize(800,600);
  CentreOnScreen();
}

void MainFrame::setupMenuBar() {
  // Set up the menu bar
  wxMenuBar* menuBar = new wxMenuBar;
  wxMenu* fileMenu = new wxMenu;
  wxMenu* viewMenu = new wxMenu;
  wxMenu* toolMenu = new wxMenu;
  wxMenu* helpMenu = new wxMenu;
  
  fileMenu->Append(wxID_OPEN,wxT("&Open...\tCtrl+O"),
    wxT("Load an LTS from file"));
  fileMenu->Append(myID_SAVEPIC,wxT("Save &Picture...\tCtrl+P"),
      wxT("Save current picture to file"));
  fileMenu->AppendSeparator();
  fileMenu->Append(wxID_EXIT, wxT("E&xit\tCtrl+Q"), wxT("Exit application"));
    
  viewMenu->Append(wxID_RESET, wxT("&Reset viewpoint\tF2"),
      wxT("Set the viewpoint to the default position"));
  viewMenu->AppendSeparator();
  viewMenu->AppendCheckItem(myID_DISPLAY_STATES, wxT("Display &states\tF3"),
      wxT("Show/hide individual states"));
  viewMenu->AppendCheckItem(myID_DISPLAY_TRANSITIONS, 
    wxT("Display &transitions\tF4"), wxT("Show/hide individual transitions"));
  viewMenu->AppendCheckItem(myID_DISPLAY_BACKPOINTERS,
    wxT("Display &backpointers\tF5"), wxT("Show/hide backpointers"));
  viewMenu->AppendCheckItem(myID_DISPLAY_WIREFRAME,
      wxT("Display &wireframe\tF6"),wxT("Toggle wireframe/surface"));

  toolMenu->AppendRadioItem(myID_SELECT,wxT("&Select\tS"),wxT("Select tool"));
  toolMenu->AppendRadioItem(myID_PAN,wxT("&Pan\tP"),wxT("Pan tool"));
  toolMenu->AppendRadioItem(myID_ZOOM,wxT("&Zoom\tZ"),wxT("Zoom tool"));
  toolMenu->AppendRadioItem(myID_ROTATE,wxT("&Rotate\tR"),wxT("Rotate tool"));

  helpMenu->Append(wxID_ABOUT,wxT("&About"));
  
  menuBar->Append(fileMenu, wxT("&File"));
  menuBar->Append(viewMenu, wxT("&View"));
  menuBar->Append(toolMenu, wxT("&Tools"));
  menuBar->Append(helpMenu, wxT("&Help"));
  
  SetMenuBar(menuBar);
}

void MainFrame::setupMainArea() {
  wxFlexGridSizer* mainSizer = new wxFlexGridSizer(1,2,0,0);
  mainSizer->AddGrowableCol(0);
  mainSizer->AddGrowableRow(0);

  wxPanel* rightPanel = new wxPanel(this,wxID_ANY,wxDefaultPosition,
      wxDefaultSize,wxRAISED_BORDER);
  setupRightPanel(rightPanel);

  int w,h;
  rightPanel->GetSize(&w,&h);
  int attribList[] = { WX_GL_RGBA,WX_GL_DOUBLEBUFFER };
  glCanvas = new GLCanvas(mediator,this,wxDefaultSize,attribList);
  
  mainSizer->Add(glCanvas,1,wxALIGN_CENTER|wxEXPAND|wxALL,0);
  mainSizer->Add(rightPanel,1,wxALIGN_CENTER|wxEXPAND|wxALL,0);
  
  mainSizer->Fit(this);
  SetSizer(mainSizer);
  Layout();
}

void MainFrame::setupRightPanel(wxPanel* panel) {
  wxFlexGridSizer* sizer = new wxFlexGridSizer(2,1,0,0);
  sizer->AddGrowableCol(0);
  sizer->AddGrowableRow(1);

  int lflags = wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL;
  int rflags = wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxEXPAND | wxALL;
  
  // setup the top part (information box)
  wxFlexGridSizer* topSizer = new wxFlexGridSizer(6, 2, 0, 0);
  numberOfStatesLabel = new wxStaticText(panel, wxID_ANY, wxEmptyString,
      wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
  numberOfTransitionsLabel = new wxStaticText(panel, wxID_ANY, wxEmptyString,
      wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
  numberOfClustersLabel = new wxStaticText(panel, wxID_ANY, wxEmptyString,
      wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
  numberOfRanksLabel = new wxStaticText(panel, wxID_ANY, wxEmptyString,
      wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
  numberOfMarkedStatesLabel = new wxStaticText(panel, wxID_ANY, wxEmptyString,
      wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
  numberOfMarkedTransitionsLabel = new wxStaticText(panel, wxID_ANY, wxEmptyString,
      wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
  
  topSizer->AddGrowableCol(1);
  topSizer->Add(new wxStaticText(panel, wxID_ANY, wxT("Number of states:")),
      0, lflags, 3);
  topSizer->Add(numberOfStatesLabel, 0, rflags, 3);
  topSizer->Add(new wxStaticText(panel, wxID_ANY,
	wxT("Number of transitions:")), 0, lflags, 3);
  topSizer->Add(numberOfTransitionsLabel, 0, rflags, 3);
  topSizer->Add(new wxStaticText(panel, wxID_ANY, wxT("Number of clusters:")
	), 0, lflags, 3);
  topSizer->Add(numberOfClustersLabel, 0, rflags, 3);
  topSizer->Add(new wxStaticText(panel, wxID_ANY, wxT("Number of ranks:")),
      0, lflags, 3);
  topSizer->Add(numberOfRanksLabel, 0, rflags, 3);
  topSizer->Add(new wxStaticText(panel, wxID_ANY,
	wxT("Number of marked states:")), 0, rflags, 3);
  topSizer->Add(numberOfMarkedStatesLabel, 0, rflags, 3);
  topSizer->Add(new wxStaticText(panel, wxID_ANY,
	wxT("Number of marked transitions:")), 0, rflags, 3);
  topSizer->Add(numberOfMarkedTransitionsLabel, 0, rflags, 3);

  // setup the bottom part (notebook)
  wxNotebook* bottomNotebook = new wxNotebook(panel, wxID_ANY);
  wxScrolledWindow* settingsPanel = new wxScrolledWindow(bottomNotebook,
      wxID_ANY);
  wxScrolledWindow* markPanel = new wxScrolledWindow(bottomNotebook,wxID_ANY);
  
  setupSettingsPanel(settingsPanel);
  setupMarkPanel(markPanel);
  
  settingsPanel->SetScrollRate(10,10);
  markPanel->SetScrollRate(10,10);
  
  bottomNotebook->AddPage(settingsPanel, wxT("Settings"), true);
  bottomNotebook->AddPage(markPanel, wxT("Mark"), false);
  
  sizer->Add(topSizer, 0, wxEXPAND | wxALL, 5);
  sizer->Add(bottomNotebook, 0, wxEXPAND | wxALL, 5);
  sizer->Fit(panel);
  panel->SetSizer(sizer);
  panel->Layout();
}

void MainFrame::setupSettingsPanel(wxPanel* panel) {
  wxStaticBoxSizer* parSizer = new wxStaticBoxSizer(wxVERTICAL,panel,
      wxT("Parameters"));

  int lflags = wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL; 
  int rflags = wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL; 
  int border = 3;

  wxFlexGridSizer* styleSizer = new wxFlexGridSizer(2,2,0,0);
  styleSizer->AddGrowableCol(0);
  styleSizer->AddGrowableCol(1);
  styleSizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Rank style:")),1,lflags,
      border);
  wxString rs_choices[2] = { wxT("Iterative"),wxT("Cyclic") };
  wxChoice* rankstyleChoice = new wxChoice(panel,myID_RANK_STYLE,
      wxDefaultPosition,wxDefaultSize,2,rs_choices);
  rankstyleChoice->SetSelection(0);
  styleSizer->Add(rankstyleChoice,1,lflags,border);
  styleSizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Visualisation style:")),
      1,lflags,border);
  wxString vs_choices[3] = { wxT("Cones"),wxT("Tubes"),wxT("Atomium") };
  wxChoice* visstyleChoice = new wxChoice(panel,myID_VIS_STYLE,
      wxDefaultPosition,wxDefaultSize,3,vs_choices);
  visstyleChoice->SetSelection(0);
  styleSizer->Add(visstyleChoice,1,lflags,border);
  
  wxFlexGridSizer* parsubSizer = new wxFlexGridSizer(7,2,0,0);
  parsubSizer->AddGrowableCol(0);
  for (int i=0; i<5; ++i) {
    parsubSizer->AddGrowableRow(i);
  }
  parSizer->Add(parsubSizer,1,wxEXPAND|wxALL,0);
  
  wxSize spinctrlSize(50,-1);

  nodesizeSpinCtrl = new wxSpinCtrl(panel,myID_SETTINGS_CONTROL,
      wxEmptyString,wxDefaultPosition);
  nodesizeSpinCtrl->SetRange(0,1000);
  nodesizeSpinCtrl->SetSizeHints(spinctrlSize,spinctrlSize);
  parsubSizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Node size:")),0,lflags,
      border);
  parsubSizer->Add(nodesizeSpinCtrl,0,rflags,border);
  
  branchrotationSpinCtrl = new wxSpinCtrl(panel,myID_SETTINGS_CONTROL,
      wxEmptyString,wxDefaultPosition);
  branchrotationSpinCtrl->SetRange(0, 360);
  branchrotationSpinCtrl->SetSizeHints(spinctrlSize, spinctrlSize);
  parsubSizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Branch rotation:")),0,
      lflags,border);
  parsubSizer->Add(branchrotationSpinCtrl,0,rflags,border);

  innerbranchtiltSpinCtrl = new wxSpinCtrl(panel,myID_SETTINGS_CONTROL,
      wxEmptyString,wxDefaultPosition);
  innerbranchtiltSpinCtrl->SetRange(0,90);
  innerbranchtiltSpinCtrl->SetSizeHints(spinctrlSize,spinctrlSize);
  parsubSizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Inner branch tilt:")),0,
      lflags,border);
  parsubSizer->Add(innerbranchtiltSpinCtrl,0,rflags,border);

  outerbranchtiltSpinCtrl = new wxSpinCtrl(panel,myID_SETTINGS_CONTROL,
      wxEmptyString,wxDefaultPosition);
  outerbranchtiltSpinCtrl->SetRange(0,90);
  outerbranchtiltSpinCtrl->SetSizeHints(spinctrlSize,spinctrlSize);
  parsubSizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Outer branch tilt:")),0,
      lflags,border);
  parsubSizer->Add(outerbranchtiltSpinCtrl,0,rflags,border);
  
  qualitySpinCtrl = new wxSpinCtrl(panel, myID_SETTINGS_CONTROL,wxEmptyString,
      wxDefaultPosition);
  qualitySpinCtrl->SetRange(2,50);
  qualitySpinCtrl->SetSizeHints(spinctrlSize,spinctrlSize);
  parsubSizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Accuracy:")),0,lflags,
      border);
  parsubSizer->Add(qualitySpinCtrl,0,rflags,border);
  
  ellipsoidSpinCtrl = new wxSpinCtrl(panel,myID_SETTINGS_CONTROL,wxEmptyString,
      wxDefaultPosition);
  ellipsoidSpinCtrl->SetRange(1,100);
  ellipsoidSpinCtrl->SetSizeHints(spinctrlSize, spinctrlSize);
  parsubSizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Ellipsoid threshold:")),
      0,lflags,border);
  parsubSizer->Add(ellipsoidSpinCtrl,0,rflags,border);

  // Setup the Colors panel
  
  wxStaticBoxSizer* colSizer = new wxStaticBoxSizer(wxVERTICAL,panel,
      wxT("Colours"));
  
  wxFlexGridSizer* colsubSizer = new wxFlexGridSizer(8,3,0,0);
  colsubSizer->AddGrowableCol(0);
  for (int i=0; i<8 ; ++i) {
    colsubSizer->AddGrowableRow(i);
  }
  colSizer->Add(colsubSizer,1,wxEXPAND|wxALL,0);
  
  transparencySpinCtrl = new wxSpinCtrl(panel,myID_SETTINGS_CONTROL);
  transparencySpinCtrl->SetRange(0,100);
  transparencySpinCtrl->SetSizeHints(spinctrlSize,spinctrlSize);
  colsubSizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Transparency:")),0,
      lflags,border);
  colsubSizer->AddSpacer(0);
  colsubSizer->Add(transparencySpinCtrl,0,rflags,border);

  wxSize btnSize(25,25);

  backgroundButton = new wxColorButton(panel,this,myID_COLOR_BUTTON);
  backgroundButton->SetSizeHints(btnSize,btnSize);
  colsubSizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Background:")),0,lflags,
      border);
  colsubSizer->AddSpacer(0);
  colsubSizer->Add(backgroundButton,0,rflags,border);

  nodeButton = new wxColorButton(panel,this,myID_COLOR_BUTTON);
  nodeButton->SetSizeHints(btnSize,btnSize);
  colsubSizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Node:")),0,lflags,
      border);
  colsubSizer->AddSpacer(0);
  colsubSizer->Add(nodeButton,0,rflags,border);

  downEdgeButton = new wxColorButton(panel,this,myID_COLOR_BUTTON);
  downEdgeButton->SetSizeHints(btnSize,btnSize);
  colsubSizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Edge (down/up):")),0,
      lflags,border);
  colsubSizer->Add(downEdgeButton,0,rflags,border);

  upEdgeButton = new wxColorButton(panel,this,myID_COLOR_BUTTON);
  upEdgeButton->SetSizeHints(btnSize,btnSize);
  colsubSizer->Add(upEdgeButton,0,rflags,border);

  markButton = new wxColorButton(panel,this,myID_COLOR_BUTTON);
  markButton->SetSizeHints(btnSize,btnSize);
  colsubSizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Mark:")),0,lflags,
      border);
  colsubSizer->AddSpacer(0);
  colsubSizer->Add(markButton,0,rflags,border);

  interpolate1Button = new wxColorButton(panel,this,myID_COLOR_BUTTON);
  interpolate1Button->SetSizeHints(btnSize,btnSize);
  interpolate2Button = new wxColorButton(panel,this,myID_COLOR_BUTTON);
  interpolate2Button->SetSizeHints(btnSize,btnSize);
  colsubSizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Interpolate:")),0,
      lflags,border);
  colsubSizer->Add(interpolate1Button,0,rflags,border);
  colsubSizer->Add(interpolate2Button,0,rflags,border);
  
  longinterpolateCheckBox = new wxCheckBox(panel,myID_SETTINGS_CONTROL,
      wxT("Long interpolation"));
  colsubSizer->Add(longinterpolateCheckBox,0,lflags,border);

  wxFlexGridSizer* panelSizer = new wxFlexGridSizer(4,1,0,0);
  panelSizer->AddGrowableCol(0);
  panelSizer->AddGrowableRow(0);
  panelSizer->AddGrowableRow(1);
  panelSizer->Add(styleSizer,0,wxEXPAND|wxALL,border);
  panelSizer->Add(parSizer,1,wxEXPAND|wxALL,5);
  panelSizer->Add(colSizer,1,wxEXPAND|wxALL,5);

  panelSizer->Add(new wxButton(panel,wxID_RESET,wxT("Default settings")),0,
      lflags,border);
  panelSizer->Fit(panel);
  panel->SetSizer(panelSizer);
  panel->Layout();
}

void MainFrame::setupMarkPanel(wxPanel* panel) {
  wxFlexGridSizer* markSizer = new wxFlexGridSizer(6,1,0,0);
  markSizer->AddGrowableCol(0);
  markSizer->AddGrowableRow(4);
  markSizer->AddGrowableRow(5);

  int flags = wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL;
  int border = 3;
  
  nomarksRadio = new wxRadioButton(panel,myID_MARK_RADIOBUTTON,wxT("No marks"),
      wxDefaultPosition,wxDefaultSize,wxRB_GROUP);
  nomarksRadio->SetValue(true);
  markDeadlocksRadio = new wxRadioButton(panel,myID_MARK_RADIOBUTTON,
      wxT("Mark deadlocks"));
  markStatesRadio = new wxRadioButton(panel,myID_MARK_RADIOBUTTON,
      wxT("Mark states"));
  markTransitionsRadio = new wxRadioButton(panel,myID_MARK_RADIOBUTTON,
      wxT("Mark transitions"));
    
  markSizer->Add(nomarksRadio,0,flags,border);
  markSizer->Add(markDeadlocksRadio,0,flags,border);
  markSizer->Add(markStatesRadio,0,flags,border);
  markSizer->Add(markTransitionsRadio,0,flags,border);
  
  wxStaticBoxSizer* markStatesSizer = new wxStaticBoxSizer(wxVERTICAL,panel,
      wxT("Mark states"));
  wxString choices[2] = { wxT("Match any of the following"),
      wxT("Match all of the following") };
  markAnyAllChoice = new wxChoice(panel,myID_MARK_ANYALL,wxDefaultPosition,
      wxDefaultSize,2,choices);
  markAnyAllChoice->SetSelection(0);
  markStatesSizer->Add(markAnyAllChoice,0,flags,border);
  
  markStatesListBox = new wxCheckListBox(panel,myID_MARK_RULES,
      wxDefaultPosition,wxDefaultSize,0,NULL,
      wxLB_SINGLE|wxLB_NEEDED_SB|wxLB_HSCROLL);
  markStatesSizer->Add(markStatesListBox,1,flags|wxEXPAND,border);
  wxBoxSizer* addremoveSizer = new wxBoxSizer(wxHORIZONTAL);
  addremoveSizer->Add(new wxButton(panel,myID_ADD_RULE,wxT("Add")),0,flags,
      border);
  addremoveSizer->Add(new wxButton(panel,myID_REMOVE_RULE,wxT("Remove")),0,
      flags,border);
  markStatesSizer->Add(addremoveSizer,0,flags,border);
  
  wxStaticBoxSizer* markTransitionsSizer = new wxStaticBoxSizer(wxVERTICAL,
      panel,wxT("Mark transitions"));
  markTransitionsListBox = new wxCheckListBox(panel,myID_MARK_TRANSITIONS,
      wxDefaultPosition,wxDefaultSize,0,NULL,wxLB_SINGLE|wxLB_SORT|
      wxLB_NEEDED_SB|wxLB_HSCROLL);
  markTransitionsSizer->Add(markTransitionsListBox,1,flags|wxEXPAND,border);
  
  markSizer->Add(markStatesSizer,0,wxEXPAND|wxALL,border);
  markSizer->Add(markTransitionsSizer,0,wxEXPAND|wxALL,border);
  markSizer->Fit(panel);
  panel->SetSizer(markSizer);
  panel->Layout();
}

GLCanvas* MainFrame::getGLCanvas() const {
  return glCanvas;
}

void MainFrame::setFileInfo(wxFileName fn) {
  filename.Assign(fn);
}

void MainFrame::onAbout(wxCommandEvent& /*event*/) {
  wxString ttl = wxT("About LTSView");
  wxString msg = wxT("LTSView - revision "REVISION"\n\n")
    wxT("Tool for the interactive visualisation of state transition systems.\n\n")
    wxT("Developed by Bas Ploeger and Carst Tankink.\n")
    wxT("Based on visualisation techniques by Frank van Ham and Jack van Wijk. ")
    wxT("See: F. van Ham, H. van de Wetering and J.J. van Wijk, ")
    wxT("\"Visualization of State Transition Graphs\". ")
    wxT("Proc. IEEE Symp. Information Visualization 2001, IEEE CS Press, pp. 59-66, 2001.\n\n")
    wxT("Distributed as part of the mCRL2 toolset. For information see: http://www.mcrl2.org\n\n")
    wxT("Please send all complaints, comments and bug reports to: bug@mcrl2.org\n");
  wxMessageDialog dlg(this,msg,ttl,wxOK|wxICON_INFORMATION);
  dlg.ShowModal();
}

/*
void MainFrame::onIdle(wxIdleEvent &event) {
  glCanvas->display();
}
*/
void MainFrame::onOpen(wxCommandEvent& /*event*/) {
  wxString filemask = wxT("FSM files (*.fsm)|*.fsm");
  wxFileDialog* dialog = new wxFileDialog(this,wxT("Open LTS"),
      filename.GetPath(),filename.GetFullName(),filemask,wxFD_OPEN);
  dialog->CentreOnParent();
  if (dialog->ShowModal() == wxID_OK) {
    filename.Assign(dialog->GetPath());
    mediator->openFile(string(filename.GetFullPath().fn_str()));
  }
  dialog->Destroy();
}

void MainFrame::onSavePic(wxCommandEvent& /*event*/) {
  SavePicDialog sp_dlg(this,glCanvas,filename.GetFullName(),filename.GetPath());
  sp_dlg.ShowModal();
}

void MainFrame::onExit(wxCommandEvent& /*event*/) {
  Close();
}

void MainFrame::onActivateTool(wxCommandEvent& event) {
  glCanvas->setActiveTool(event.GetId());
}

void MainFrame::onRankStyle(wxCommandEvent& event) {
  if (event.GetSelection() == 0)
    mediator->setRankStyle(ITERATIVE);
  else
    mediator->setRankStyle(CYCLIC);
}

void MainFrame::onVisStyle(wxCommandEvent& event){
  if (event.GetSelection() == 0)
    mediator->setVisStyle(CONES);
  else if (event.GetSelection() == 1)
    mediator->setVisStyle(TUBES);
  else
    mediator->setVisStyle(ATOMIUM);
}

void MainFrame::onResetView(wxCommandEvent& /*event*/) {
  glCanvas->resetView();
}

void MainFrame::onDisplay(wxCommandEvent& event) {
  if (event.GetId() == myID_DISPLAY_STATES) {
    glCanvas->setDisplayStates(event.IsChecked());
  } else if (event.GetId() == myID_DISPLAY_TRANSITIONS) {
    glCanvas->setDisplayTransitions(event.IsChecked());
  } else if (event.GetId() == myID_DISPLAY_BACKPOINTERS) {
    glCanvas->setDisplayBackpointers(event.IsChecked());
  } else if (event.GetId() == myID_DISPLAY_WIREFRAME) {
    glCanvas->setDisplayWireframe(event.IsChecked());
  }
  glCanvas->display();
}

void MainFrame::onColorButton(wxCommandEvent& /*event*/) {
  mediator->applySettings();
}

void MainFrame::onSpinSettingChanged(wxSpinEvent& /*event*/) {
  mediator->applySettings();
}

void MainFrame::onCommandSettingChanged(wxCommandEvent& /*event*/) {
  mediator->applySettings();
}

void MainFrame::onResetButton(wxCommandEvent& /*event*/) {
  mediator->applyDefaultSettings();
}

void MainFrame::onMarkRadio(wxCommandEvent& event) {
  wxRadioButton* buttonClicked = (wxRadioButton*)event.GetEventObject();

  if (buttonClicked == nomarksRadio)
    mediator->applyMarkStyle(NO_MARKS);
  else if (buttonClicked == markDeadlocksRadio)
    mediator->applyMarkStyle(MARK_DEADLOCKS);
  else if (buttonClicked == markStatesRadio)
    mediator->applyMarkStyle(MARK_STATES);
  else if (buttonClicked == markTransitionsRadio)
    mediator->applyMarkStyle(MARK_TRANSITIONS);
}

void MainFrame::onMarkRuleActivate(wxCommandEvent& event) {
  int i = event.GetInt();
  mediator->activateMarkRule(i, markStatesListBox->IsChecked(i));
  markStatesRadio->SetValue(true);
}

void MainFrame::onMarkRuleEdit(wxCommandEvent& event) {
  mediator->editMarkRule(event.GetSelection());
}

void MainFrame::onMarkAnyAll(wxCommandEvent& event) {
  mediator->setMatchAnyMarkRule(event.GetSelection() == 0);
  markStatesRadio->SetValue(true);
}

void MainFrame::onAddMarkRuleButton(wxCommandEvent& /*event*/) {
  mediator->addMarkRule();
}

void MainFrame::onRemoveMarkRuleButton(wxCommandEvent& /*event*/) {
  int sel_index = markStatesListBox->GetSelection();
  if (sel_index != wxNOT_FOUND) {
    markStatesListBox->Delete(sel_index);
    mediator->removeMarkRule(sel_index);
    markStatesRadio->SetValue(true);
  }
  Layout();
}

void MainFrame::onMarkTransition(wxCommandEvent& event) {
  int i = event.GetInt();
  if (markTransitionsListBox->IsChecked(i)) {
    mediator->markAction(string(markTransitionsListBox->GetString(i).fn_str()));
  } else {
    mediator->unmarkAction(string(
          markTransitionsListBox->GetString(i).fn_str()));
  }
  markTransitionsRadio->SetValue(true);
}

VisSettings MainFrame::getVisSettings() const {
  RGB_Color downC = wxC_to_RGB(downEdgeButton->GetBackgroundColour());
  RGB_Color intC1 = wxC_to_RGB(interpolate1Button->GetBackgroundColour());
  RGB_Color intC2 = wxC_to_RGB(interpolate2Button->GetBackgroundColour());
  RGB_Color markC = wxC_to_RGB(markButton->GetBackgroundColour());
  RGB_Color stateC = wxC_to_RGB(nodeButton->GetBackgroundColour());
  RGB_Color upC = wxC_to_RGB(upEdgeButton->GetBackgroundColour());
  VisSettings result = {
    ellipsoidSpinCtrl->GetValue() / 10.0f,
    nodesizeSpinCtrl->GetValue() / 10.0f,
    branchrotationSpinCtrl->GetValue(),
    innerbranchtiltSpinCtrl->GetValue(),
    outerbranchtiltSpinCtrl->GetValue(),
    qualitySpinCtrl->GetValue()*2,
    static_cast<unsigned char>((100-transparencySpinCtrl->GetValue())*2.55f),
    longinterpolateCheckBox->GetValue(),
    downC,
    intC1,
    intC2,
    markC,
    stateC,
    upC
  };
  return result;
}

RGB_Color MainFrame::getBackgroundColor() const {
  return wxC_to_RGB(backgroundButton->GetBackgroundColour());
}

void MainFrame::setBackgroundColor(RGB_Color c) {
  backgroundButton->SetBackgroundColour(RGB_to_wxC(c));
}

void MainFrame::setVisSettings(VisSettings ss) {
  nodeButton->SetBackgroundColour(RGB_to_wxC(ss.stateColor));
  downEdgeButton->SetBackgroundColour(RGB_to_wxC(ss.downEdgeColor));
  ellipsoidSpinCtrl->SetValue(int(10*ss.ellipsoidThreshold));
  upEdgeButton->SetBackgroundColour(RGB_to_wxC(ss.upEdgeColor));
  markButton->SetBackgroundColour(RGB_to_wxC(ss.markedColor));
  innerbranchtiltSpinCtrl->SetValue(ss.innerBranchTilt);
  interpolate1Button->SetBackgroundColour(RGB_to_wxC(ss.interpolateColor1));
  interpolate2Button->SetBackgroundColour(RGB_to_wxC(ss.interpolateColor2));
  longinterpolateCheckBox->SetValue(ss.longInterpolation);
  transparencySpinCtrl->SetValue(static_cast<int>((255-ss.alpha)/2.55f));
  nodesizeSpinCtrl->SetValue(int(10*ss.nodeSize));
  outerbranchtiltSpinCtrl->SetValue(ss.outerBranchTilt);
  qualitySpinCtrl->SetValue(ss.quality/2);
  branchrotationSpinCtrl->SetValue(ss.branchRotation);
}

void MainFrame::createProgressDialog(const string title,const string text) {
  progDialog = new wxProgressDialog(wxString(title.c_str(),wxConvUTF8),
      wxString(text.c_str(),wxConvUTF8),100,this,
      wxPD_APP_MODAL|wxPD_AUTO_HIDE|wxPD_ELAPSED_TIME);
  progDialog->SetSize(wxSize(400,100));
  progDialog->CentreOnParent();
}

void MainFrame::updateProgressDialog(int val,string msg) {
  if (progDialog != NULL) {
    progDialog->Update(val,wxString(msg.c_str(),wxConvUTF8));
    if (val == 100) {
      progDialog->Close();
      progDialog->Destroy();
      progDialog = NULL;
    }
  }
}

void MainFrame::showMessage(string title,string text) {
  wxMessageDialog* msgDialog = new wxMessageDialog(this,
      wxString(text.c_str(),wxConvUTF8),wxString(title.c_str(),wxConvUTF8),
      wxOK);
  msgDialog->ShowModal();
  msgDialog->Close();
  msgDialog->Destroy();
}

void MainFrame::loadTitle() {
  wxString fn = filename.GetFullName();
  if (fn != wxEmptyString)
    SetTitle(fn + wxT(" - LTSView"));
  else
    SetTitle(wxT("LTSView"));
}

void MainFrame::setNumberInfo(int ns,int nt,int nc,int nr) {
  numberOfStatesLabel->SetLabel(wxString::Format(wxT("%d"), ns));
  numberOfTransitionsLabel->SetLabel(wxString::Format(wxT("%d"), nt));
  numberOfClustersLabel->SetLabel(wxString::Format(wxT("%d"), nc));
  numberOfRanksLabel->SetLabel(wxString::Format(wxT("%d"), nr));
  Layout();
}

void MainFrame::setMarkedStatesInfo(int number) {
  numberOfMarkedStatesLabel->SetLabel(wxString::Format(wxT("%d"),number));
  Layout();
}

void MainFrame::setMarkedTransitionsInfo(int number) {
  numberOfMarkedTransitionsLabel->SetLabel(wxString::Format(wxT("%d"),number));
  Layout();
}

void MainFrame::addMarkRule(wxString str) {
  markStatesListBox->Append(str);
  markStatesListBox->Check(markStatesListBox->GetCount()-1,true);
  markStatesRadio->SetValue(true);
  Layout();
}

void MainFrame::replaceMarkRule(int index,wxString str) {
  bool isChecked = markStatesListBox->IsChecked(index);
  markStatesListBox->SetString(index,str);
  markStatesListBox->Check(index,isChecked);
  markStatesRadio->SetValue(true);
}
  
void MainFrame::resetMarkRules() {
  markStatesListBox->Clear();
  markAnyAllChoice->SetSelection(0);
  nomarksRadio->SetValue(true);
  Layout();
}

void MainFrame::setActionLabels(vector< ATerm > &labels) {
  wxArrayString strLabels;
  strLabels.Alloc(labels.size());
  for (vector< ATerm >::iterator it=labels.begin(); it!=labels.end(); ++it) {
    strLabels.Add(wxString(ATwriteToString(*it),wxConvLocal));
  }
  strLabels.Sort();
  markTransitionsListBox->Set(strLabels);
  Layout();
}

void MainFrame::startRendering() {
  SetStatusText(wxT("Rendering..."),0);
  GetStatusBar()->Update();
}

void MainFrame::stopRendering() {
  /*double currentTime = clock() / CLOCKS_PER_SEC;
  ++frameCount;
  if (currentTime-previousTime > 1.0) {
    SetStatusText(wxString::Format(wxT("FPS: %.3f"),
          frameCount/(currentTime-previousTime)),1);
    previousTime = currentTime;
    frameCount = 0;
  }*/
  SetStatusText(wxT(""),0);
  GetStatusBar()->Update();
}

wxColour MainFrame::RGB_to_wxC(RGB_Color c) const {
  return wxColour(c.r,c.g,c.b);
}

RGB_Color MainFrame::wxC_to_RGB(wxColour c) const {
  RGB_Color result = {c.Red(),c.Green(),c.Blue()};
  return result;
}

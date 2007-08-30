// Author(s): Bas Ploeger and Carst Tankink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mainframe.cpp
/// \brief Add your file description here.

#include "mainframe.h"
#include <wx/bitmap.h>
#include <wx/event.h>
#include <wx/filedlg.h>
#include <wx/notebook.h>
#include <wx/sysopt.h>
//#include <time.h>
#include "ids.h"
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
  EVT_MENU  (wxID_PREFERENCES,MainFrame::onSettings)
  EVT_MENU  (myID_PAN,MainFrame::onActivateTool)
  EVT_MENU  (myID_ROTATE,MainFrame::onActivateTool)
  EVT_MENU  (myID_SELECT,MainFrame::onActivateTool)
  EVT_MENU  (myID_ZOOM,MainFrame::onActivateTool)
  EVT_MENU  (wxID_ABOUT,MainFrame::onAbout)
  EVT_MENU  (myID_ITERATIVE,MainFrame::onRankStyle)
  EVT_MENU  (myID_CYCLIC,MainFrame::onRankStyle)
  EVT_MENU  (myID_CONES_STYLE,MainFrame::onVisStyle)
  EVT_MENU  (myID_TUBES_STYLE,MainFrame::onVisStyle)
  EVT_MENU  (myID_ZOOM_IN_ABOVE, MainFrame::onZoomInAbove)
  EVT_MENU  (myID_ZOOM_IN_BELOW, MainFrame::onZoomInBelow)
  EVT_MENU  (myID_ZOOM_OUT, MainFrame::onZoomOut)

  EVT_BUTTON(wxID_RESET, MainFrame::onResetButton)
  EVT_RADIOBUTTON(myID_MARK_RADIOBUTTON, MainFrame::onMarkRadio)
  EVT_CHOICE(myID_MARK_ANYALL, MainFrame::onMarkAnyAll)
  EVT_LISTBOX_DCLICK(myID_MARK_RULES, MainFrame::onMarkRuleEdit)
  EVT_CHECKLISTBOX(myID_MARK_RULES, MainFrame::onMarkRuleActivate)
  EVT_CHECKLISTBOX(myID_MARK_TRANSITIONS, MainFrame::onMarkTransition)
  EVT_BUTTON(myID_ADD_RULE, MainFrame::onAddMarkRuleButton)
  EVT_BUTTON(myID_REMOVE_RULE, MainFrame::onRemoveMarkRuleButton)

  EVT_BUTTON(myID_SIM_START_BUTTON, MainFrame::onSimStartButton)
  EVT_BUTTON(myID_SIM_RESET_BUTTON, MainFrame::onSimResetButton)
  EVT_BUTTON(myID_SIM_STOP_BUTTON, MainFrame::onSimStopButton)

  EVT_LIST_ITEM_SELECTED(myID_SIM_TRANSITIONS_VIEW, 
                         MainFrame::onSimTransitionSelected)
  EVT_LIST_ITEM_ACTIVATED(myID_SIM_TRANSITIONS_VIEW, 
                          MainFrame::onSimTransitionActivated)
  EVT_BUTTON(myID_SIM_TRIGGER_BUTTON, MainFrame::onSimTriggerButton)
  EVT_BUTTON(myID_SIM_UNDO_BUTTON, MainFrame::onSimUndoButton)
  EVT_LIST_ITEM_SELECTED(myID_SIM_STATE_VIEW, MainFrame::onSimStateSelected)
//  EVT_IDLE(MainFrame::onIdle)
END_EVENT_TABLE()

MainFrame::MainFrame(Mediator* owner,Settings* ss)
  : wxFrame(NULL,wxID_ANY,wxT("LTSView")), simReader(NULL) {
//  previousTime = 0.0;
//  frameCount = 0;
  mediator = owner;
  settings = ss;
  progDialog = NULL;
  savePicDialog = NULL;
  settingsDialog = NULL;

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
  viewMenu->Append(myID_ZOOM_IN_ABOVE, wxT("Zoom into &above\tZ"),
      wxT("Zooms into the selected cluster and the clusters above it"));
  viewMenu->Append(myID_ZOOM_IN_BELOW, wxT("Zoom into &below\tX"),
      wxT("Zooms into the selected cluster and the clusters below it"));
  viewMenu->Append(myID_ZOOM_OUT, wxT("Zoom &out\tC"),
      wxT("Zooms out one level"));
  viewMenu->AppendSeparator();
  viewMenu->AppendRadioItem(myID_ITERATIVE,wxT("Iterative ranking"),
    wxT("Apply iterative ranking"));
  viewMenu->AppendRadioItem(myID_CYCLIC,wxT("Cyclic ranking"),
    wxT("Apply cyclic ranking"));
  viewMenu->AppendSeparator();
  viewMenu->AppendCheckItem(myID_DISPLAY_STATES, wxT("Display &states\tF3"),
      wxT("Show/hide individual states"));
  viewMenu->AppendCheckItem(myID_DISPLAY_TRANSITIONS, 
    wxT("Display &transitions\tF4"), wxT("Show/hide individual transitions"));
  viewMenu->AppendCheckItem(myID_DISPLAY_BACKPOINTERS,
    wxT("Display &backpointers\tF5"), wxT("Show/hide backpointers"));
  viewMenu->AppendCheckItem(myID_DISPLAY_WIREFRAME,
      wxT("Display &wireframe\tF6"),wxT("Toggle wireframe/surface"));
  viewMenu->AppendSeparator();
  viewMenu->AppendRadioItem(myID_CONES_STYLE,wxT("Cones"),
    wxT("Cones visualization style"));
  viewMenu->AppendRadioItem(myID_TUBES_STYLE,wxT("Tubes"),
    wxT("Tubes visualization style"));
  viewMenu->AppendSeparator();
  viewMenu->Append(wxID_PREFERENCES,wxT("S&ettings..."),
    wxT("Show the settings panel"));

  viewMenu->Check(myID_DISPLAY_STATES,settings->getBool(DisplayStates));
  viewMenu->Check(myID_DISPLAY_TRANSITIONS,
    settings->getBool(DisplayTransitions));
  viewMenu->Check(myID_DISPLAY_BACKPOINTERS,
    settings->getBool(DisplayBackpointers));
  viewMenu->Check(myID_DISPLAY_WIREFRAME,
    settings->getBool(DisplayWireframe));

  toolMenu->AppendRadioItem(myID_SELECT,wxT("&Select\tS"),wxT("Select tool"));
  toolMenu->AppendRadioItem(myID_PAN,wxT("&Pan\tD"),wxT("Pan tool"));
  toolMenu->AppendRadioItem(myID_ZOOM,wxT("&Zoom\tA"),wxT("Zoom tool"));
  toolMenu->AppendRadioItem(myID_ROTATE,wxT("&Rotate\tF"),wxT("Rotate tool"));

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

  wxSplitterWindow* rightPanel = new wxSplitterWindow(this,wxID_ANY,
    wxDefaultPosition, wxDefaultSize);
  setupRightPanel(rightPanel);

  int attribList[] = { WX_GL_RGBA,WX_GL_DOUBLEBUFFER,0 };
  glCanvas = new GLCanvas(mediator,this,settings,wxDefaultSize,attribList);
  
  mainSizer->Add(glCanvas,1,wxALIGN_CENTER|wxEXPAND|wxALL,0);
  mainSizer->Add(rightPanel,1,wxALIGN_CENTER|wxEXPAND|wxALL,0);
  
  mainSizer->Fit(this);
  SetSizer(mainSizer);
  Layout();

  // Now that we know the window size, update minimum pane size of the right 
  // panel and update Sash position accordingly.
  rightPanel->SetMinimumPaneSize(50);
  rightPanel->SetSashPosition(rightPanel->GetClientSize().GetHeight()/2, true);
}

void MainFrame::setupRightPanel(wxSplitterWindow* panel) {
  //wxFlexGridSizer* sizer = new wxFlexGridSizer(2,1,0,0);
  //sizer->AddGrowableCol(0);
  //sizer->AddGrowableRow(1);

  int lf = wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL;
  int rf = wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxEXPAND | wxALL;
  
  wxNotebook* topNotebook = new wxNotebook(panel, wxID_ANY);
  
  wxPanel* infoPanel  = new wxPanel(topNotebook, wxID_ANY);
  // setup the top part (information box)
  wxFlexGridSizer* topSizer = new wxFlexGridSizer(6,2,0,0);
  nsLabel = new wxStaticText(infoPanel,wxID_ANY,wxEmptyString,wxDefaultPosition,
    wxDefaultSize,wxALIGN_RIGHT|wxST_NO_AUTORESIZE);
  ntLabel = new wxStaticText(infoPanel,wxID_ANY,wxEmptyString,wxDefaultPosition,
    wxDefaultSize,wxALIGN_RIGHT|wxST_NO_AUTORESIZE);
  ncLabel = new wxStaticText(infoPanel,wxID_ANY,wxEmptyString,wxDefaultPosition,
    wxDefaultSize,wxALIGN_RIGHT|wxST_NO_AUTORESIZE);
  nrLabel = new wxStaticText(infoPanel,wxID_ANY,wxEmptyString,wxDefaultPosition,
    wxDefaultSize,wxALIGN_RIGHT|wxST_NO_AUTORESIZE);
  nmsLabel = new wxStaticText(infoPanel,
                              wxID_ANY,wxEmptyString,wxDefaultPosition,
    wxDefaultSize,wxALIGN_RIGHT|wxST_NO_AUTORESIZE);
  nmtLabel = new wxStaticText(infoPanel,
                              wxID_ANY,wxEmptyString,wxDefaultPosition,
    wxDefaultSize,wxALIGN_RIGHT|wxST_NO_AUTORESIZE);
  
  topSizer->AddGrowableCol(1);
  topSizer->Add(new wxStaticText(infoPanel,wxID_ANY,wxT("States:")),0,lf,3);
  topSizer->Add(nsLabel,0,rf,3);
  topSizer->Add(new wxStaticText(infoPanel,
                                 wxID_ANY,wxT("Transitions:")),0,lf,3);
  topSizer->Add(ntLabel,0,rf,3);
  topSizer->Add(new wxStaticText(infoPanel,wxID_ANY,wxT("Clusters:")),0,lf,3);
  topSizer->Add(ncLabel,0,rf,3);
  topSizer->Add(new wxStaticText(infoPanel,wxID_ANY,wxT("Ranks:")),0,lf,3);
  topSizer->Add(nrLabel,0,rf,3);
  topSizer->Add(new wxStaticText(infoPanel,
                                 wxID_ANY,wxT("Marked states:")),0,rf,3);
  topSizer->Add(nmsLabel,0,rf,3);
  topSizer->Add(new wxStaticText(infoPanel,
                                 wxID_ANY,wxT("Marked transitions:")),0,rf,3);
  topSizer->Add(nmtLabel,0,rf,3);
  
  topSizer->Fit(infoPanel);
  infoPanel->SetSizer(topSizer);
  infoPanel->Fit();
  infoPanel->Layout();

  topNotebook->AddPage(infoPanel, wxT("LTS information"), false);

  // TODO: Layout page with selection information
  selectionInfo = new wxScrolledWindow(topNotebook);
  selectionInfo->SetScrollRate(0,5);
  

  selSizer = new wxFlexGridSizer(1, 2, 0, 0);
  selSizer->AddGrowableCol(1);
//  selSizer->SetFlexibleDirection(wxVERTICAL);
  selSizer->Add(new wxStaticText(selectionInfo, wxID_ANY, 
                                 wxT("Parameter")),0,lf,3);
  selSizer->Add(new wxStaticText(selectionInfo, wxID_ANY, 
                                 wxT("Value"), wxDefaultPosition, 
                                            wxDefaultSize, 
                                            wxALIGN_RIGHT|wxST_NO_AUTORESIZE), 
                                            0, rf, 3);
  selSizer->Fit(selectionInfo);
  selectionInfo->SetSizer(selSizer);
  selectionInfo->Fit();
  selectionInfo->Layout();

  topNotebook->AddPage(selectionInfo, wxT("State information"), false);

  // setup the bottom part (notebook)
  wxNotebook* bottomNotebook = new wxNotebook(panel, wxID_ANY);
  wxScrolledWindow* markPanel = new wxScrolledWindow(bottomNotebook,wxID_ANY);
  markPanel->SetScrollRate(0,5);
  setupMarkPanel(markPanel);
  bottomNotebook->AddPage(markPanel, wxT("Mark"), false);

  wxScrolledWindow* simPanel = new wxScrolledWindow(bottomNotebook, wxID_ANY);
  simPanel->SetScrollRate(0,5);
  setupSimPanel(simPanel);
  bottomNotebook->AddPage(simPanel, wxT("Simulation"), false); 

  
  //sizer->Add(topNotebook, 0, wxEXPAND | wxALL, 5);
  //sizer->Add(bottomNotebook, 0, wxEXPAND | wxALL, 5);
  //sizer->Fit(panel);
  //panel->SetSizer(sizer);
  //panel->Fit();

  panel->SplitHorizontally(topNotebook, bottomNotebook);
  panel->SetSashGravity(1.0);
  
  panel->Layout();
  panel->UpdateSize();
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
      wxDefaultPosition,wxSize(200,100),0,NULL,
      wxLB_SINGLE|wxLB_NEEDED_SB|wxLB_HSCROLL);
  //markStatesListBox->SetMinSize(wxSize(200,-1));
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
      wxDefaultPosition,wxSize(200,-1),0,NULL,wxLB_SINGLE|wxLB_SORT|
      wxLB_NEEDED_SB|wxLB_HSCROLL);
  markTransitionsListBox->SetMinSize(wxSize(200,-1));
  markTransitionsSizer->Add(markTransitionsListBox,1,flags|wxEXPAND,border);
  
  markSizer->Add(markStatesSizer,0,wxEXPAND|wxALL,border);
  markSizer->Add(markTransitionsSizer,0,wxEXPAND|wxALL,border);
  markSizer->Fit(panel);
  panel->SetSizer(markSizer);
  panel->Fit();
  panel->Layout();
}

void MainFrame::setupSimPanel(wxPanel* panel) {
  // Container for all elements in tab.
  wxFlexGridSizer* simSizer = new wxFlexGridSizer(3, 1, 0, 0);
  simSizer->AddGrowableCol(0);
  simSizer->AddGrowableRow(1);
  simSizer->AddGrowableRow(2);

  int flags = wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL;
  int border = 3;

  // Buttons for general simulation control: start, reset, stop
  wxFlexGridSizer* simButtonSizer = new wxFlexGridSizer(1, 3, 0, 0);
  simButtonSizer->AddGrowableCol(0);
  simButtonSizer->AddGrowableCol(1);
  simButtonSizer->AddGrowableCol(2);

  simStartButton = new wxButton(panel, myID_SIM_START_BUTTON,
                                          wxT("Start"));
  simStartButton->Disable();

  simResetButton = new wxButton(panel, myID_SIM_RESET_BUTTON,
                                          wxT("Reset"));
  simResetButton->Disable();

  simStopButton  = new wxButton(panel, myID_SIM_STOP_BUTTON,
                                          wxT("Stop"));
  simStopButton->Disable();

  simButtonSizer->Add(simStartButton, 0, flags, border);
  simButtonSizer->Add(simResetButton, 0, flags, border);
  simButtonSizer->Add(simStopButton,  0, flags, border);

  simSizer->Add(simButtonSizer, 1, flags, border);

  // List of transitions and buttons to fire these transitions and go back one
  // step
  wxStaticBoxSizer* simTransSizer = new wxStaticBoxSizer(wxVERTICAL, panel, 
    wxT("Transitions"));
  int listViewStyle = wxLC_REPORT|wxSUNKEN_BORDER|wxLC_HRULES|wxLC_VRULES|
                      wxLC_SINGLE_SEL;

  simTransView = new wxListView(panel, myID_SIM_TRANSITIONS_VIEW, 
    wxDefaultPosition, wxSize(200, 100), listViewStyle);

  simTransView->InsertColumn(0, wxT("Action"), wxLIST_FORMAT_LEFT, 120);
  simTransView->InsertColumn(1, wxT("State change"), wxLIST_FORMAT_LEFT);
  simTransView->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER|wxLIST_AUTOSIZE);

  simTransSizer->Add(simTransView, 1, flags|wxEXPAND, border);

  wxBoxSizer* triggerUndoSizer = new wxBoxSizer(wxHORIZONTAL);
  simTriggerButton = new wxButton(panel, myID_SIM_TRIGGER_BUTTON,
                                         wxT("Trigger"));
  simTriggerButton->Disable();
  triggerUndoSizer->Add(simTriggerButton, 0, flags, border);

  simUndoButton = new wxButton(panel, myID_SIM_UNDO_BUTTON, wxT("Undo"));
  simUndoButton->Disable();
  triggerUndoSizer->Add(simUndoButton, 0, flags, border);
  simTransSizer->Add(triggerUndoSizer, 0, flags, border);

  simSizer->Add(simTransSizer, 0, wxEXPAND|wxALL, border);


  // Information about current state
  /*wxStaticBoxSizer* simStateSizer = new wxStaticBoxSizer(wxVERTICAL, panel, 
                                                         wxT("Current state"));
  simStateView = new wxListView(panel, myID_SIM_STATE_VIEW, 
                                         wxDefaultPosition, wxSize(200, 100), 
                                         listViewStyle);
  simStateView->InsertColumn(0, wxT("Parameter"), wxLIST_FORMAT_LEFT, 120);
  simStateView->InsertColumn(1, wxT("Value"), wxLIST_FORMAT_LEFT);
  simStateView->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER|wxLIST_AUTOSIZE);

  simStateSizer->Add(simStateView, 1, flags|wxEXPAND, border);
  simSizer->Add(simStateSizer, 0, wxEXPAND|wxALL, border);*/
  
  panel->SetSizer(simSizer);
  panel->Fit();
  panel->Layout();

  // Now the panel has been laid out, we can get fill up the columns
  simTransView->SetColumnWidth(1, simTransView->GetClientSize().GetWidth() - 
                               simTransView->GetColumnWidth(0));
  /*simStateView->SetColumnWidth(1, simStateView->GetClientSize().GetWidth() - 
                               simStateView->GetColumnWidth(0));*/
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
  if (savePicDialog == NULL) {
    savePicDialog = new SavePicDialog(this,GetStatusBar(),glCanvas,filename);
  }
  savePicDialog->ShowModal();
}

void MainFrame::onExit(wxCommandEvent& /*event*/) {
  if (settingsDialog != NULL) {
    settingsDialog->Destroy();
  }
  Close();
}

void MainFrame::onActivateTool(wxCommandEvent& event) {
  glCanvas->setActiveTool(event.GetId());
}

void MainFrame::onRankStyle(wxCommandEvent& event) {
  if (event.GetId() == myID_ITERATIVE) {
    mediator->setRankStyle(ITERATIVE);
  } else if (event.GetId() == myID_CYCLIC) {
    mediator->setRankStyle(CYCLIC);
  }
}

void MainFrame::onVisStyle(wxCommandEvent& event){
  if (event.GetId() == myID_CONES_STYLE) {
    mediator->setVisStyle(CONES);
  } else if (event.GetId() == myID_TUBES_STYLE) {
    mediator->setVisStyle(TUBES);
  }
}

void MainFrame::onResetView(wxCommandEvent& /*event*/) {
  glCanvas->resetView();
}

void MainFrame::onDisplay(wxCommandEvent& event) {
  if (event.GetId() == myID_DISPLAY_STATES) {
    settings->setBool(DisplayStates,event.IsChecked());
  } else if (event.GetId() == myID_DISPLAY_TRANSITIONS) {
    settings->setBool(DisplayTransitions,event.IsChecked());
  } else if (event.GetId() == myID_DISPLAY_BACKPOINTERS) {
    settings->setBool(DisplayBackpointers,event.IsChecked());
  } else if (event.GetId() == myID_DISPLAY_WIREFRAME) {
    settings->setBool(DisplayWireframe,event.IsChecked());
  } else {
    return;
  }
  glCanvas->display();
}

void MainFrame::onSettings(wxCommandEvent& /*event*/) {
  if (settingsDialog == NULL) {
    settingsDialog = new SettingsDialog(this,glCanvas,settings);
  }
  settingsDialog->Show();
}

void MainFrame::onResetButton(wxCommandEvent& /*event*/) {
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
    markStatesListBox->GetParent()->Fit();
    Layout();
  }
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

void MainFrame::onZoomInBelow(wxCommandEvent& event)
{
  mediator->zoomInBelow();
  glCanvas->display(); 
}

void MainFrame::onZoomInAbove(wxCommandEvent& event)
{
  mediator->zoomInAbove();
  glCanvas->display();
}

void MainFrame::onZoomOut(wxCommandEvent& event)
{
  mediator->zoomOut();
  glCanvas->display();
}

// Simulation event handlers implementations
void MainFrame::onSimStartButton(wxCommandEvent& event) {
  mediator->startSim();
}

void MainFrame::onSimResetButton(wxCommandEvent& event) {
  sim->resetSim();
}

void MainFrame::onSimStopButton(wxCommandEvent& event) {
  sim->stop();
}

void MainFrame::onSimTransitionSelected(wxListEvent& event) {
  // Get index of transition that was selected
  int trans = event.GetIndex();

  // Choose trans to be the next transition
  sim->chooseTrans(trans);

  
}

void MainFrame::onSimTransitionActivated(wxListEvent& event) {
  onSimTransitionSelected(event);
  
  sim->followTrans();
}
void MainFrame::onSimTriggerButton(wxCommandEvent& event) {
  sim->followTrans();
}

void MainFrame::onSimUndoButton(wxCommandEvent& event) {
  sim->undoStep();
}

void MainFrame::onSimStateSelected(wxListEvent& event) {
  // Is this one necessary?
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
      wxString(text.c_str(),wxConvLocal),wxString(title.c_str(),wxConvLocal),
      wxOK);
  msgDialog->ShowModal();
  msgDialog->Close();
  msgDialog->Destroy();
}

void MainFrame::loadTitle() {
  wxString fn = filename.GetFullName();
  if (fn != wxEmptyString) {
    SetTitle(fn + wxT(" - LTSView"));
  } else {
    SetTitle(wxT("LTSView"));
  }
}

void MainFrame::setNumberInfo(int ns,int nt,int nc,int nr) {
  nsLabel->SetLabel(wxString::Format(wxT("%d"), ns));
  ntLabel->SetLabel(wxString::Format(wxT("%d"), nt));
  ncLabel->SetLabel(wxString::Format(wxT("%d"), nc));
  nrLabel->SetLabel(wxString::Format(wxT("%d"), nr));
  nrLabel->GetParent()->Fit();
  Layout();
}

void MainFrame::setMarkedStatesInfo(int number) {
  nmsLabel->SetLabel(wxString::Format(wxT("%d"),number));
  Layout();
}

void MainFrame::setMarkedTransitionsInfo(int number) {
  nmtLabel->SetLabel(wxString::Format(wxT("%d"),number));
  Layout();
}

void MainFrame::addMarkRule(wxString str) {
  markStatesListBox->Append(str);
  markStatesListBox->Check(markStatesListBox->GetCount()-1,true);
  markStatesRadio->SetValue(true);
  markStatesListBox->GetParent()->Fit();
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
  markStatesListBox->GetParent()->Fit();
  Layout();
}

void MainFrame::setActionLabels(vector< string > &labels) {
  wxArrayString strLabels;
  strLabels.Alloc(labels.size());
  for (vector<string>::iterator it = labels.begin(); it != labels.end(); ++it) {
    strLabels.Add(wxString(it->c_str(),wxConvLocal));
  }
  strLabels.Sort();
  markTransitionsListBox->Set(strLabels);
  markTransitionsListBox->GetParent()->Fit();
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

void MainFrame::addParameter(std::string par)
{
  if (parameters.find(par) == parameters.end())
  {
    std::pair<std::string, wxStaticText*> param(par, 
        new wxStaticText(selectionInfo, wxID_ANY, wxEmptyString,
                         wxDefaultPosition, wxDefaultSize,
                         wxALIGN_RIGHT|wxST_NO_AUTORESIZE));

    parameters.insert(param);

    selSizer->SetRows(selSizer->GetRows() + 1);
    selSizer->Add(new wxStaticText(selectionInfo, wxID_ANY, 
                  wxString(par.c_str(), wxConvLocal)), 0,
                  wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL|wxALL,3);
    selSizer->Add(parameters.find(par)->second, 0,
                        wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxEXPAND | wxALL, 3);
  }
}

void MainFrame::setParameterValue(std::string par, std::string value)
{
  addParameter(par);
  map<string, wxStaticText*>::iterator it = parameters.find(par);
  
  it->second->SetLabel(wxString(value.c_str(), wxConvLocal));
  it->second->GetParent()->Fit();
}

void MainFrame::refresh() {
  if (sim != NULL) {
    // There is a simulation, so we can request information from it.
    if (!sim->getStarted()) {
      // The simulation has not yet been started, enable the start button.
      simStartButton->Enable();
      simResetButton->Disable();
      simStopButton->Disable();

      // Clear the list view
      simTransView->DeleteAllItems();
    }
    else {
      // The simluation has been started, disable start button, enable stop and 
      // reset buttons
      simStartButton->Disable();

      simResetButton->Enable();
      simStopButton->Enable();


      // Refresh the transition list
      simTransView->DeleteAllItems();

      vector<Transition*> posTrans = sim->getPosTrans();
      
      State* currState = sim->getCurrState();
      
      // Get the possible transitions
      for(size_t i = 0; i < posTrans.size(); ++i) {
        int labelId = posTrans[i]->getLabel();
        string label = mediator->getActionLabel(labelId);

        simTransView->InsertItem(i, wxString(label.c_str(), wxConvLocal));

        // Determine the state change this action will effectuate.
        State* nextState = posTrans[i]->getEndState();
        
        wxString stateChange = wxT("");
        if ((nextState != NULL) && (currState != NULL)) {
          for(int j = 0; j < mediator->getNumberOfParams(); ++j) {
            string nextVal = mediator->getParValue(j, 
                                       nextState->getParameterValue(j));
            if (mediator->getParValue(j, currState->getParameterValue(j)) !=
                nextVal) {
              stateChange += wxString(mediator->getParName(j).c_str(), 
                               wxConvLocal) +  
                            wxT(":=") + 
                            wxString(nextVal.c_str(), wxConvLocal) +
                            wxT(",");
            }
          }
          // Remove last comma. There always is one since an empty transition 
          // does not exist
          stateChange.RemoveLast();
        }
        // Add stateChange value to the list
        simTransView->SetItem(i, 1, stateChange);
      }
      
      // Display selected transition
      int chosenTrans = sim->getChosenTransi();

      if(chosenTrans != -1) {
        simTransView->Select(chosenTrans);
      }
      
      // Trigger and undo buttons
      if(sim->getChosenTransi() != -1) {
        simTriggerButton->Enable();
      }
      else {
        simTriggerButton->Disable();
      }

      if(sim->getTransHis().size() != 0) {
        simUndoButton->Enable();
      }
      else {
        simUndoButton->Disable();
      }


      // Refresh the state list with the information about the current state
      /*simStateView->DeleteAllItems();
      
      if (currState != NULL) {
        for(int i = 0; i < mediator->getNumberOfParams(); ++i) {
          // Parameter name
          simStateView->InsertItem(i, wxString(mediator->getParName(i).c_str(),
                                               wxConvLocal));

          // Parameter value
          simStateView->SetItem(i, 1, wxString(
            mediator->getParValue(i, currState->getParameterValue(i)).c_str(), 
              wxConvLocal));
        }
      }*/
    }
  }
  Layout();
}

void MainFrame::selChange() {
  // There always is a simulation to inform us of a selection change.
  int j = sim->getChosenTransi();

  if(j != -1) 
  {
    simTransView->Select(j);
    simTriggerButton->Enable();
  }
  else 
  {
    simTriggerButton->Disable();
  }
}

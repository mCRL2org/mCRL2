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
  EVT_MENU  (myID_ATOMIUM_STYLE,MainFrame::onVisStyle)
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

MainFrame::MainFrame(Mediator* owner,Settings* ss)
  : wxFrame(NULL,wxID_ANY,wxT("LTSView")) {
//  previousTime = 0.0;
//  frameCount = 0;
  mediator = owner;
  settings = ss;
  progDialog = NULL;

  SetIcon(wxIcon(main_window));

  CreateStatusBar(2);
  setupMenuBar();
  setupMainArea();
  settingsDialog = new SettingsDialog(this,glCanvas,ss);
  
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
  viewMenu->AppendRadioItem(myID_ATOMIUM_STYLE,wxT("Atomium"),
    wxT("Atomium visualization style"));
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
  viewMenu->Enable(myID_TUBES_STYLE,false);
  viewMenu->Enable(myID_ATOMIUM_STYLE,false);

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

  int attribList[] = { WX_GL_RGBA,WX_GL_DOUBLEBUFFER };
  glCanvas = new GLCanvas(mediator,this,settings,wxDefaultSize,attribList);
  
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

  int lf = wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL;
  int rf = wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxEXPAND | wxALL;
  
  // setup the top part (information box)
  wxFlexGridSizer* topSizer = new wxFlexGridSizer(6,2,0,0);
  nsLabel = new wxStaticText(panel,wxID_ANY,wxEmptyString,wxDefaultPosition,
    wxDefaultSize,wxALIGN_RIGHT|wxST_NO_AUTORESIZE);
  ntLabel = new wxStaticText(panel,wxID_ANY,wxEmptyString,wxDefaultPosition,
    wxDefaultSize,wxALIGN_RIGHT|wxST_NO_AUTORESIZE);
  ncLabel = new wxStaticText(panel,wxID_ANY,wxEmptyString,wxDefaultPosition,
    wxDefaultSize,wxALIGN_RIGHT|wxST_NO_AUTORESIZE);
  nrLabel = new wxStaticText(panel,wxID_ANY,wxEmptyString,wxDefaultPosition,
    wxDefaultSize,wxALIGN_RIGHT|wxST_NO_AUTORESIZE);
  nmsLabel = new wxStaticText(panel,wxID_ANY,wxEmptyString,wxDefaultPosition,
    wxDefaultSize,wxALIGN_RIGHT|wxST_NO_AUTORESIZE);
  nmtLabel = new wxStaticText(panel,wxID_ANY,wxEmptyString,wxDefaultPosition,
    wxDefaultSize,wxALIGN_RIGHT|wxST_NO_AUTORESIZE);
  
  topSizer->AddGrowableCol(1);
  topSizer->Add(new wxStaticText(panel,wxID_ANY,wxT("States:")),0,lf,3);
  topSizer->Add(nsLabel,0,rf,3);
  topSizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Transitions:")),0,lf,3);
  topSizer->Add(ntLabel,0,rf,3);
  topSizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Clusters:")),0,lf,3);
  topSizer->Add(ncLabel,0,rf,3);
  topSizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Ranks:")),0,lf,3);
  topSizer->Add(nrLabel,0,rf,3);
  topSizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Marked states:")),0,rf,3);
  topSizer->Add(nmsLabel,0,rf,3);
  topSizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Marked transitions:")),0,rf,3);
  topSizer->Add(nmtLabel,0,rf,3);

  // setup the bottom part (notebook)
  wxNotebook* bottomNotebook = new wxNotebook(panel, wxID_ANY);
  wxScrolledWindow* markPanel = new wxScrolledWindow(bottomNotebook,wxID_ANY);
  markPanel->SetScrollRate(0,5);
  setupMarkPanel(markPanel);
  bottomNotebook->AddPage(markPanel, wxT("Mark"), false);
  
  sizer->Add(topSizer, 0, wxEXPAND | wxALL, 5);
  sizer->Add(bottomNotebook, 0, wxEXPAND | wxALL, 5);
  sizer->Fit(panel);
  panel->SetSizer(sizer);
  panel->Fit();
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
  settingsDialog->Destroy();
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
  } else if (event.GetId() == myID_ATOMIUM_STYLE) {
    mediator->setVisStyle(ATOMIUM);
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

void MainFrame::setActionLabels(vector< ATerm > &labels) {
  wxArrayString strLabels;
  strLabels.Alloc(labels.size());
  for (vector< ATerm >::iterator it=labels.begin(); it!=labels.end(); ++it) {
    strLabels.Add(wxString(ATwriteToString(*it),wxConvLocal));
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

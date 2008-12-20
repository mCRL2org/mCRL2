// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mainframe.cpp
/// \brief Implements the main LTSView window

#include "wx.hpp" // precompiled headers

#include "mainframe.h"
#include <wx/checklst.h>
#include <wx/filedlg.h>
#include <wx/notebook.h>
#include <wx/progdlg.h>
#include <mcrl2/lts/lts.h>
//#include <time.h>
#include "glcanvas.h"
#include "icons/main_window.xpm"
#include "ids.h"
#include "infodialog.h"
#include "simdialog.h"
#include "markdialog.h"
#include "mediator.h"
#include "savepicdialog.h"
#include "savevecdialog.h"
#include "settings.h"
#include "settingsdialog.h"
#include "mcrl2/utilities/about_dialog.h"

// For compatibility with older wxWidgets versions (pre 2.8)
#if (wxMINOR_VERSION < 8)
# define wxFD_OPEN wxOPEN
#endif

using namespace std;
using namespace Utils;
using namespace IDs;
// Event table
BEGIN_EVENT_TABLE(MainFrame, wxFrame)
  EVT_MENU  (wxID_OPEN, MainFrame::onOpen)
  EVT_MENU  (myID_OPEN_TRACE, MainFrame::onOpenTrace)
  EVT_MENU  (myID_SAVEPIC, MainFrame::onSavePic)
  EVT_MENU  (myID_SAVEVEC, MainFrame::onSaveVec)
  EVT_MENU  (myID_SAVETXT, MainFrame::onSaveText)
  EVT_MENU  (wxID_EXIT, MainFrame::onExit)
  EVT_MENU  (wxID_RESET, MainFrame::onResetView)
  EVT_MENU  (myID_DISPLAY_STATES,MainFrame::onDisplay)
  EVT_MENU  (myID_DISPLAY_TRANSITIONS,MainFrame::onDisplay)
  EVT_MENU  (myID_DISPLAY_BACKPOINTERS,MainFrame::onDisplay)
  EVT_MENU  (myID_DISPLAY_WIREFRAME,MainFrame::onDisplay)
  EVT_MENU  (wxID_PREFERENCES,MainFrame::onSettings)
  EVT_MENU  (myID_INFO,MainFrame::onInfo)
  EVT_MENU  (myID_MARK,MainFrame::onMark)
  EVT_MENU  (myID_SIM, MainFrame::onSim)
  EVT_MENU  (myID_PAN,MainFrame::onActivateTool)
  EVT_MENU  (myID_ROTATE,MainFrame::onActivateTool)
  EVT_MENU  (myID_SELECT,MainFrame::onActivateTool)
  EVT_MENU  (myID_ZOOM,MainFrame::onActivateTool)
  EVT_MENU  (wxID_ABOUT,MainFrame::onAbout)
  EVT_MENU  (wxID_HELP,MainFrame::onHelpContents)
  EVT_MENU  (myID_ITERATIVE,MainFrame::onRankStyle)
  EVT_MENU  (myID_CYCLIC,MainFrame::onRankStyle)
  EVT_MENU  (myID_CONES_STYLE,MainFrame::onVisStyle)
  EVT_MENU  (myID_TUBES_STYLE,MainFrame::onVisStyle)
  EVT_MENU  (myID_FSM_STYLE,MainFrame::onFSMStyle)
  EVT_MENU  (myID_ZOOM_IN_ABOVE, MainFrame::onZoomInAbove)
  EVT_MENU  (myID_ZOOM_IN_BELOW, MainFrame::onZoomInBelow)
  EVT_MENU  (myID_ZOOM_OUT, MainFrame::onZoomOut)
  EVT_MENU  (myID_START_FORCE_DIRECTED, MainFrame::onStartForceDirected)
  EVT_MENU  (myID_STOP_FORCE_DIRECTED, MainFrame::onStopForceDirected)
  EVT_MENU  (myID_RESET_STATE_POSITIONS, MainFrame::onResetStatePositions)

//  EVT_IDLE(MainFrame::onIdle)
  EVT_CLOSE(MainFrame::onClose)
END_EVENT_TABLE()

MainFrame::MainFrame(Mediator* owner,Settings* ss)
  : wxFrame(NULL,wxID_ANY,wxT("LTSView"))  {
//  previousTime = 0.0;
//  frameCount = 0;
  mediator = owner;
  settings = ss;
  progDialog = NULL;
  savePicDialog = NULL;
  saveVecDialog = NULL;
  settingsDialog = NULL;
  infoDialog = new InfoDialog(this);
  simDialog = new SimDialog(this, mediator);
  markDialog = new MarkDialog(this, mediator);

  SetIcon(wxIcon(main_window));

  CreateStatusBar(2);
  setupMenuBar();
  setupMainArea();
  
  SetSize(800,600);
  CentreOnScreen();
}

void MainFrame::setSim(Simulation* sim)
{
  //this->sim = sim;
  simDialog->setSim(sim);
}

void MainFrame::setupMenuBar() {
  // Set up the menu bar
  wxMenuBar* menuBar = new wxMenuBar;
  wxMenu* fileMenu = new wxMenu;
  wxMenu* exportMenu = new wxMenu;
  wxMenu* viewMenu = new wxMenu;
  toolMenu = new wxMenu;
  wxMenu* helpMenu = new wxMenu;
  
  fileMenu->Append(wxID_OPEN,wxT("&Open...\tCtrl+O"),
    wxT("Load an LTS from file"));
  fileMenu->Append(myID_OPEN_TRACE, wxT("Open &Trace...\tCtrl+T"),
    wxT("Open a trace for this file"));
  fileMenu->AppendSeparator();
  exportMenu->Append(myID_SAVEPIC,wxT("&Bitmap..."),
      wxT("Export picture to bitmap"));
  exportMenu->Append(myID_SAVETXT,wxT("&Text..."),
      wxT("Export picture to text"));
  exportMenu->Append(myID_SAVEVEC,wxT("&Vector..."),
      wxT("Export picture to vector graphics"));
  fileMenu->AppendSubMenu(exportMenu,wxT("Export"),wxT("Export picture"));
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
  viewMenu->AppendCheckItem(myID_FSM_STYLE,
    wxT("FSMView style"), wxT("Toggle FSMView style"));
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
  toolMenu->AppendSeparator();
  toolMenu->Append(myID_INFO, wxT("&Information...\tCtrl+I"),
      wxT("Show information dialog"));
  toolMenu->Append(myID_SIM, wxT("Sim&ulation...\tCtrl+S"),
      wxT("Show simulation dialog"));
  toolMenu->Append(myID_MARK, wxT("&Mark...\tCtrl+M"), wxT("Show mark dialog"));
  toolMenu->AppendSeparator();
  toolMenu->Append(myID_START_FORCE_DIRECTED,wxT("Start &force directed"),
    wxT("Starts force directed state positioning algorithm"));
  toolMenu->Append(myID_STOP_FORCE_DIRECTED,wxT("Stop f&orce directed"),
    wxT("Stops force directed state positioning algorithm"));
  toolMenu->Append(myID_RESET_STATE_POSITIONS,wxT("R&eset state positions"),
    wxT("Assign states to their default positions"));
  toolMenu->Enable(myID_STOP_FORCE_DIRECTED,false);
  
  helpMenu->Append(wxID_HELP,wxT("&Contents"));
  helpMenu->AppendSeparator();
  helpMenu->Append(wxID_ABOUT,wxT("&About"));
  
  menuBar->Append(fileMenu, wxT("&File"));
  menuBar->Append(viewMenu, wxT("&View"));
  menuBar->Append(toolMenu, wxT("&Tools"));
  menuBar->Append(helpMenu, wxT("&Help"));
  
  SetMenuBar(menuBar);
}

void MainFrame::setupMainArea() {
  wxFlexGridSizer* mainSizer = new wxFlexGridSizer(1,1,0,0);
  mainSizer->AddGrowableCol(0);
  mainSizer->AddGrowableRow(0);


  int attribList[] = { WX_GL_RGBA,WX_GL_DOUBLEBUFFER,0 };
  glCanvas = new GLCanvas(mediator,this,settings,wxDefaultSize,attribList);
  
  mainSizer->Add(glCanvas,1,wxALIGN_CENTER|wxEXPAND|wxALL,0);
  
  mainSizer->Fit(this);
  SetSizer(mainSizer);
  Layout();
}

GLCanvas* MainFrame::getGLCanvas() const {
  return glCanvas;
}

void MainFrame::setFileInfo(wxFileName fn) {
  filename.Assign(fn);
}

void MainFrame::onHelpContents( wxCommandEvent& /* event */ )
{
  wxLaunchDefaultBrowser(wxT("http://www.mcrl2.org/wiki/index.php/User_manual/ltsview"));
}

void MainFrame::onAbout(wxCommandEvent& /*event*/) {
  std::vector<std::string> developers;
  developers.push_back("Bas Ploeger");
  developers.push_back("Carst Tankink");
  mcrl2::utilities::wx::about_dialog("LTSView",
    "Tool for interactive visualisation of state transition systems.\n"
    "\n"
    "LTSView is based on visualisation techniques by Frank van Ham and Jack van Wijk. "
    "See: F. van Ham, H. van de Wetering and J.J. van Wijk, "
    "\"Visualization of State Transition Graphs\". "
    "Proceedings of the IEEE Symposium on Information Visualization 2001. IEEE CS Press, pp. 59-66, 2001.\n"
    "\n"
    "The default colour scheme for state marking was obtained through http://www.colorbrewer.org",
    developers
  );
}

/*
void MainFrame::onIdle(wxIdleEvent &event) {
  glCanvas->display();
}
*/

void MainFrame::onOpen(wxCommandEvent& /*event*/) {
  wxString filemask = wxString(("All supported files (" +
        mcrl2::lts::lts::lts_extensions_as_string() +
        ")|" +
        mcrl2::lts::lts::lts_extensions_as_string(";") +
        "|All files (*.*)|*.*").c_str(),
        wxConvLocal);
  wxFileDialog* dialog = new wxFileDialog(this,wxT("Open LTS"),
      filename.GetPath(),filename.GetFullName(),filemask,wxFD_OPEN);
  dialog->CentreOnParent();
  if (dialog->ShowModal() == wxID_OK) {
    filename.Assign(dialog->GetPath());
    mediator->openFile(string(filename.GetFullPath().fn_str()));
  }
  dialog->Destroy();
}

void MainFrame::onOpenTrace(wxCommandEvent& /*event*/)
{
  wxString filemask = wxT("Traces (*.trc)|*.trc|All files (*.*)|*.*");
  wxFileDialog* dialog = new wxFileDialog(this, wxT("Open Trace"),
    filename.GetPath(), wxEmptyString,filemask,wxFD_OPEN);
  dialog->CentreOnParent();
  if (dialog->ShowModal() == wxID_OK)
  {
    std::string path(dialog->GetPath().mb_str());
    mediator->loadTrace(path);
  }
  dialog->Destroy();
}

void MainFrame::onSavePic(wxCommandEvent& /*event*/)
{
  if (savePicDialog == NULL)
  {
    savePicDialog = new SavePicDialog(this,GetStatusBar(),glCanvas,filename);
  }
  else
  {
    savePicDialog->updateAspectRatio();
  }
  savePicDialog->ShowModal();
}

void MainFrame::onSaveVec(wxCommandEvent& /*event*/)
{
  if (saveVecDialog == NULL)
  {
    saveVecDialog = new SaveVecDialog(this,GetStatusBar(),glCanvas,filename);
  }
  saveVecDialog->ShowModal();
}

void MainFrame::onSaveText(wxCommandEvent& /*event*/)
{
  wxString new_file = wxFileSelector(wxT("Select a file"),filename.GetPath(),
    wxT(""),wxT(""),wxT("*.*"),wxFD_SAVE,this);
  if (!new_file.empty())
  {
    mediator->exportToText(static_cast<std::string>(new_file.fn_str()));
  }
}

void MainFrame::onExit(wxCommandEvent& /*event*/) {
  Close();
}

void MainFrame::onClose(wxCloseEvent &event)
{
  if (settingsDialog != NULL)
  {
    settingsDialog->Destroy();
  }
  if (progDialog != NULL)
  {
    progDialog->Destroy();
  }
  if (savePicDialog != NULL)
  {
    savePicDialog->Destroy();
  }
  if (saveVecDialog != NULL)
  {
    saveVecDialog->Destroy();
  }
  infoDialog->Destroy();
  simDialog->Destroy();
  markDialog->Destroy();
  glCanvas->stopForceDirected();
  event.Skip();
}

void MainFrame::onActivateTool(wxCommandEvent& event) {
  glCanvas->setActiveTool(event.GetId());
}

void MainFrame::onRankStyle(wxCommandEvent& event) {
  mediator->zoomOutTillTop();
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

void MainFrame::onFSMStyle(wxCommandEvent& event){
  mediator->setFSMStyle(event.IsChecked());
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

void MainFrame::onInfo(wxCommandEvent& /*event*/) {
  infoDialog->Show();
}

void MainFrame::onMark(wxCommandEvent& /*event*/)
{
  markDialog->Show();
}

void MainFrame::onSim(wxCommandEvent& /*event*/)
{
  simDialog->Show();
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

void MainFrame::onStartForceDirected(wxCommandEvent& /*event*/) {
  toolMenu->Enable(myID_START_FORCE_DIRECTED,false);
  toolMenu->Enable(myID_STOP_FORCE_DIRECTED,true);
  glCanvas->startForceDirected();
}

void MainFrame::onStopForceDirected(wxCommandEvent& /*event*/) {
  glCanvas->stopForceDirected();
  toolMenu->Enable(myID_START_FORCE_DIRECTED,true);
  toolMenu->Enable(myID_STOP_FORCE_DIRECTED,false);
}

void MainFrame::onResetStatePositions(wxCommandEvent& /*event*/) {
  glCanvas->resetStatePositions();
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
  infoDialog->setLTSInfo(ns,nt,nc,nr);
}

void MainFrame::setMarkedStatesInfo(int number) {
  infoDialog->setNumMarkedStates(number);
}

void MainFrame::setMarkedTransitionsInfo(int number) {
  infoDialog->setNumMarkedTransitions(number);
}

void MainFrame::addMarkRule(wxString str,int mr) {
  markDialog->addMarkRule(str,mr);
}

void MainFrame::replaceMarkRule(wxString str,int mr) {
  markDialog->replaceMarkRule(str,mr);
}
  
void MainFrame::resetMarkRules() {
  markDialog->resetMarkRules();
}

void MainFrame::setActionLabels(vector< string > &labels) {
  markDialog->setActionLabels(labels);
}

void MainFrame::startRendering() {
  SetStatusText(wxT("Rendering..."),0);
  //GetStatusBar()->Update();
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

void MainFrame::resetParameters() {
  infoDialog->resetParameterNames();
}

void MainFrame::resetParameterValues() {
  infoDialog->resetParameterValues();
}

void MainFrame::addParameter(int i,std::string par) {
  infoDialog->setParameterName(i,par);
}

void MainFrame::setParameterValue(int i,std::string value) {
  infoDialog->setParameterValue(i,value);
}

void MainFrame::setParameterValues(int i, std::vector<std::string> values)
{
  infoDialog->setParameterValues(i, values);
}

void MainFrame::setClusterStateNr(int n)
{
  infoDialog->setClusterStateNr(n);
}

void MainFrame::reportError(std::string const& error)
{
  wxString err(error.c_str(), wxConvLocal);

  wxMessageBox(err, wxT("LTSView - An error occured"), wxICON_ERROR|wxOK, this);
}


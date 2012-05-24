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

#include <gl2ps.h>
#include <QDebug>
#include <QFileDialog>
#include <QImageWriter>
#include <QMessageBox>
#include <QString>
#include "infodialog.h"
#include "savepicturedialog.h"
#include "savevectordialog.h"
#include "settingsdialog.h"
#include "simdialog.h"

#include "wx.hpp" // precompiled headers

#include "mainframe.h"
#include <wx/checklst.h>
#include <wx/filedlg.h>
#include <wx/notebook.h>
#include <wx/progdlg.h>
#include "mcrl2/lts/lts_io.h"
#include "glcanvas.h"
#include "icons/main_window.xpm"
#include "ids.h"
#include "markdialog_old.h"
#include "mediator.h"
#include "settings.h"

using namespace std;
using namespace IDs;

// Event table
BEGIN_EVENT_TABLE(MainFrame, wxFrame)
  EVT_MENU(wxID_OPEN, MainFrame::onOpen)
  EVT_MENU(myID_OPEN_TRACE, MainFrame::onOpenTrace)
  EVT_MENU(myID_SAVEPIC, MainFrame::onSavePic)
  EVT_MENU(myID_SAVEVEC, MainFrame::onSaveVec)
  EVT_MENU(myID_SAVETXT, MainFrame::onSaveText)
  EVT_MENU(wxID_EXIT, MainFrame::onExit)
  EVT_MENU(wxID_RESET, MainFrame::onResetView)
  EVT_MENU(myID_DISPLAY_STATES,MainFrame::onDisplay)
  EVT_MENU(myID_DISPLAY_TRANSITIONS,MainFrame::onDisplay)
  EVT_MENU(myID_DISPLAY_BACKPOINTERS,MainFrame::onDisplay)
  EVT_MENU(myID_DISPLAY_WIREFRAME,MainFrame::onDisplay)
  EVT_MENU(wxID_PREFERENCES,MainFrame::onSettings)
  EVT_MENU(myID_INFO,MainFrame::onInfo)
  EVT_MENU(myID_MARK,MainFrame::onMark)
  EVT_MENU(myID_SIM, MainFrame::onSim)
  EVT_MENU(myID_PAN,MainFrame::onActivateTool)
  EVT_MENU(myID_ROTATE,MainFrame::onActivateTool)
  EVT_MENU(myID_SELECT,MainFrame::onActivateTool)
  EVT_MENU(myID_ZOOM,MainFrame::onActivateTool)
  EVT_MENU(myID_ZOOM_IN_ABOVE, MainFrame::onZoomInAbove)
  EVT_MENU(myID_ZOOM_IN_BELOW, MainFrame::onZoomInBelow)
  EVT_MENU(myID_ZOOM_OUT, MainFrame::onZoomOut)

//  EVT_IDLE(MainFrame::onIdle)
  EVT_CLOSE(MainFrame::onClose)
END_EVENT_TABLE()

MainFrame::MainFrame(Mediator* owner, Settings* ss, MarkManager *manager)
  : wxFrame(NULL,wxID_ANY,wxT("LTSView"))
{
//  previousTime = 0.0;
//  frameCount = 0;
  mediator = owner;
  settings = ss;
  progDialog = NULL;
  settingsDialog = new SettingsDialog(0, settings); // TODO: s/0/this/
  infoDialog = new InfoDialog(0); // TODO: s/0/this/
  simDialog = new SimDialog(0); // TODO: s/0/this/
  markDialog = new MarkDialog(this, manager);

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
  simDialog->setSimulation(sim);
}

void MainFrame::setupMenuBar()
{
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
  viewMenu->AppendCheckItem(myID_DISPLAY_STATES, wxT("Display &states\tF3"),
                            wxT("Show/hide individual states"));
  viewMenu->AppendCheckItem(myID_DISPLAY_TRANSITIONS,
                            wxT("Display &transitions\tF4"), wxT("Show/hide individual transitions"));
  viewMenu->AppendCheckItem(myID_DISPLAY_BACKPOINTERS,
                            wxT("Display &backpointers\tF5"), wxT("Show/hide backpointers"));
  viewMenu->AppendCheckItem(myID_DISPLAY_WIREFRAME,
                            wxT("Display &wireframe\tF6"),wxT("Toggle wireframe/surface"));
  viewMenu->AppendSeparator();
  viewMenu->Append(wxID_PREFERENCES,wxT("S&ettings..."),
                   wxT("Show the settings panel"));

  viewMenu->Check(myID_DISPLAY_STATES,settings->displayStates.value());
  viewMenu->Check(myID_DISPLAY_TRANSITIONS,
                  settings->displayTransitions.value());
  viewMenu->Check(myID_DISPLAY_BACKPOINTERS,
                  settings->displayBackpointers.value());
  viewMenu->Check(myID_DISPLAY_WIREFRAME,
                  settings->displayWireframe.value());

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

  helpMenu->Append(wxID_HELP,wxT("&Contents"),wxT("Show help contents"));
  helpMenu->AppendSeparator();
  helpMenu->Append(wxID_ABOUT,wxT("&About"));

  menuBar->Append(fileMenu, wxT("&File"));
  menuBar->Append(viewMenu, wxT("&View"));
  menuBar->Append(toolMenu, wxT("&Tools"));
  menuBar->Append(helpMenu, wxT("&Help"));

  SetMenuBar(menuBar);
}

void MainFrame::setupMainArea()
{
  wxFlexGridSizer* mainSizer = new wxFlexGridSizer(1,1,0,0);
  mainSizer->AddGrowableCol(0);
  mainSizer->AddGrowableRow(0);

  int attribList[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0 };
  glCanvas = new GLCanvas(mediator,this,settings,wxDefaultSize,attribList);

  mainSizer->Add(glCanvas,1,wxALIGN_CENTER|wxEXPAND|wxALL,0);

  mainSizer->Fit(this);
  SetSizer(mainSizer);
  Layout();
}

GLCanvas* MainFrame::getGLCanvas() const
{
  return glCanvas;
}

void MainFrame::setFileInfo(wxFileName fn)
{
  filename.Assign(fn);
}

void MainFrame::onOpen(wxCommandEvent& /*event*/)
{
  wxString filemask = wxString(("All supported files (" +
                                mcrl2::lts::detail::lts_extensions_as_string() +
                                ")|" +
                                mcrl2::lts::detail::lts_extensions_as_string(";") +
                                "|All files (*.*)|*.*").c_str(),
                               wxConvLocal);
  wxFileDialog dialog(this,wxT("Open LTS"),
                      filename.GetPath(),filename.GetFullName(),filemask,wxFD_OPEN);
  dialog.CentreOnParent();
  if (dialog.ShowModal() == wxID_OK)
  {
    filename.Assign(dialog.GetPath());
    mediator->openFile(string(filename.GetFullPath().mb_str(wxConvUTF8)));
  }
}

void MainFrame::onOpenTrace(wxCommandEvent& /*event*/)
{
  wxString filemask = wxT("Traces (*.trc)|*.trc|All files (*.*)|*.*");
  wxFileDialog dialog(this, wxT("Open Trace"),
                      filename.GetPath(), wxEmptyString,filemask,wxFD_OPEN);
  dialog.CentreOnParent();
  if (dialog.ShowModal() == wxID_OK)
  {
    std::string path(dialog.GetPath().mb_str());
    mediator->loadTrace(path);
  }
}

void MainFrame::onSavePic(wxCommandEvent& /*event*/)
{
  QString filter = "Images (";
  QList<QByteArray> formats = QImageWriter::supportedImageFormats();
  for(QList<QByteArray>::iterator i = formats.begin(); i != formats.end(); i++)
  {
    if(i != formats.begin())
    {
      filter += " ";
    }
    filter += "*." + *i;
  }
  filter += ")";

  // TODO: s/0/this/
  QString filename = QFileDialog::getSaveFileName(0, "Save picture", QString(), filter);
  if (filename.isNull())
  {
    return;
  }

  // TODO: s/0/this/
  SavePictureDialog dialog(0, glCanvas, filename);
  connect(&dialog, SIGNAL(statusMessage(QString)), this, SLOT(setStatusBar(QString)));
  dialog.exec();
  setStatusBar("");
}

void MainFrame::onSaveVec(wxCommandEvent& /*event*/)
{
  QString all = "All supported files (*.ps *.eps *.pdf *.svg)";
  QString ps = "PostScript (*.ps)";
  QString eps = "Encapsulated PostScript (*.eps)";
  QString pdf = "Portable Document Format (*.pdf)";
  QString svg = "Scalable Vector Graphics (*.svg)";

  QString filter = all + ";;" + ps + ";;" + eps + ";;" + pdf + ";;" + svg;
  QString selectedFilter;
  // TODO: s/0/this/
  QString filename = QFileDialog::getSaveFileName(0, "Save vector", QString(), filter, &selectedFilter);
  if (filename.isNull())
  {
    return;
  }

  GLint format;
  if (selectedFilter == ps || (selectedFilter == all && filename.right(3).toLower() == ".ps"))
  {
    format = GL2PS_PS;
  }
  else if (selectedFilter == eps || (selectedFilter == all && filename.right(4).toLower() == ".eps"))
  {
    format = GL2PS_EPS;
  }
  else if (selectedFilter == pdf || (selectedFilter == all && filename.right(4).toLower() == ".pdf"))
  {
    format = GL2PS_PDF;
  }
  else if (selectedFilter == svg || (selectedFilter == all && filename.right(4).toLower() == ".svg"))
  {
    format = GL2PS_SVG;
  }
  else
  {
    // TODO: s/0/this/
    QMessageBox::critical(0, "Error writing file", "Saving picture failed: unsupported file format.");
  }

  // TODO: s/0/this/
  SaveVectorDialog dialog(0, glCanvas, filename, format);
  connect(&dialog, SIGNAL(statusMessage(QString)), this, SLOT(setStatusBar(QString)));
  dialog.exec();
  setStatusBar("");
}

void MainFrame::onSaveText(wxCommandEvent& /*event*/)
{
  wxString new_file = wxFileSelector(wxT("Select a file"),filename.GetPath(),
                                     wxT(""),wxT(""),wxT("*.*"),wxFD_SAVE,this);
  if (!new_file.empty())
  {
    mediator->exportToText(static_cast<std::string>(new_file.mb_str(wxConvUTF8)));
  }
}

void MainFrame::onExit(wxCommandEvent& /*event*/)
{
  Close();
}

void MainFrame::onClose(wxCloseEvent& event)
{
  settingsDialog->hide();
  if (progDialog != NULL)
  {
    progDialog->Destroy();
  }
  infoDialog->hide();
  simDialog->hide();
  markDialog->Destroy();
  event.Skip();
}

void MainFrame::onActivateTool(wxCommandEvent& event)
{
  glCanvas->setActiveTool(event.GetId());
}

void MainFrame::onResetView(wxCommandEvent& /*event*/)
{
  glCanvas->resetView();
}

void MainFrame::onDisplay(wxCommandEvent& event)
{
  if (event.GetId() == myID_DISPLAY_STATES)
  {
    settings->displayStates.setValue(event.IsChecked());
  }
  else if (event.GetId() == myID_DISPLAY_TRANSITIONS)
  {
    settings->displayTransitions.setValue(event.IsChecked());
  }
  else if (event.GetId() == myID_DISPLAY_BACKPOINTERS)
  {
    settings->displayBackpointers.setValue(event.IsChecked());
  }
  else if (event.GetId() == myID_DISPLAY_WIREFRAME)
  {
    settings->displayWireframe.setValue(event.IsChecked());
  }
  else
  {
    return;
  }
  glCanvas->display();
}

void MainFrame::onSettings(wxCommandEvent& /*event*/)
{
  settingsDialog->show();
}

void MainFrame::onInfo(wxCommandEvent& /*event*/)
{
  infoDialog->show();
}

void MainFrame::onMark(wxCommandEvent& /*event*/)
{
  markDialog->Show();
}

void MainFrame::onSim(wxCommandEvent& /*event*/)
{
  simDialog->show();
}

void MainFrame::onZoomInBelow(wxCommandEvent& /*event*/)
{
  mediator->zoomInBelow();
  glCanvas->display();
}

void MainFrame::onZoomInAbove(wxCommandEvent& /*event*/)
{
  mediator->zoomInAbove();
  glCanvas->display();
}

void MainFrame::onZoomOut(wxCommandEvent& /*event*/)
{
  mediator->zoomOut();
  glCanvas->display();
}

void MainFrame::createProgressDialog(const string& title,const string& text)
{
  progDialog = new wxProgressDialog(wxString(title.c_str(),wxConvUTF8),
                                    wxString(text.c_str(),wxConvUTF8),100,this,
                                    wxPD_APP_MODAL|wxPD_AUTO_HIDE);
  progDialog->SetMinSize(wxSize(400,100));
  progDialog->SetSize(wxSize(400,100));
  progDialog->CentreOnParent();
  progDialog->Update(0,wxString(text.c_str(),wxConvUTF8));
  progDialog->Show();
}

void MainFrame::updateProgressDialog(int val,string msg)
{
  if (progDialog != NULL)
  {
    progDialog->Update(val,wxString(msg.c_str(),wxConvUTF8));
    if (val == 100)
    {
      progDialog->Close();
      progDialog->Destroy();
      progDialog = NULL;
    }
  }
}

void MainFrame::showMessage(string title,string text)
{
  wxMessageDialog* msgDialog = new wxMessageDialog(this,
      wxString(text.c_str(),wxConvLocal),wxString(title.c_str(),wxConvLocal),
      wxOK);
  msgDialog->ShowModal();
  msgDialog->Close();
  msgDialog->Destroy();
}

void MainFrame::loadTitle()
{
  wxString fn = filename.GetFullName();
  if (fn != wxEmptyString)
  {
    SetTitle(fn + wxT(" - LTSView"));
  }
  else
  {
    SetTitle(wxT("LTSView"));
  }
}

void MainFrame::setNumberInfo(int ns,int nt,int nc,int nr)
{
  infoDialog->setLTSInfo(ns,nt,nc,nr);
}

void MainFrame::setMarkedStatesInfo(int number)
{
  infoDialog->setNumMarkedStates(number);
}

void MainFrame::setMarkedTransitionsInfo(int number)
{
  infoDialog->setNumMarkedTransitions(number);
}

void MainFrame::resetMarkRules()
{
  markDialog->resetMarkRules();
}

void MainFrame::setActionLabels(vector< string > &labels)
{
  markDialog->setActionLabels(labels);
}

void MainFrame::startRendering()
{
  SetStatusText(wxT("Rendering..."),0);
  //GetStatusBar()->Update();
}

void MainFrame::stopRendering()
{
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

void MainFrame::resetParameterValues()
{
  infoDialog->resetParameterValues();
}

void MainFrame::setParameterNames(QStringList parameters)
{
  infoDialog->setParameterNames(parameters);
}

void MainFrame::setParameterValue(int parameter, QString value)
{
  infoDialog->setParameterValue(parameter, value);
}

void MainFrame::setParameterValues(int parameters, QStringList values)
{
  infoDialog->setParameterValues(parameters, values);
}

void MainFrame::setStatesInCluster(int n)
{
  infoDialog->setStatesInCluster(n);
}


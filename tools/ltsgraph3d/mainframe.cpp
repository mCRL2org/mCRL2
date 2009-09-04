// Author(s): Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mainframe.cpp
/// \brief Main window implementation.

#include "wx.hpp" // precompiled headers

#include <memory>
#include "mainframe.h"
#include "ids.h"
#include <wx/menu.h>
#include <wx/filename.h>
#include "export_svg.h"
#include "export_xml.h"
#include "export_latex.h"
#include <mcrl2/lts/lts.h>

// For compatibility with older wxWidgets versions (pre 2.8)
#if (wxMINOR_VERSION < 8)
# define wxFD_SAVE wxSAVE
# define wxFD_OPEN wxOPEN
# define wxFD_CHANGE_DIR wxCHANGE_DIR
# define wxFD_OVERWRITE_PROMPT wxOVERWRITE_PROMPT
#endif

using namespace IDS;

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
  EVT_MENU(wxID_OPEN, MainFrame::onOpen)
  EVT_MENU(myID_IMPORT, MainFrame::onImport)
  EVT_MENU(myID_MENU_EXPORT, MainFrame::onExport)
  EVT_MENU(wxID_EXIT, MainFrame::onQuit)
  EVT_CLOSE(MainFrame::onClose)
  EVT_MENU(myID_TOGGLE_POSITIONING, MainFrame::onTogglePositioning)
  EVT_MENU(myID_TOGGLE_VECTOR, MainFrame::onToggleVector)
  EVT_MENU(myID_TOGGLE_3D, MainFrame::onToggle3D)
  EVT_MENU(myID_DLG_INFO, MainFrame::onInfo)
  EVT_MENU(myID_DLG_ALGO, MainFrame::onAlgo)
  EVT_MENU(wxID_PREFERENCES, MainFrame::onSettings)
  EVT_MENU(wxID_PREFERENCES, MainFrame::onSettings)
  EVT_MENU(myID_SELECT, MainFrame::onSelect)
  EVT_MENU(myID_COLOUR, MainFrame::onColour)
  EVT_MENU(myID_ROTATE, MainFrame::onMode)
  EVT_MENU(myID_PAN, MainFrame::onMode)
  EVT_MENU(myID_ZOOM, MainFrame::onMode)
  EVT_MENU(myID_NONE, MainFrame::onMode)
  EVT_MENU(myID_RESET_ALL, MainFrame::onResetAll)
  EVT_MENU(myID_RESET_ROTATE, MainFrame::onResetRot)
  EVT_MENU(myID_RESET_PAN, MainFrame::onResetPan)
  EVT_MENU(myID_SHOW_SYSTEM, MainFrame::onShowSystem)
END_EVENT_TABLE()


MainFrame::MainFrame(LTSGraph3d* owner)
  : wxFrame(NULL, wxID_ANY, wxT("LTSGraph3d"),
    wxDefaultPosition, wxDefaultSize,
    wxDEFAULT_FRAME_STYLE|wxFULL_REPAINT_ON_RESIZE)
{
  app = owner;

  algoDlg = new AlgoDialog(app, this);
  settingsDlg = new SettingsDialog(app, this);
  infoDlg = new InfoDialog(this);

  setupMenuBar();
  setupMainArea();

  SetSize(800, 600);
  CentreOnScreen();

  algoDlg->CentreOnParent();
  infoDlg->CentreOnParent();
}

void MainFrame::setupMenuBar()
{
  wxMenuBar* menuBar = new wxMenuBar;

  // File menu
  wxMenu* fileMenu = new wxMenu;
  fileMenu->Append(wxID_OPEN, wxT("&Open...\tCTRL-o"),
               wxT("Read an LTS from a file."));
  fileMenu->Append(myID_IMPORT, wxT("&Import...\tCTRL-i"),
      wxT("Read an LTS Layout from a file."));
  fileMenu->Append(myID_MENU_EXPORT, wxT("E&xport to...\tCTRL-x"),
               wxT("Export this LTS to file."));

  fileMenu->Append(wxID_EXIT, wxT("&Quit \tCTRL-q"), wxT("Quit LTSGraph3d."));

  // View menu
  wxMenu* viewMenu = new wxMenu;
  viewMenu->Append(myID_TOGGLE_3D, wxT("Toggle the ltsgraph version (3d or 2d) \tCTRL-s"), wxT("Changes the ltsgraph version"));
  viewMenu->Append(myID_RESET_ALL, wxT("Reset viewpoint \tF2"), wxT("Resets any panning and rotations done."));
  viewMenu->Append(myID_RESET_ROTATE, wxT("Reset rotations \tCTRL-r"), wxT("Resets any ratations done"));
  viewMenu->Append(myID_RESET_PAN, wxT("Reset pannings \tCTRL-d"), wxT("Resets and panning done"));
  viewMenu->AppendSeparator();
  viewMenu->Append(myID_SHOW_SYSTEM, wxT("Display coord&inate system \tI"), wxT("Displays a coordinate system at the left below corner of the window."));


  // Tools menu
  wxMenu* toolsMenu = new wxMenu;
  toolsMenu->AppendRadioItem(myID_SELECT, wxT("Select\tS"), wxT("Select tool"));
  toolsMenu->AppendRadioItem(myID_COLOUR, wxT("Colour \tC"), wxT("Colouring tool"));
  toolsMenu->AppendSeparator();
  toolsMenu->AppendRadioItem(myID_ZOOM, wxT("Zoom \tA"), wxT("Zoom tool"));
  toolsMenu->AppendRadioItem(myID_PAN, wxT("Pan\tD"), wxT("Pan tool"));
  toolsMenu->AppendRadioItem(myID_ROTATE, wxT("Rotate \tF"), wxT("Rotate tool"));
  toolsMenu->AppendRadioItem(myID_NONE, wxT("Disable \tN"), wxT("Disable tool"));
  toolsMenu->AppendSeparator();
  toolsMenu->Append(myID_TOGGLE_VECTOR, wxT("Toggle state &vector display \tCTRL-V"));
  toolsMenu->Append(myID_TOGGLE_POSITIONING, wxT("Toggle optimisation... \tCTRL-T"),
                    wxT("Activates or deactivates the layout optimisation algorithm."));
  toolsMenu->AppendSeparator();
  toolsMenu->Append(myID_DLG_ALGO, wxT("O&ptimization... \tCTRL-p"),
                    wxT("Display dialog for layout optimization algorithm."));
  toolsMenu->Append(wxID_PREFERENCES, wxT("Settings..."),
                   wxT("Display the visualization settings dialog."));
  toolsMenu->Append(myID_DLG_INFO, wxT("&Information... \tCTRL-n"),
                    wxT("Display dialog with information about this LTS."));

  // Help menu
  wxMenu* helpMenu = new wxMenu;
  helpMenu->Append(wxID_HELP, wxT("&Contents"), wxT("Show help contents"));
  helpMenu->AppendSeparator();
  helpMenu->Append(wxID_ABOUT, wxT("&About"));


  menuBar->Append(fileMenu, wxT("&File"));
  menuBar->Append(viewMenu, wxT("&View"));
  menuBar->Append(toolsMenu, wxT("&Tools"));
  menuBar->Append(helpMenu, wxT("&Help"));

  SetMenuBar(menuBar);
}

void MainFrame::setupMainArea()
{
  wxFlexGridSizer* mainSizer = new wxFlexGridSizer(1,1,0,0);
  mainSizer->AddGrowableCol(0);
  mainSizer->AddGrowableRow(0);

  int attribList[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, 0 };
  glCanvas = new GLCanvas(app, this, wxDefaultSize, attribList);

  mainSizer->Add(glCanvas, 1, wxALIGN_CENTER|wxEXPAND|wxALL, 0);

  mainSizer->Fit(this);
  SetSizer(mainSizer);
  Layout();
}

void MainFrame::onOpen(wxCommandEvent& /*event*/)
{
  wxFileDialog dialog(this, wxT("Select a file"), wxEmptyString, wxEmptyString,
    wxString(("All supported formats ("+
             mcrl2::lts::lts::lts_extensions_as_string()+
             ")|"+
             mcrl2::lts::lts::lts_extensions_as_string(";")+
             "|All files (*.*)|*.*").c_str(),
             wxConvLocal),
    wxFD_OPEN|wxFD_CHANGE_DIR);

  if (dialog.ShowModal() == wxID_OK)
  {
    wxString path = dialog.GetPath();
    std::string stPath(path.fn_str());

    app->getAlgorithm(0)->stop();
    app->openFile(stPath);
  }
}
void MainFrame::onSelect(wxCommandEvent& /*event*/) {
  app->setTool(false);
}

void MainFrame::onColour(wxCommandEvent& /*event*/) {
  app->setTool(true);
}

void MainFrame::onImport(wxCommandEvent& /*event*/)
{
  wxFileDialog dialog(this, wxT("Select a layout file"), wxEmptyString, wxEmptyString,
    wxT("XML layout file (*.xml)|*.xml|All files (*.*)|*.*"),
    wxFD_OPEN|wxFD_CHANGE_DIR);

  if (dialog.ShowModal() == wxID_OK)
  {
    wxString path = dialog.GetPath();
    std::string stPath(path.fn_str());

    app->getAlgorithm(0)->stop();
    app->openFile(stPath);
  }
}

void MainFrame::onExport(wxCommandEvent& /*event*/)
{
  bool startExport = false;
  wxString caption = wxT("Export layout as");
  wxString wildcard = wxT("Scalable Vector Graphics (*.svg)|*.svg|XML Layout file (*.xml)|*.xml|LaTeX TikZ drawing (*.tex)|*.tex");
  wxString defaultDir = wxEmptyString; // Empty string -> cwd

  wxString defaultFileName(app->getFileName().c_str(), wxConvLocal);

  // Strip extension from filename
  if(defaultFileName.Find('.') != -1 )
  {
    defaultFileName = defaultFileName.BeforeLast('.');
  }

  wxFileDialog dialog(this, caption, defaultDir, defaultFileName, wildcard,
                      wxFD_SAVE | wxFD_CHANGE_DIR);
  std::auto_ptr< Exporter > exporter;
  wxString fileName;

  while(!startExport)
  {
    if(dialog.ShowModal() == wxID_CANCEL)
    {
      return;
    }

    fileName = dialog.GetPath();
    wxString extension = fileName.AfterLast('.');
    if(extension == fileName)
    {
      // No extension given, get it from the filter index
      switch(dialog.GetFilterIndex())
      {
        case 0: // SVG item
          fileName.Append(wxT(".svg"));
          exporter.reset(new ExporterSVG(app->getGraph(), app));
          break;
        case 1: // XML item
          fileName.Append(wxT(".xml"));
          exporter.reset(new ExporterXML(app->getGraph()));
          break;
        case 2: // TeX item
          fileName.Append(wxT(".tex"));
          exporter.reset(new ExporterLatex(app->getGraph(), app));
          break;
      }
    }
    else
    {
      // An extension was given
      if (extension == wxT("svg"))
      {
        exporter.reset(new ExporterSVG(app->getGraph(), app));
      }
      else if (extension == wxT("xml"))
      {
        exporter.reset(new ExporterXML(app->getGraph()));
      }
      else if (extension == wxT("tex"))
      {
        exporter.reset(new ExporterLatex(app->getGraph(), app));
      }
      else
      {
        // Unknown file format, export to xml
        exporter.reset(new ExporterXML(app->getGraph()));
      }
    }
    // Check if the file exsits
    wxFileName fn(fileName);
    if(fn.FileExists()) {
      wxMessageDialog fileExistDialog(
        this,
        wxT("The file ") + fileName + wxT(" already exists, overwrite?\n"),
        wxT("File exists"),
        wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION);
      if(fileExistDialog.ShowModal() == wxID_YES) {
        startExport = true;
      }
    }
    else {
      startExport = true;
    }
  }

  if(exporter->export_to(fileName))
  {
    wxMessageDialog msgDlg(
      this,
      wxT("The layout was exported to file:\n\n") + fileName,
      wxT("Layout exported"),
      wxOK | wxICON_INFORMATION);
    msgDlg.ShowModal();
  }
}

void MainFrame::onQuit(wxCommandEvent& e)
{
  app->getAlgorithm(0)->stop();
  Close(true);
}

void MainFrame::onClose(wxCloseEvent& e)
{
  app->getAlgorithm(0)->stop();
  Destroy();
}

GLCanvas* MainFrame::getGLCanvas()
{
  return glCanvas;
}

void MainFrame::onTogglePositioning(wxCommandEvent&)
{
  app->getAlgorithm(0)->toggle();
}

void MainFrame::onToggleVector(wxCommandEvent&)
{
  app->toggleVectorSelected();
}

void MainFrame::onInfo(wxCommandEvent& /* event */)
{
  infoDlg->Show();
}

void MainFrame::onAlgo(wxCommandEvent& /* event */)
{
  algoDlg->Show();
}

void MainFrame::onSettings(wxCommandEvent& /* event */)
{
  settingsDlg->Show();
}


void MainFrame::setLTSInfo(int is, int ns, int nt, int nl)
{
  infoDlg->setLTSInfo(is, ns, nt, nl);

  wxString title(app->getFileName().c_str(), wxConvLocal);
  title = wxT("LTSGraph3d - ") + title;
  SetTitle(title);
}

void MainFrame::onResetAll(wxCommandEvent& /*evt*/)
{
	glCanvas->ResetAll();
	glCanvas->display();
}

void MainFrame::onResetRot(wxCommandEvent& /*evt*/)
{
	glCanvas->ResetRot();
	glCanvas->display();
}

void MainFrame::onResetPan(wxCommandEvent& /*evt*/)
{
	glCanvas->ResetPan();
	glCanvas->display();
}

void MainFrame::onMode(wxCommandEvent& event)
{
	glCanvas->setMode(event.GetId());
	glCanvas->display();
}

void MainFrame::onShowSystem(wxCommandEvent& /*evt*/)
{
	glCanvas->showSystem();
	glCanvas->display();
}

void MainFrame::onToggle3D(wxCommandEvent& /*evt*/)
{
	if(glCanvas->get3D())
		app->forceWalls();
	glCanvas->changeDrawMode();
	glCanvas->display();
}

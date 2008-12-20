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
#include <mcrl2/utilities/about_dialog.h>

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
  EVT_MENU(myID_DLG_INFO, MainFrame::onInfo)
  EVT_MENU(myID_DLG_ALGO, MainFrame::onAlgo)
  EVT_MENU(wxID_PREFERENCES, MainFrame::onSettings)
  EVT_MENU(wxID_PREFERENCES, MainFrame::onSettings)
  EVT_MENU(wxID_ABOUT, MainFrame::onAbout)
  EVT_MENU(wxID_HELP, MainFrame::onHelpContents)
  EVT_MENU(myID_TOOL_SELECT, MainFrame::onSelect)
  EVT_MENU(myID_COLOUR, MainFrame::onColour)
END_EVENT_TABLE()


MainFrame::MainFrame(LTSGraph* owner)
  : wxFrame(NULL, wxID_ANY, wxT("LTSGraph"),
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

  fileMenu->Append(wxID_EXIT, wxT("&Quit \tCTRL-q"), wxT("Quit LTSGraph."));

  // Tools menu
  wxMenu* toolsMenu = new wxMenu;
  toolsMenu->AppendRadioItem(
    myID_TOOL_SELECT,wxT("&Select\tS"),wxT("Select tool"));
  toolsMenu->AppendRadioItem(myID_COLOUR,
    wxT("&Colour\tC"),wxT("Colouring tool"));
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

void MainFrame::onHelpContents( wxCommandEvent& /* event */ )
{
  wxLaunchDefaultBrowser(wxT("http://www.mcrl2.org/wiki/index.php/User_manual/ltsgraph"));
}

void MainFrame::onAbout(wxCommandEvent& /*event*/) {
  std::vector<std::string> developers;
  developers.push_back("Carst Tankink");
  mcrl2::utilities::wx::about_dialog("LTSGraph",
    "Tool for visualizing a labelled transition systems as a graph, and optimizing graph layout.",
    developers
  );
}

void MainFrame::onQuit(wxCommandEvent& /*event */)
{
  app->getAlgorithm(0)->stop();
  Close(TRUE);
}
GLCanvas* MainFrame::getGLCanvas()
{
  return glCanvas;
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
  title = wxT("LTSGraph - ") + title;
  SetTitle(title);
}


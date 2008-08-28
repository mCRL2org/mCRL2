#include "mainframe.h"
#include "ids.h"
#include <wx/menu.h>

using namespace IDS;

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
  EVT_MENU(wxID_OPEN, MainFrame::onOpen)
  EVT_MENU(wxID_EXIT, MainFrame::onQuit)
  EVT_MENU(myID_DLG_INFO, MainFrame::onInfo)
  EVT_MENU(myID_DLG_ALGO, MainFrame::onAlgo)
END_EVENT_TABLE()


MainFrame::MainFrame(GLTSGraph* owner)
  : wxFrame(NULL, wxID_ANY, wxT("GLTSGraph"))
{
  app = owner;

  int attribList[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, 0 };
  glCanvas = new GLCanvas(app, this, wxDefaultSize, attribList);
  
  algoDlg = new AlgoDialog(app, this);
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
  fileMenu->Append(myID_MENU_EXPORT, wxT("E&xport to...\tCTRL-x"), 
               wxT("Export this LTS to file."));

  fileMenu->Append(wxID_EXIT, wxT("&Quit \tCTRL-q"), wxT("Quit GLTSGraph."));
  
  // View menu
  wxMenu* viewMenu = new wxMenu;
  wxMenuItem* displayState = viewMenu->Append(myID_DISPLAY_STATE_LBL,
               wxT("Display state labels"),
               wxT("Toggle the display of state labels."), wxITEM_CHECK);
  displayState->Check();

  wxMenuItem* displayTrans = viewMenu->Append(myID_DISPLAY_TRANS_LBL,
                wxT("Display transition labels"),
                wxT("Toggle the display of transitions labels."), wxITEM_CHECK);
  displayTrans->Check();

  viewMenu->Append(wxID_PREFERENCES, wxT("Settings..."), 
                   wxT("Display the settings dialog."));
  // Tools menu
  wxMenu* toolsMenu = new wxMenu;
  toolsMenu->Append(myID_DLG_INFO, wxT("&Information... \tCTRL-i"), 
                    wxT("Display dialog with information about this LTS."));
  toolsMenu->Append(myID_DLG_ALGO, wxT("O&ptimization... \tCTRL-p"),
                    wxT("Display dialog for layout optimization algorithm."));

 
  // Help menu
  wxMenu* helpMenu = new wxMenu;
  helpMenu->Append(wxID_ABOUT, wxT("&About..."), wxEmptyString);


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
    wxT("All supported formats(*.ltsgraph;*.aut;*.svc)|*.ltsgraph;*.aut;*.svc|Position data (*.ltsgraph)|*.ltsgraph|LTS format (*.aut; *.svc)|*.aut;*.svc|All files (*.*)|*.*"));
  
  if (dialog.ShowModal() == wxID_OK)
  {
    wxString path = dialog.GetPath();
    std::string stPath(path.fn_str()); 

    app->openFile(stPath);
  }
}

void MainFrame::onQuit(wxCommandEvent& /*event */)
{
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

void MainFrame::setLTSInfo(int is, int ns, int nt, int nl)
{
  infoDlg->setLTSInfo(is, ns, nt, nl);
}


// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mac_installer.cpp
/// \brief The MacOS-X installer for exporting symbolic links.

#include "wx/wx.h" 
#include <wx/filepicker.h>
#include <wx/statline.h>

#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

#include "mcrl2/utilities/basename.h"

using namespace std;
using namespace mcrl2::utilities;

std::string install_prefix;

class MacOSXInstaller: public wxApp
{
    virtual bool OnInit();
};

class MacOSXInstallerFrame: public wxFrame
{
public:

    MacOSXInstallerFrame(const wxString& title, const wxPoint& pos);

    void OnQuit(wxCommandEvent& event);
    void OnExport(wxCommandEvent& event);
    wxDirPickerCtrl* dpc;
};

enum
{
    ID_Quit = 1,
    ID_Ok,
    ID_DIR_CRTL,
};

IMPLEMENT_APP_NO_MAIN(MacOSXInstaller)

int main(int argc, char **argv)
{
  if(argc > 1)
    install_prefix = argv[1];
  else
    install_prefix = "/";

  return wxEntry(argc, argv);
}

bool MacOSXInstaller::OnInit()
{
    MacOSXInstallerFrame *frame = new MacOSXInstallerFrame( wxT("Exporter for mCRL2 symbolic links"), wxPoint(50, 50));
    frame->Show(true);
    SetTopWindow(frame);
    return true;
} 

MacOSXInstallerFrame::MacOSXInstallerFrame(const wxString& title, const wxPoint& pos)
: wxFrame( NULL, -1, title, pos, wxSize(550, 250), wxDEFAULT_FRAME_STYLE & ~ (wxRESIZE_BORDER | wxRESIZE_BOX | wxMAXIMIZE_BOX) )
{
    wxPanel *panel = new wxPanel(this, wxID_ANY, wxPoint(15,10), wxSize( 520  ,160), wxBORDER_SUNKEN );

    wxFont font(15, wxDEFAULT, wxNORMAL, wxBOLD);
    wxStaticText *heading = new wxStaticText(panel, wxID_ANY, wxT("Select a directory to export symbolic links for individual tools."), wxPoint(15, 15));
    heading->SetFont(font);

    new wxStaticText(panel, wxID_ANY, wxT("This is necessary to use the tools outside the SquaDT environment."), wxPoint(15, 40));

    new wxStaticText(panel, wxID_ANY, wxT("For command-line tools this directory should be added to the PATH variable."), wxPoint(15, 60));
 
    new wxStaticText(panel, wxID_ANY, wxT("Press Cancel to skip installation of symbolic links."), wxPoint(15, 80));

    struct stat st;
    wxString install;
    if(stat(install_prefix.c_str() ,&st) == 0)
    {
      install = wxT(install_prefix.c_str());
    } else {
      install = wxT("/");
    }

    dpc = new wxDirPickerCtrl(panel, ID_DIR_CRTL,
    install, wxT("Pick a Directory"),
    wxPoint(75,110), wxDefaultSize,
      wxDIRP_USE_TEXTCTRL 
    );
    dpc->SetSize( wxSize( 390 , 20 ));

    wxButton *okbtn = new wxButton(this, ID_Ok, wxT("Ok"), 
      wxPoint(465, 190));
    okbtn -> SetDefault();

    new wxButton(this, ID_Quit, wxT("Cancel"), 
      wxPoint(15, 190));

    Connect(ID_Ok, wxEVT_COMMAND_BUTTON_CLICKED, 
      wxCommandEventHandler(MacOSXInstallerFrame::OnExport));
    Connect(ID_Quit, wxEVT_COMMAND_BUTTON_CLICKED, 
      wxCommandEventHandler(MacOSXInstallerFrame::OnQuit));

    Centre();
}

void MacOSXInstallerFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE);
}

void MacOSXInstallerFrame::OnExport(wxCommandEvent& WXUNUSED(event))
{
  //Get basename for mac_installer
  mcrl2::utilities::basename basename;
  string basename_str = basename.get_executable_basename();

  //Get target directory for putting symbolic links 
  string export_path = dpc->GetPath().mb_str(wxConvUTF8);

  //Test for write access
  if( access( export_path.c_str(), W_OK ) == -1 ){
    wxMessageDialog *dial = new wxMessageDialog(NULL, 
      wxT("Cannot write in supplied directory. Please select another path."), wxT("Error"), wxOK | wxICON_ERROR);
    dial->ShowModal();
    return;
  }

  //If install_tools exists, place symbolic links in target directory and exit
  string line;
  ifstream myfile ("../share/installer/install_tools");
  
  int cntr = 0; 
  if (myfile.is_open())
  {
    while (! myfile.eof() )
    { 
      cntr++;
      getline (myfile,line);
      string exec_str("ln -s " + basename_str + "/" + line + " " + export_path ); 
      system( exec_str.c_str() );
    }
    myfile.close();

    wxMessageDialog *dial = new wxMessageDialog(NULL,
      wxString::Format(wxT("Created %d symbolic links"), cntr ), wxT("Info"), wxOK);
    dial->ShowModal();
    Close(TRUE);
  }

  else{ 
    wxMessageDialog *dial = new wxMessageDialog(NULL, 
      wxT("Error loading file:\n../share/installer/install_tools"), wxT("Error"), wxOK | wxICON_ERROR);
    dial->ShowModal();
  }
}

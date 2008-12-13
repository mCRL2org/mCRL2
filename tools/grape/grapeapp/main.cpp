// Author(s): VitaminB100
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file main.cpp
//
// Main file of the GraPE application

#include <wx/wx.h>
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2gen/mcrl2gen.h"
#include <wx/aboutdlg.h>
#include "grape_frame.h"

#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/utilities/command_line_wx.h"

#include <iostream>

#define NAME   "GraPE"
#define AUTHOR "VitaminB100"

/**
 * \brief toolset version string
 **/
using namespace grape::grapeapp;
using namespace grape::mcrl2gen;
using namespace mcrl2::core;

wxAboutDialogInfo get_about_info() {
  using mcrl2::utilities::interface_description;

  std::string copyright_message(
    interface_description("", NAME, AUTHOR, "", "").copyright_message());

  wxAboutDialogInfo info;

  info.SetName(wxT(NAME));
  info.SetVersion(wxString(mcrl2::utilities::version_tag().c_str(), wxConvLocal));
  info.SetDescription(wxT("This is a Graphical Process Editor for the mCRL2 language.\nThis program is part of the mCRL2 toolkit"));
  info.SetCopyright(wxString(copyright_message.c_str(), wxConvLocal));
  info.SetWebSite(wxT("http://www.mcrl2.org/" ));
  info.AddDeveloper(wxT( "Remco Blewanus" ));
  info.AddDeveloper(wxT( "Thorstin Crijns" ));
  info.AddDeveloper(wxT( "Diana Koenraadt" ));
  info.AddDeveloper(wxT( "Bas Luksenburg" ));
  info.AddDeveloper(wxT( "Jonathan Nelisse" ));
  info.AddDeveloper(wxT( "Hans Poppelaars" ));
  info.AddDeveloper(wxT( "Bram Schoenmakers" ));
  info.AddDocWriter(wxT( "Hans Poppelaars" ));

  return info;
}

class grape_app: public mcrl2::utilities::wx::tool< grape_app >
{
  friend class mcrl2::utilities::wx::tool< grape_app >;

  private:

    std::string parse_error;

    // the filename is the first parameter
    wxString    filename;

    bool parse_command_line(int& argc, wxChar** argv);

  public:

    bool DoInit();
    void show_window();
};

void grape_app::show_window()
{

}

bool grape_app::DoInit()
{
  try
  {
    grape_frame *frame = new grape_frame( filename );
    SetTopWindow(frame);

    if (!parse_error.empty()) {
      wxMessageDialog(GetTopWindow(), wxString(parse_error.c_str(), wxConvLocal),
                         wxT("Command line parsing error"), wxOK|wxICON_ERROR).ShowModal();
    }

    wxInitAllImageHandlers();
  }
  catch (std::exception& e) // parsing not successful
  {
    std::cerr << e.what() << std::endl;
  }

  return true;
}

bool grape_app::parse_command_line(int& argc, wxChar** argv) {
  using namespace mcrl2::utilities;

  interface_description clinterface(std::string(wxString(static_cast< wxChar** > (argv)[0], wxConvLocal).fn_str()),
      NAME, AUTHOR, "[OPTION]... [INFILE]",
      "Graphical editing environment for mCRL2 process specifications. "
      "If INFILE is supplied, it is loaded as a GraPE specification."); 

  command_line_parser parser(clinterface, argc, static_cast< wxChar** > (argv));
 
  if (parser.continue_execution()) {
    if (0 < parser.arguments.size()) {
      filename = wxString(parser.arguments[0].c_str(), wxConvLocal);
    }
    if (1 < parser.arguments.size()) {
      parser.error("too many file arguments");
    }
  }

  return parser.continue_execution();
}

#ifdef __WINDOWS__
extern "C" int WINAPI WinMain(HINSTANCE hInstance,                    
                                  HINSTANCE hPrevInstance,                
                                  wxCmdLineArgType lpCmdLine,             
                                  int nCmdShow) {                                                                     

  MCRL2_ATERM_INIT(0, lpCmdLine);
  gsSetVerboseMsg();
  return wxEntry(hInstance, hPrevInstance, lpCmdLine, nCmdShow);  
}
#endif
 
int main(int argc, char** argv)
{
  init_mcrl2libs(argc, argv);
  return wxEntry(argc, argv);
}

IMPLEMENT_APP_NO_MAIN(grape_app)

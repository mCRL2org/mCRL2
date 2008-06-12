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
#include "../mcrl2gen.h"
#include <wx/aboutdlg.h>
#include "grape_frame.h"

#include "mcrl2/utilities/command_line_interface.h"

#include <iostream>

#define NAME   "GraPE"
#define AUTHOR "VitaminB100"

/**
 * \brief toolset version string
 **/
using namespace grape::grapeapp;
using namespace grape::mcrl2gen;

wxAboutDialogInfo get_about_info() {
  using mcrl2::utilities::interface_description;

  std::string copyright_message(
    interface_description("", NAME, AUTHOR, "", "").copyright_message());

  wxAboutDialogInfo info;

  info.SetName(wxT(NAME));
  info.SetVersion(wxString(mcrl2::utilities::version_tag.c_str(), wxConvLocal));
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

class grape_app: public wxApp
{
  virtual bool OnInit();
  void show_window();
};

void grape_app::show_window()
{

}

bool grape_app::OnInit()
{
  using namespace mcrl2::utilities;

  try // parsing successful
  {
    interface_description clinterface(std::string(wxString(static_cast< wxChar** > (argv)[0], wxConvLocal).fn_str()),
        NAME, AUTHOR, "[OPTION]... [INFILE]",
        "Graphical editing environment for mCRL2 process specifications. "
        "If INFILE is supplied, it is loaded as a GraPE specification."); 

    command_line_parser parser(clinterface, argc, static_cast< wxChar** > (argv));
 
    // the filename is the first parameter
    wxString filename;

    if (1 < parser.arguments.size()) {
      filename = wxString(parser.arguments[0].c_str(), wxConvLocal);
    }

    grape_frame *frame = new grape_frame( filename );
    SetTopWindow(frame);

    wxInitAllImageHandlers();
  }
  catch (std::exception& e) // parsing not successful
  {
    std::cerr << e.what() << std::endl;
  }

  return true;
}

int main(int argc, char** argv)
{
  init_mcrl2libs(argc, argv);
  return wxEntry(argc, argv);
}

IMPLEMENT_APP_NO_MAIN(grape_app)

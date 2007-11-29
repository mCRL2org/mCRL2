// Author(s): Jeroen van der Wulp
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file gui/squadt.cpp
/// \brief Add your file description here.

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

#include "tipi/utility/print_logger.hpp"

#include "settings_manager.hpp"
#include "tool_manager.hpp"
#include "build_system.hpp"
#include "executor.hpp"

#include "gui/splash.hpp"
#include "gui/main.hpp"

#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/thread.h>
#include <wx/cmdline.h>
#include <wx/msgdlg.h>
#include <wx/sysopt.h>

const char* program_name    = "squadt";
const char* program_version = "July 2007";

using namespace squadt::GUI;

class initialisation : public wxThread {

  private:

    splash* splash_window;

  public:

    bool too_many_tools_failed;

    initialisation(splash* s) : wxThread(wxTHREAD_JOINABLE), splash_window(s) {
      too_many_tools_failed = false;
      Create();
      Run();
    }

    void* Entry() {
      try {
        squadt::global_build_system.get_tool_manager()->query_tools(
                    boost::bind(&splash::set_operation, splash_window, "", _1));
      }
      catch (...) {
        too_many_tools_failed = true;
      }

      return (0);
    }
};

bool parse_command_line(int argc, wxChar** argv, boost::function < void (squadt::GUI::main*) >& action) {
  bool c = 0 < argc;

  if (c) {
    wxCmdLineParser parser(argc, argv); 
 
    parser.AddSwitch(wxT("c"),wxT("create"),wxT(""));
    parser.AddSwitch(wxT("d"),wxT("debug"),wxT(""));
    parser.AddSwitch(wxT("h"),wxT("help"),wxT(""));
    parser.AddSwitch(wxT("q"),wxT("quiet"),wxT(""));
    parser.AddSwitch(wxT("v"),wxT("verbose"),wxT(""));
    parser.AddSwitch(wxT(""),wxT("version"),wxT(""));
    parser.AddParam(wxEmptyString,wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL);
 
    c = parser.Parse(false) == 0;
 
    tipi::controller::communicator::get_standard_logger()->set_filter_level(1);

    if (c) {
      if (parser.Found(wxT("c"))) {
        if (0 < parser.GetParamCount()) {
          action = boost::bind(&squadt::GUI::main::project_new, _1, std::string(parser.GetParam(0).fn_str()), std::string());
        }
        else {
          std::cerr << "Fatal: found -c, or --create option so expected path argument\n" << std::string(parser.GetParam(0).fn_str());

          return (false);
        }
      }
      else if (0 < parser.GetParamCount()) {
        action = boost::bind(&squadt::GUI::main::project_open, _1, std::string(parser.GetParam(0).fn_str()));
      }
      if (parser.Found(wxT("d"))) {
        tipi::controller::communicator::get_standard_logger()->set_filter_level(3);
      }
      if (parser.Found(wxT("v"))) {
        tipi::controller::communicator::get_standard_logger()->set_filter_level(2);
      }
      if (parser.Found(wxT("h"))) {
        std::cout << "Usage: " << program_name << " [OPTION] [PATH]\n"
                  << "Graphical environment that provides a uniform interface for using all kinds\n"
                  << "of other connected tools.\n"
                  << "\n"
                  << "Mandatory arguments to long options are mandatory for short options too.\n"
                  << "  -c, --create          create new project in PATH\n"
                  << "  -d, --debug           produce lots of debug output\n"
                  << "  -h, --help            display this help message\n"
                  << "  -q, --quiet           represses unnecessary output\n"
                  << "  -v, --verbose         display additional information during operation\n"
                  << "      --version         display version information\n"
                  << "\n";
 
        return (false);
      }
      if (parser.Found(wxT("q"))) {
        tipi::controller::communicator::get_standard_logger()->set_filter_level(0);
      }
      if (parser.Found(wxT("version"))) {
        std::cerr << program_name << " " << program_version << " (revision " << MCRL2_REVISION << ")" << std::endl;
 
        return (false);
      }
    }
  }

  return (c);
}

/* Squadt class declaration */
class Squadt : public wxApp {
  public:

    virtual bool OnInit();
    virtual int  OnExit();
};

IMPLEMENT_APP(Squadt)

/*
 * Squadt class implementation
 *
 * Must return true because static initialisation might not have completed
 */
bool Squadt::OnInit() {
  using namespace squadt;
  using namespace squadt::GUI;

  boost::function < void (squadt::GUI::main*) > action;

  bool c = parse_command_line(argc, argv, action);

  if (c) {
    wxInitAllImageHandlers();

    wxSystemOptions::SetOption(wxT("msw.remap"), 0);
    
    #include "pixmaps/logo.xpm"
    
    wxImage logo(logo_xpm);
    
    splash* splash_window = new splash(&logo, 1);

    try {
      try {
      global_build_system.initialise(
          std::auto_ptr < settings_manager > (new settings_manager(std::string(wxFileName::GetHomeDir().fn_str()))),
          std::auto_ptr < tool_manager > (new tool_manager()),
          std::auto_ptr < executor > (new executor()),
          std::auto_ptr < type_registry > (new type_registry()));
      }
      catch (std::exception& e) {
        wxMessageDialog(0, wxT("Initialisation error!\n\n") + wxString(e.what(), wxConvLocal), wxT("Fatal"), wxOK|wxICON_ERROR).ShowModal();

        return false;
      }
     
      splash_window->set_category("Querying tools", global_build_system.get_tool_manager()->number_of_tools());
     
      /* Perform initialisation */
      initialisation ti(splash_window);
     
      /* Cannot just wait because the splash would not be updated */
      while (ti.IsAlive()) {
        splash_window->update();
     
        wxApp::Yield();
      }

      if (ti.too_many_tools_failed) {
        wxMessageDialog retry(0, wxT("Do you want to replace the current list of known tools with the default set and retry?"),
                wxT("Initialisation of multiple tools failed!"), wxOK|wxCANCEL|wxICON_WARNING);

        if (retry.ShowModal() == wxID_OK) {
          /* Perform initialisation */
          initialisation reinitialisation_thread(splash_window);

          global_build_system.get_tool_manager()->factory_configuration();
       
          /* Cannot just wait because the splash would not be updated */
          while (reinitialisation_thread.IsAlive()) {
            splash_window->update();
       
            wxApp::Yield();
          }
        }
      }

      global_build_system.get_type_registry()->rebuild_indices();

      splash_window->set_category("Initialising components");

      /* Disable splash */
      splash_window->set_done();

      /* Initialise main application window */
      SetTopWindow(new squadt::GUI::main());
     
      if (action) {
        action(static_cast < squadt::GUI::main* > (GetTopWindow()));
      }

      SetUseBestVisual(true);
    }
    catch (...) {
      splash_window->set_done();

      wxMessageDialog(0, wxT("Initialisation failed! Another instance, or tool related to a previous instance, is probably still active and blocking the initialisation."), wxT("Fatal"), wxOK|wxICON_ERROR).ShowModal();

      return (false);
    }
  }

  return (c);
}

int Squadt::OnExit() {
  return (wxApp::OnExit());
}

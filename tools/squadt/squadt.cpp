// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file squadt.cpp

#include "wx.hpp" // precompiled headers

#include "boost/version.hpp"
#include "boost/thread/condition.hpp"
#include "boost/thread/thread.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/function.hpp"
#include "boost/bind.hpp"
#include "boost/filesystem/convenience.hpp"

#define MCRL2_TOOL_CLASSES_NO_CORE
#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/utilities/wx_tool.h"

#include "settings_manager.hpp"
#include "tool_manager.hpp"
#include "build_system.hpp"
#include "type_registry.hpp"
#include "executor.hpp"

#include "gui/splash.hpp"
#include "gui/main.hpp"

#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/dirdlg.h>
#include <wx/image.h>
#include <wx/msgdlg.h>
#include <wx/sysopt.h>

inline boost::filesystem::path parent_path(boost::filesystem::path const& p) {
#if (103500 < BOOST_VERSION)
  return p.parent_path();
#else
  return p.branch_path();
#endif
}

using namespace squadt::GUI;
using namespace mcrl2::utilities;

/* SQuADt class declaration */
class SQuADt : public mcrl2::utilities::wx::tool< SQuADt, mcrl2::utilities::tools::input_tool > {

  private:

    typedef mcrl2::utilities::wx::tool< SQuADt, mcrl2::utilities::tools::input_tool > super;

    // Optional action to execute after GUI construction
    boost::function < void (squadt::GUI::main*) > action;

    // Port number to listen on for incoming TCP connections
    tipi::tcp_port                                tcp_port_number;

  protected:

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("create", "create new project in PATH", 'c').
           add_option("port", make_mandatory_argument("PORT"), "listen on TCP port number PORT", 'p');
    }

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);

      // default log level
      tipi::utility::logger::log_level default_log_level = 1;

      if (0 < parser.options.count("create")) {
        if (parser.arguments.size() != 1) {
          parser.error("create option requires that a path is provided.");
        }
      }
      if (0 < parser.options.count("port")) {
        tcp_port_number = parser.option_argument_as< tipi::tcp_port > ("port");
      }

      if (parser.arguments.size() == 1) {
        boost::filesystem::path target(parser.arguments[0]);

        if (!target.has_root_path()) {
          target = boost::filesystem::initial_path() / target;
        }

        if (0 < parser.options.count("create")) {
          action = boost::bind(&squadt::GUI::main::project_new, _1, target.string(), std::string());
        }
        else {
          action = boost::bind(&squadt::GUI::main::project_open, _1, target.string());
        }
      }

      if (1 < parser.arguments.size()) {
        parser.error("too many file arguments!");
      }

      if (parser.options.count("quiet")) {
        default_log_level = 0;
      }
      if (parser.options.count("verbose")) {
        default_log_level = 2;
      }
      if (parser.options.count("debug")) {
        default_log_level = 3;
      }

      tipi::controller::communicator::get_default_logger().set_default_filter_level(default_log_level);
      tipi::controller::communicator::get_default_logger().set_filter_level(default_log_level);
    }


    bool parse_command_line(int& argc, wxChar** argv);

    std::string synopsis() const
    {
      return "[OPTION]... [PATH]";
    }

  public:

    bool run();

    SQuADt() : super(
        "SQuADT", // tool name
        "interactive integration of software tools",
        "Graphical environment that provides a uniform interface"
        " for using all kinds of other connected tools.", // GUI specific
        "Graphical environment that provides a uniform interface for using all kinds of "
        "other connected tools. If PATH is provided, it provides an existing project in "
        "PATH.",
        std::vector< std::string >(1, "Jeroen van der Wulp")), tcp_port_number(10949)
    { }
};

IMPLEMENT_APP(SQuADt)

/*
 * SQuADt class implementation
 *
 * Must return true because static initialisation might not have completed
 */
bool SQuADt::run() {
  using namespace squadt;
  using namespace squadt::GUI;

  wxInitAllImageHandlers();

  // Windows specific workaround for correct handling of icon transparency
  wxSystemOptions::SetOption(wxT("msw.remap"), 0);

  splash* splash_window = new splash(1);

  try {
    struct local {
      static void initialise_tools(splash& splash_window, bool& finished, std::vector< boost::shared_ptr< squadt::tool > >& retry_list) {
        tool_manager& local_tool_manager(squadt::global_build_system.get_tool_manager());

        tool_manager::const_tool_sequence tools(local_tool_manager.get_tools());

        for (tool_manager::const_tool_sequence::const_iterator t = tools.begin(); t != tools.end(); ++t) {
          splash_window.set_operation("", (*t)->get_name());

          if (!local_tool_manager.query_tool(*t)) {
            retry_list.push_back(*t);
          }
        }

        finished = true;
      }
    };

    try {
      std::auto_ptr < settings_manager > global_settings_manager(new settings_manager(std::string(wxFileName::GetHomeDir().fn_str())));

      // Open log file
      tipi::controller::communicator::get_default_logger().set_filter_level(3);
      tipi::controller::communicator::get_default_logger().redirect(global_settings_manager->path_to_user_settings().append("/log"));

      global_build_system.initialise(
        global_settings_manager,
        std::auto_ptr < tool_manager > (new tool_manager(tcp_port_number)),
        std::auto_ptr < executor > (new executor()),
        std::auto_ptr < type_registry > (new type_registry()));
    }
    catch (std::exception& e) {
      wxMessageDialog(0, wxT("Initialisation error!\n\n") + wxString(e.what(), wxConvLocal), wxT("Fatal"), wxOK|wxICON_ERROR).ShowModal();
 
      exit(EXIT_FAILURE);
    }

    splash_window->set_category("Querying tools", global_build_system.get_tool_manager().number_of_tools());

    /* Perform initialisation */
    bool finished = false;
    std::vector< boost::shared_ptr < squadt::tool > > retry_list;

    boost::thread initialisation_thread(boost::bind(&local::initialise_tools,
                      boost::ref(*splash_window), boost::ref(finished), boost::ref(retry_list)));

    /* Cannot just wait because the splash screen would not be updated */
    do {
      wxApp::Yield();

      splash_window->update();
    }
    while (!finished);

    if (!retry_list.empty()) {

      std::string s;
      for(std::vector< boost::shared_ptr < squadt::tool > >::iterator i = retry_list.begin(); i != retry_list.end(); ++i)
      {
	s.append("\t- ");
        s.append((*i)->get_name().c_str());
	s.append("\n");
      }
      

      wxMessageDialog retry(0, wxT("Cannot initialize the following tools:\n"+ 
              wxString(s.c_str(), wxConvLocal) ),
              wxT("Initialisation failure!"), wxOK|wxICON_WARNING);

      retry.ShowModal();

    }

    splash_window->set_category("Initialising components");

    global_build_system.get_type_registry().rebuild_indices();

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

    wxMessageDialog(0, wxT("Another instance or tool related to a previous instance, "
      "is probably still active and blocking the initialisation."),
                 wxT("Intialisation failed"), wxOK|wxICON_ERROR).ShowModal();

    return false;
  }

  return true;
}

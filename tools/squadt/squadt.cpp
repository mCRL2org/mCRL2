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

#define NAME "squadt"
#define AUTHOR "Jeroen van der Wulp"

#include "boost/version.hpp"
#include "boost/thread/condition.hpp"
#include "boost/thread/thread.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/function.hpp"
#include "boost/bind.hpp"
#include "boost/filesystem/convenience.hpp"

#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/utilities/command_line_wx.h"

#include "settings_manager.hpp"
#include "tool_manager.hpp"
#include "build_system.hpp"
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

/* SQuADt class declaration */
class SQuADt : public mcrl2::utilities::wx::tool< SQuADt > {
  friend class mcrl2::utilities::wx::tool< SQuADt >;

  private:

    // Optional action to execute after GUI construction
    boost::function < void (squadt::GUI::main*) > action;

    // Port number to listen on for incoming TCP connections
    tipi::tcp_port                                tcp_port_number;

  private:

    bool parse_command_line(int& argc, wxChar** argv);

    bool DoInit();
};

IMPLEMENT_APP(SQuADt)

bool SQuADt::parse_command_line(int& argc, wxChar** argv) {
  using namespace mcrl2::utilities;

  if (0 < argc) {
    interface_description clinterface(std::string(wxString(static_cast< wxChar** > (argv)[0], wxConvLocal).fn_str()), NAME, AUTHOR, "[OPTION]... [PATH]\n",
        "Graphical environment that provides a uniform interface for using all kinds of "
        "other connected tools. If PATH is provided, it provides an existing project in "
        "PATH.");

    clinterface.
      add_option("create", "create new project in PATH", 'c').
      add_option("port", make_mandatory_argument("PORT"), "listen on TCP port number PORT", 'p');

    tcp_port_number = 10949;

    command_line_parser parser(clinterface, argc, static_cast< wxChar** > (argv));

    if (parser.continue_execution()) {
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

    return parser.continue_execution();
  }

  return true;
}

/*
 * SQuADt class implementation
 *
 * Must return true because static initialisation might not have completed
 */
bool SQuADt::DoInit() {
  using namespace squadt;
  using namespace squadt::GUI;

  wxInitAllImageHandlers();

  // Windows specific workaround for correct handling of icon transparency
  wxSystemOptions::SetOption(wxT("msw.remap"), 0);

  splash* splash_window = new splash(1);

  try {
    struct local {
      static void initialise_tools(splash& splash_window, bool& finished, std::vector< boost::shared_ptr< tool > >& retry_list) {
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

      return false;
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
      wxMessageDialog retry(0, wxT("Initialisation of some tools failed.\n"
              "Do you want to retry initialisation of these tools interactively?"),
              wxT("Initialisation failure!"), wxYES_NO|wxICON_WARNING);

      if (retry.ShowModal() == wxID_YES) {
        struct tester {
          inline static bool query_with_path(tool& t, boost::filesystem::path const& p) {
            boost::shared_ptr< tool > dummy(new tool(t));

            dummy->set_location(p);

            if (squadt::global_build_system.get_tool_manager().query_tool(dummy)) {
              t.set_location(dummy->get_location());
              t.set_capabilities(dummy->get_capabilities());

              return true;
            }

            return false;
          }
        };

        boost::filesystem::path path_to_try(parent_path(retry_list[0]->get_location()));

        // Perform initialisation
        for (std::vector< boost::shared_ptr< tool > >::iterator t = retry_list.begin(); t != retry_list.end(); ++t) {
          splash_window->set_operation("", (*t)->get_name());
          splash_window->update();

          if (!boost::filesystem::exists((*t)->get_location()) &&
                   (!path_to_try.empty() &&
		 !tester::query_with_path(**t, path_to_try / (*t)->get_location().leaf()))) {

            wxFileDialog file_picker(0, wxT("Choose the file to use for `") +
                      wxString((*t)->get_name().c_str(), wxConvLocal) + wxT("'"),
                              wxString(path_to_try.string().c_str(), wxConvLocal), wxT(""), wxT("*"), wxFD_DEFAULT_STYLE|wxFD_FILE_MUST_EXIST|wxFD_OPEN);

            if (file_picker.ShowModal() == wxID_OK) {
              path_to_try = parent_path(boost::filesystem::path(
                              std::string(file_picker.GetPath().fn_str())));

              tester::query_with_path(**t, std::string(file_picker.GetPath().fn_str()));
            }
          }
        }
      }
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

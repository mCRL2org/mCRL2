// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/wx_tool.h
/// \brief A tool class that use wxWidgets

#ifndef __MCRL2_COMMAND_LINE_WX_HPP__
#define __MCRL2_COMMAND_LINE_WX_HPP__

#include <algorithm>
#include <iostream>

#include <wx/wx.h>
#include <wx/aboutdlg.h>

#include "boost/bind.hpp"
#include "boost/shared_array.hpp"
#include "boost/algorithm/string/join.hpp"
#include "boost/algorithm/string/case_conv.hpp"

#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/utilities/input_tool.h" // temporary measure

namespace mcrl2 {
  namespace utilities {
    /**
     * \brief Namespace containing wxWidgets utility functionality
     **/
    namespace wx {
      /**
       * \brief Component for building wxWidgets tools with mCRL2 toolset command line interface
       *
       * The issues that are solved are that parse errors are reported to the
       * user in a standard way and that the application terminates
       * successfully after some command line options such as --help or
       * --version.
       *
       * Event triggered by the standard wxWidgets `help' and `about' menu buttons
       * are captured and processed if the application itself handle the
       * events. The standard response for help events is to open the online
       * manual page for the tool on the wiki with the system web browser. For
       * about buttons a standard about dialog is shown.
       *
       * To use this component the wxWidgets application class should derive
       * from this template class and implement a DoInit method instead of the
       * OnInit method. It must also implement a parse_command_line(int,
       * wxChar**) method.
       *
       * The type parameter is the name of the class that implements the
       * wxWidgets application (using the Curiously Recurring Template
       * Pattern). Instead deriving directly of the application class (say A)
       * from wxApp it should derive from wx::tool< A, B >. Where B is an
       * instance of another tool class.
       **/
      template < typename Derived, typename ToolBase = void >
      class tool : public wxApp, public ToolBase {

        private:

          bool         m_execute;

          std::string  m_parse_error;

          std::string                m_tool_name;
          std::string                m_description;
          std::vector< std::string > m_developers;
          std::vector< std::string > m_documenters;

        private:

          static wxString wx_cast(std::string const& source) {
            return wxString(source.c_str(), wxConvLocal);
          };

          bool pre_run() {
            m_execute = true;

            return false;
          }

          bool Initialize(int& argc, wxChar** argv) {
            try {
              bool result = execute(argc, argv);

              m_execute = (m_execute || result) && wxApp::Initialize(argc, argv);
            }
            catch (std::exception& e) {
              if (wxApp::Initialize(argc, argv)) {
                m_parse_error = std::string(e.what()).
                  append("\n\nNote that other command line options may have been ignored because of this error.");
              }
              else {
                std::cerr << e.what() << std::endl;

                return false;
              }
            }

            return true;
          }

          // Tool class compatibility
          int execute(int& argc, wxChar** argv) {
            std::vector< boost::shared_array< char > >   arguments;
            boost::shared_array< char* >                 converted_arguments(new char*[argc]);

            for (int i = 0; i < argc; ++i)
            {
              std::string argument(wxString(argv[i], wxConvLocal).fn_str());

              arguments.push_back(boost::shared_array< char >(new char[argument.size() + 1]));

              std::copy(argument.begin(), argument.end(), arguments.back().get());

              // Make sure that the string is zero terminated
              arguments.back()[argument.size()] = '\0';

              converted_arguments[i] = arguments.back().get();
            }

            return ToolBase::execute(argc, converted_arguments.get());
          }

          class about_information : public wxAboutDialogInfo {
            public:
              void add_developer(std::string const& s) {
                AddDeveloper(wx_cast(s));
              }
              void add_documenter(std::string const& s) {
                AddDocWriter(wx_cast(s));
              }
          };

          void OnAbout() const {
            about_information information;

            //set tool name
            information.SetName(wx_cast(m_tool_name));

            //our approach (put version in description to improve formatting):
            information.SetDescription(wx_cast("mCRL2 toolset " + mcrl2::utilities::version_tag() +
                         "\n(revision " MCRL2_REVISION + ")\n\n" + m_description));

            //set copyright
            information.SetCopyright(wx_cast(std::string("Copyright \xA9 ") +
                         mcrl2::utilities::copyright_period() + " Technische Universiteit Eindhoven."));

            //set developers
            std::for_each(m_developers.begin(), m_developers.end(),
                 boost::bind(&about_information::add_developer, &information, _1));

            //set documenters
            std::for_each(m_documenters.begin(), m_documenters.end(),
                 boost::bind(&about_information::add_documenter, &information, _1));

            //set license
            information.SetLicense(wxT("This is free software.\n"
              "You may redistribute copies of it under the terms of the Boost Software License "
              "<http://www.boost.org/LICENSE_1_0.txt>.\n"
              "There is NO WARRANTY, to the extent permitted by law."));

            //set website
            information.SetWebSite(wxT("http://www.mcrl2.org"), wxT("mCRL2 home page"));

            //show the about box
            ::wxAboutBox(information);
          }

          // Helper class for wxEventHandling
          class wx_handler : public wxEvtHandler {

              wx::tool< Derived, ToolBase > const& m_wx_tool;

            public:

              void on_about(wxCommandEvent&) {
                m_wx_tool.OnAbout();
              }

              void on_help(wxCommandEvent&) {
                wxLaunchDefaultBrowser(wxString(
                      std::string("http://www.mcrl2.org/mcrl2/wiki/index.php/User_manual/"
                               + boost::to_lower_copy(m_wx_tool.m_tool_name)).c_str(), wxConvLocal));
              }

              wx_handler(wx::tool< Derived, ToolBase > const& wx_tool) : m_wx_tool(wx_tool) {
              }
          };

          // Needed for successful termination
          int OnRun() {
            if (m_execute) {
              if (wxWindow* window = GetTopWindow()) {
                wx_handler* handler = new wx_handler(*this);

                // register fallback event handler
                wxEvtHandler* original(window->PopEventHandler());

                window->PushEventHandler(handler);
                window->PushEventHandler(original);

                handler->Connect(wxID_ABOUT, wxEVT_COMMAND_MENU_SELECTED,
                   wxCommandEventHandler(wx_handler::on_about), 0, handler);
                handler->Connect(wxID_HELP, wxEVT_COMMAND_MENU_SELECTED,
                   wxCommandEventHandler(wx_handler::on_help), 0, handler);
              }

              return wxApp::OnRun();
            }

            return EXIT_SUCCESS;
          }

          bool OnInit() {
            if (m_execute) {
              if (static_cast< Derived& >(*this).run()) {
                if (!m_parse_error.empty()) {
                  wxMessageDialog(GetTopWindow(), wxString(m_parse_error.c_str(), wxConvLocal),
                                     wxT("Command line parsing error"), wxOK|wxICON_ERROR).ShowModal();
                }
              }
            }

            return true;
          }

        protected:

          /// \brief Override for wxApp::OnExit
          virtual int OnExit() {
            return wxApp::OnExit();
          }

        public:

          /** \brief Preferred constructor
           *  \param[in] tool_name   The name of the tool
           *  \param[in] description A one line description of the tool
           *  \param[in] description_gui A one line description of the tool (GUI specific)
           *  \param[in] developers  The developers of the tool
           *  \param[in] documenters The documenters of the tool
           **/
          tool(std::string const& tool_name,
                      std::string const& what_is,
                      std::string const& description_gui,
                      std::string const& description,
                      std::vector< std::string > const& developers,
                      std::string const& known_issues = "",
                      std::vector< std::string > const& documenters = std::vector< std::string >()) :
                          ToolBase(boost::to_lower_copy(tool_name),
                                   boost::join(developers, ","),
                                   what_is, description, known_issues),
                          m_execute(false), m_tool_name(tool_name),
                          m_description(description_gui), m_developers(developers),
                          m_documenters(documenters) {
          }
      };

      /// Backward compatibility (temporary measure)
      /// \deprecated
      template < typename Derived >
      class tool< Derived, void > : public tool< Derived, tools::input_tool >
      {
        public:
          bool run() {
            return static_cast< Derived& >(*this).DoInit();
          }

          tool(std::string const& tool_name,
                      std::string const& description,
                      std::vector< std::string > const& developers,
                      std::vector< std::string > const& documenters = std::vector< std::string >()) :
                          tool< Derived, tools::input_tool >(
                                   tool_name, "", description, description, developers, "", documenters)
          { }
      };
    }
  }
}

#endif


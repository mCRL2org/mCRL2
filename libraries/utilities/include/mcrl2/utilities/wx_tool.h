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
#include "boost/algorithm/string/case_conv.hpp"

#include "mcrl2/utilities/command_line_interface.h"

namespace mcrl2 {
  namespace utilities {
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
       * from wxApp it should derive from wx::tool< A >.
       **/
      template < typename CRTP >
      class tool : public wxApp {

        private:

          bool         m_execute;

          std::string  m_parse_error;

          std::string                m_tool_name;
          std::string                m_description;
          std::vector< std::string > m_developers;
          std::vector< std::string > m_documentors;

        private:

          static inline wxString wx_cast(std::string const& source) {
            return wxString(source.c_str(), wxConvLocal);
          };

          inline bool Initialize(int& argc, wxChar** argv) {
            try {
              m_execute = static_cast< CRTP& >(*this).parse_command_line(argc, argv);
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

              return true;
            }

            if (!wxApp::Initialize(argc, argv) && m_execute) {
              return false;
            }

            return true;
          }

          class about_information : public wxAboutDialogInfo {
            public:
              void add_developer(std::string const& s) {
                AddDeveloper(wx_cast(s));
              }
              void add_documentor(std::string const& s) {
                AddDocWriter(wx_cast(s));
              }
          };

          inline void OnAbout() const {
            about_information information;

            //set tool name
            information.SetName(wx_cast(m_tool_name));
     
            //our approach (put version in description to improve formatting):
            information.SetDescription(wx_cast("mCRL2 toolset " + mcrl2::utilities::version_tag() +
                         "(revision " MCRL2_REVISION + ")\n\n" + m_description));
     
            //set copyright
            information.SetCopyright(wx_cast(std::string("Copyright \xA9 ") +
                         mcrl2::utilities::copyright_period() + " Technische Universiteit Eindhoven."));
     
            //set developers
            std::for_each(m_developers.begin(), m_developers.end(),
                 boost::bind(&about_information::add_developer, &information, _1));
     
            //set documenters
            std::for_each(m_documentors.begin(), m_documentors.end(),
                 boost::bind(&about_information::add_documentor, &information, _1));
     
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

          // Needed for successful termination
          inline int OnRun() {
            // Helper class for wxEventHandling
            class wx_handler : public wxEvtHandler {

                wx::tool< CRTP > const& m_wx_tool;

              public:

                void on_about(wxCommandEvent&) {
                  m_wx_tool.OnAbout();
                }

                void on_help(wxCommandEvent&) {
                  wxLaunchDefaultBrowser(wxString(
                        std::string("http://www.mcrl2.org/wiki/index.php/User_manual/"
                                 + boost::to_lower_copy(m_wx_tool.m_tool_name)).c_str(), wxConvLocal));
                }

                wx_handler(wx::tool< CRTP > const& wx_tool) : m_wx_tool(wx_tool) {
                }
            };

            if (m_execute) {
              if (wxWindow* window = GetTopWindow()) {
 	        wx_handler* handler = new wx_handler(*this);

                // register fallback event handler
                wxEvtHandler* original(window->PopEventHandler());

                window->PushEventHandler(handler);
                window->PushEventHandler(original);

                handler->Connect(wxID_ABOUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(wx_handler::on_about), 0, handler);
                handler->Connect(wxID_HELP, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(wx_handler::on_help), 0, handler);
              }

              return wxApp::OnRun();
            }

            return EXIT_SUCCESS;
          }

          inline virtual int OnExit() {
            return wxApp::OnExit();
          }

          inline bool OnInit() {
            if (m_execute) {
              if (static_cast< CRTP& >(*this).DoInit()) {
                if (!m_parse_error.empty()) {
                  wxMessageDialog(GetTopWindow(), wxString(m_parse_error.c_str(), wxConvLocal),
                                     wxT("Command line parsing error"), wxOK|wxICON_ERROR).ShowModal();
                }
              }
            }

            return true;
          }

        public:


          /** \brief Preferred constructor
           *  \param[in] tool_name   The name of the tool
           *  \param[in] description A one line description of the tool
           *  \param[in] developers  The developers of the tool
           *  \param[in] documenters The documenters of the tool
           **/
          inline tool(std::string const& tool_name,
                      std::string const& description,
                      std::vector< std::string > const& developers,
                      std::vector< std::string > const& documentors = std::vector< std::string >()) :
                          m_execute(true), m_tool_name(tool_name),
                          m_description(description), m_developers(developers),
                          m_documentors(documentors) {
          }
      };
    }
  }
}

#endif


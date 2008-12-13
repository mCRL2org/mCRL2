// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/command_line_wx.h

#ifndef __MCRL2_COMMAND_LINE_WX_HPP__
#define __MCRL2_COMMAND_LINE_WX_HPP__

#include <iostream>
#include <wx/wx.h>

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

        public:

          /// \brief Constructor
          inline tool() : m_execute(true) {
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

          // Needed for successful termination
          inline int OnRun() {
            if (m_execute) {
              return wxApp::OnRun();
            }
   
            return EXIT_SUCCESS;
          }

          inline virtual int OnExit() {
            return wxApp::OnExit();
          }

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
         
            return wxApp::Initialize(argc, argv);
          }
      };
    }
  }
}

#endif


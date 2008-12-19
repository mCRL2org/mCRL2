// Author(s): Aad Mathijssen and Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/about_dialog.h
/// \brief About dialog for mCRL2 GUI tools

#include <wx/wx.h>
#include <wx/aboutdlg.h>

#include "mcrl2/utilities/command_line_interface.h"

namespace mcrl2 {
  namespace utilities {
    namespace wx {

      struct about_dialog {
        /** \brief Displays a platform dependent about dialog for GUI tools of the mCRL2 toolset.
          * \param[in] tool_name   The name of the tool
          * \param[in] description A one line description of the tool
          * \param[in] developers  The developers of the tool
          * \param[in] documenters The documenters of the tool
          **/
        about_dialog(
           std::string const& tool_name,
           std::string const& description,
           std::vector<std::string> const& developers,
           std::vector<std::string> const& documenters = std::vector<std::string>()
        ) {
          wxAboutDialogInfo information;

          //set tool name
          information.SetName(wxString(tool_name.c_str(), wxConvLocal));

          //set version and description
          
          std::string version("mCRL2 toolset " + mcrl2::utilities::version_tag());
          if (MCRL2_REVISION !=0) {
            version += "\n (revision " MCRL2_REVISION ")";
          }
          ////standard approach:
          //information.SetVersion(wxString(version.c_str(), wxConvLocal));
          //information.SetDescription(wxString(description.c_str(), wxConvLocal));

          //our approach (put version in description to improve formatting):
          std::string version_description(version + "\n\n" + description);
          information.SetDescription(wxString(version_description.c_str(), wxConvLocal));

          //set copyright
          std::string copyright(std::string("Copyright \xA9 ") + mcrl2::utilities::copyright_period() + " Technische Universiteit Eindhoven.");
          information.SetCopyright(wxString(copyright.c_str(), wxConvLocal));

          //set developers
          for (unsigned int i=0; i < developers.size(); i++) {
            information.AddDeveloper(wxString(developers[i].c_str(), wxConvLocal));
          }

          //set documenters
          for (unsigned int i=0; i < documenters.size(); i++) {
            information.AddDocWriter(wxString(documenters[i].c_str(), wxConvLocal));
          }

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
      };
    }
  }
}

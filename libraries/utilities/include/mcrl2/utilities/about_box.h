#include <wx/wx.h>
#include <wx/aboutdlg.h>

#include "mcrl2/utilities/command_line_interface.h"

namespace mcrl2 {
  namespace utilities {
    namespace wx {

      struct about_box {
        /** \brief Creates and displays an about dialog
         *
         **/
        about_box(std::string const& tool_name, std::string const& authors, std::string const& description) {
          wxAboutDialogInfo information;

          information.SetName(wxString(tool_name.c_str(), wxConvLocal));
          information.SetVersion(wxString((mcrl2::utilities::version_tag() + " (revision " MCRL2_REVISION ")").c_str(), wxConvLocal));
          information.AddDeveloper(wxString(authors.c_str(), wxConvLocal));
          information.SetDescription(wxString(description.c_str(), wxConvLocal));

          std::string copyright(std::string("Copyright \xA9 ") + mcrl2::utilities::copyright_period() + " Technische Universiteit Eindhoven.");

          information.SetCopyright(wxString(copyright.c_str(), wxConvLocal));
          information.SetLicense(wxT("This is free software.\n"
            "You may redistribute copies of it under the terms of the Boost Software License "
            "<http://www.boost.org/LICENSE_1_0.txt>.\n"
            "There is NO WARRANTY, to the extent permitted by law."));

          information.SetWebSite(wxT("http://www.mcrl2.org"), wxT("mCRL2 home page"));

          ::wxAboutBox(information);
        }
      };
    }
  }
}

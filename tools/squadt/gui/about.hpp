// Author(s): Jeroen van der Wulp
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file gui/about.h
/// \brief Add your file description here.

#include <wx/wx.h>
#include <wx/msgdlg.h>
#include "mcrl2/utilities/version_info.h"

#define STRINGIFY(x) EXPAND(x)
#define EXPAND(x) #x

namespace squadt {
  namespace GUI {

    /**
     * \brief A window that shows some basic information about deskSQuADT
     **/
    class about : public wxMessageDialog {

      protected:
      
        inline wxString get_text(void) {
          wxString msg = wxString();
          msg += wxT(get_version_information("deskSQuADT"));
          msg += wxT("\n");
          msg += wxT("\n");
          msg += wxT("A controlled environment that provides a graphical user interface that helps users to ");
          msg += wxT("interact with all kinds of connected tools. The goal is to simplify the use of these ");
          msg += wxT("tools especially in combination with each other.\n");
          msg += wxT("\n");
          msg += wxT("Developed by Jeroen van der Wulp\n");
          msg += wxT("\n");
          msg += wxT("This tool is distributed as part of the mCRL2 toolset.\n");
          msg += wxT("For information see http://www.mcrl2.org\n");
          msg += wxT("\n");
          msg += wxT("For feature requests or bug reports,\n");
          msg += wxT("please visit http://www.mcrl2.org/issuetracker\n");
          return msg;
        }

      public:

        /** \brief Constructor */
        about(wxWindow*);
    };

    /**
     * \param[in] p the parent window
     * \param[in] p the message to print
     **/
    inline about::about(wxWindow* p) : wxMessageDialog(p,
       get_text(), wxT("About deskSQuADT"), wxOK|wxICON_INFORMATION) {
    }
  
  }
}

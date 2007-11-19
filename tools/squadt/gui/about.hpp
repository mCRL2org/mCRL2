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

#define STRINGIFY(x) EXPAND(x)
#define EXPAND(x) #x

namespace squadt {
  namespace GUI {

    /**
     * \brief A window that shows some basic information about deskSQuADT
     **/
    class about : public wxMessageDialog {

      public:

        /** \brief Constructor */
        about(wxWindow*);
    };

    /**
     * \param[in] p the parent window
     * \param[in] p the message to print
     **/
    inline about::about(wxWindow* p) : wxMessageDialog(p,
       wxT("deskSQuADT (revision " STRINGIFY(REVISION) ")\n\n")
       wxT("A controlled environment that provides a graphical user interface that helps users to "
           "interact with all kinds of connected tools. The goal is to simplify the use of these "
           "tools especially in combination with each other.\n\n")
       wxT("Developed by Jeroen van der Wulp\n\n")
       wxT("Distributed as part of the mCRL2 toolset\n")
       wxT("For information: http://www.mcrl2.org\n")
       wxT("For problems or enhancement requests please use: http://www.mcrl2.org/issuetracker\n"),
       wxT("About deskSQuADT"), wxOK|wxICON_INFORMATION) {
    }
  }
}

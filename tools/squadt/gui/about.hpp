// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING).
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

#define NAME "SQuADT"
#define AUTHOR "Jeroen van der Wulp"

#define STRINGIFY(x) EXPAND(x)
#define EXPAND(x) #x

namespace squadt {
  namespace GUI {

    /**
     * \brief A window that shows some basic information about SQuADT
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
       wxString(get_version_information(NAME, AUTHOR).c_str(), wxConvLocal) +
       //wxT("\n")
       //wxT("A controlled environment that provides a graphical user interface that helps users to ")
       //wxT("interact with all kinds of connected tools. The goal is to simplify the use of these ")
       //wxT("tools especially in combination with each other.\n")
       wxT("\n")
       wxT("This tool is part of the mCRL2 toolset.\n")
       wxT("For information see http://www.mcrl2.org\n")
       wxT("\n")
       wxT("For feature requests or bug reports,\n")
       wxT("please visit http://www.mcrl2.org/issuetracker\n"),
       wxT("About SQuADT"), wxOK|wxICON_INFORMATION) {
    }
  }
}

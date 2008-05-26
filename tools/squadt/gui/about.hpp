// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file gui/about.h
/// \brief Add your file description here.

#include <wx/wx.h>
#include <wx/msgdlg.h>

std::string get_about_message();

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
       wxString(get_about_message().c_str(), wxConvLocal),
       wxT("About SQuADT"), wxOK|wxICON_INFORMATION) {
    }
  }
}

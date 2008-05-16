// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file gui/dialog/preferences.h
/// \brief Add your file description here.

#ifndef SQUADT_PREFERENCES_H_
#define SQUADT_PREFERENCES_H_

#include "../main.hpp"

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/notebook.h>

namespace squadt {
  namespace GUI {

    /**
     * \brief Displays a dialog where users can adjust settings
     **/
    class preferences : public wxDialog {

      private:

        /** \brief The control that selects between pages, represented using tabs */
        wxNotebook* tab_manager;

      private:

        /** \brief Event handler for tab activation */
        void change_tab(wxNotebookEvent&);

      public:

        /** \brief Constructor */
        preferences(main*);
    };
  }
}

#endif

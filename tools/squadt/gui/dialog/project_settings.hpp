// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file gui/dialog/project_settings.h
/// \brief Add your file description here.

#ifndef GUI_DIALOG_PROJECT_H
#define GUI_DIALOG_PROJECT_H

#include <string>

#include "base.hpp"

namespace squadt {
  namespace GUI {
    namespace dialog {

      /** \brief Base class for project level dialogs */
      class project : public dialog::basic {

        protected:

          /** \brief The directory from which the directory selectors start */
          static wxString default_directory;

        protected:

          /** \brief Determines wether a directory is recognised as a project store */
          static bool is_project_directory(wxString);

          /** \brief Determines wether a directory is recognised as a project store */
          static wxString get_project_description(wxString);

        public:

          /** \brief Constructor */
          project(wxWindow*, wxString);

          /** \brief Gets the name of the project */
          virtual std::string get_name() const = 0;

          /** \brief Destructor */
          virtual ~project() = 0;
      };

    }
  }
}

#endif


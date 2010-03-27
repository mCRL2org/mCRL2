// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file gui/dialog/project_settings.cpp
/// \brief Add your file description here.

#include "wx.hpp" // precompiled headers

#include "gui/dialog/project_settings.hpp"
#include "project_manager.hpp"
#include <boost/shared_ptr.hpp>

#include <wx/filename.h>

#define cmID_BROWSE (wxID_HIGHEST + 1)

namespace squadt {
  namespace GUI {
    namespace dialog {

      wxString project::default_directory = wxFileName::GetHomeDir();

      /**
       * @param p the parent window
       * @param t the title for the window
       **/
      project::project(wxWindow* p, wxString t) : dialog::basic(p, t, wxSize(450, 350)) {
      }

      /**
       * @param[in] p should be a valid path that identifies a project store
       **/
      bool project::is_project_directory(wxString p) {

        try {
          boost::shared_ptr < project_manager > m = project_manager::create(std::string(p.fn_str()), false);

          return m.get() != 0;
        }
        catch (...) {
        }

        return false;
      }

      /**
       * @param[in] p should be a valid path that identifies a project store
       **/
      wxString project::get_project_description(wxString p) {
        return (wxEmptyString);
      }

      project::~project() {
      }

    }
  }
}

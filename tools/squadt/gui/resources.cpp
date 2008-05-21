// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file gui/resources.cpp
/// \brief Add your file description here.

#include <wx/wx.h>
#include <wx/imaglist.h>

namespace squadt {
  namespace GUI {

    wxImageList* main_icon_list;
    wxImageList* format_icon_list;
    wxImageList* format_small_icon_list;

    /* Icon for use by Squadt */
    wxIcon*     gui_icon;
    wxBitmap*   alert_icon;

    /* Images */
    #include "pixmaps/file_original.xpm"
    #include "pixmaps/file_derived_nonexistent.xpm"
    #include "pixmaps/file_derived_up_to_date.xpm"
    #include "pixmaps/file_derived_out_of_date.xpm"
    #include "pixmaps/file_generation_in_progress.xpm"
    #include "pixmaps/stock_new_small.xpm"

    #include "pixmaps/logo_icon.xpm"
    #include "pixmaps/alert.xpm"

    /* General of GUI Icons */
    wxImageList* load_main_icons() {
      wxImageList* list = new wxImageList(24, 24, true, 7);

      list->Add(wxBitmap(file_original_xpm));
      list->Add(wxBitmap(file_derived_nonexistent_xpm));
      list->Add(wxBitmap(file_derived_out_of_date_xpm));
      list->Add(wxBitmap(file_derived_up_to_date_xpm));
      list->Add(wxBitmap(file_generation_in_progress_xpm));

      gui_icon = new wxIcon(logo_icon_xpm);
      alert_icon = new wxBitmap(alert_xpm);

      return(list);
    }

    /* General of icons for file formats */
    wxImageList* load_format_icons() {
      wxImageList* list = new wxImageList(24, 24, true, 5);

      list->Add(wxBitmap(file_original_xpm));
      list->Add(wxBitmap(file_derived_nonexistent_xpm));
      list->Add(wxBitmap(file_derived_out_of_date_xpm));
      list->Add(wxBitmap(file_derived_up_to_date_xpm));
      list->Add(wxBitmap(file_generation_in_progress_xpm));

      return(list);
    }

    /* General of icons for file formats */
    wxImageList* load_small_format_icons() {
      wxImageList* list = new wxImageList(16, 16, true, 1);

      list->Add(wxBitmap(stock_new_small_xpm));

      return(list);
    }
  }
}


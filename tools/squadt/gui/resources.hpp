// Author(s): Jeroen van der Wulp
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file gui/resources.h
/// \brief Add your file description here.

#ifndef RESOURCES_H
#define RESOURCES_H

#include <wx/wx.h>

namespace squadt {
  namespace GUI {

    extern wxImageList* main_icon_list;
    extern wxImageList* format_icon_list;
    extern wxImageList* format_small_icon_list;

    extern wxIcon*   gui_icon;
    extern wxBitmap* alert_icon;

    /* Function to load the icons */
    wxImageList* load_main_icons();
    wxImageList* load_format_icons();
    wxImageList* load_small_format_icons();
  }
}

#endif

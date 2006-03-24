#ifndef RESOURCES_H
#define RESOURCES_H

#include <wx/wx.h>

namespace squadt {
  namespace GUI {

    extern wxImageList* main_icon_list;
    extern wxImageList* format_icon_list;
    extern wxImageList* format_small_icon_list;

    extern wxIcon* gui_icon;

    /* Function to load the icons */
    wxImageList* load_main_icons();
    wxImageList* load_format_icons();
    wxImageList* load_small_format_icons();
  }
}

#endif

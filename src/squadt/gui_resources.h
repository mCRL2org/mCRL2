#ifndef RESOURCES_H
#define RESOURCES_H

#include <wx/wx.h>

extern wxImageList* main_icon_list;
extern wxImageList* format_icon_list;
extern wxImageList* format_small_icon_list;

extern wxIcon* gui_icon;

/* Function to load the icons */
wxImageList* LoadMainIcons();
wxImageList* LoadFormatIcons();
wxImageList* LoadSmallFormatIcons();

#endif

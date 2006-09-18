#include <wx/wx.h>
#include <wx/imaglist.h>

namespace squadt {
  namespace GUI {

    wxImageList* main_icon_list;
    wxImageList* format_icon_list;
    wxImageList* format_small_icon_list;

    /* Icon for use by Squadt */
    wxIcon* gui_icon;

    /* Images */
    #include "pixmaps/file_original.xpm"
    #include "pixmaps/file_derived_nonexistent.xpm"
    #include "pixmaps/file_derived_up_to_date.xpm"
    #include "pixmaps/file_derived_out_of_date.xpm"
    #include "pixmaps/file_generation_in_progress.xpm"
    #include "pixmaps/stock_new_small.xpm"

    #include "pixmaps/logo_icon.xpm"

    /* General of GUI Icons */
    wxImageList* load_main_icons() {
      wxImageList* list = new wxImageList(24, 24, false, 7);

      list->Add(wxIcon(file_original_xpm));
      list->Add(wxIcon(file_derived_nonexistent_xpm));
      list->Add(wxIcon(file_derived_out_of_date_xpm));
      list->Add(wxIcon(file_derived_up_to_date_xpm));
      list->Add(wxIcon(file_generation_in_progress_xpm));

      gui_icon = new wxIcon(logo_icon_xpm);

      return(list);
    }

    /* General of icons for file formats */
    wxImageList* load_format_icons() {
      wxImageList* list = new wxImageList(24, 24, false, 4);

      list->Add(wxIcon(file_original_xpm));
      list->Add(wxIcon(file_derived_nonexistent_xpm));
      list->Add(wxIcon(file_derived_out_of_date_xpm));
      list->Add(wxIcon(file_derived_up_to_date_xpm));
      list->Add(wxIcon(file_generation_in_progress_xpm));

      return(list);
    }

    /* General of icons for file formats */
    wxImageList* load_small_format_icons() {
      wxImageList* list = new wxImageList(16, 16, false, 1);

      list->Add(wxIcon(stock_new_small_xpm));

      return(list);
    }
  }
}


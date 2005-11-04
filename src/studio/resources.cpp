#include <wx/wx.h>
#include <wx/imaglist.h>

wxImageList* main_icon_list;
wxImageList* format_icon_list;
wxImageList* format_small_icon_list;

/* Images */
#include "pixmaps/stock_new.xpm"
#include "pixmaps/stock_new_red.xpm"
#include "pixmaps/stock_new_green.xpm"
#include "pixmaps/stock_new_blue.xpm"
#include "pixmaps/stock_open.xpm"
#include "pixmaps/stock_save.xpm"
#include "pixmaps/stock_delete.xpm"
#include "pixmaps/stock_refresh.xpm"
#include "pixmaps/stock_ok.xpm"
#include "pixmaps/stock_cancel.xpm"
#include "pixmaps/stock_new_small.xpm"

/* General of GUI Icons */
wxImageList* LoadMainIcons() {
  wxImageList* list = new wxImageList(24, 24, false, 7);

  list->Add(wxIcon(stock_new_xpm));
  list->Add(wxIcon(stock_open_xpm));
  list->Add(wxIcon(stock_save_xpm));
  list->Add(wxIcon(stock_delete_xpm));
  list->Add(wxIcon(stock_refresh_xpm));
  list->Add(wxIcon(stock_ok_xpm));
  list->Add(wxIcon(stock_cancel_xpm));

  return(list);
}

/* General of icons for file formats */
wxImageList* LoadFormatIcons() {
  wxImageList* list = new wxImageList(24, 24, false, 4);

  list->Add(wxIcon(stock_new_xpm));
  list->Add(wxIcon(stock_new_red_xpm));
  list->Add(wxIcon(stock_new_green_xpm));
  list->Add(wxIcon(stock_new_blue_xpm));

  return(list);
}

/* General of icons for file formats */
wxImageList* LoadSmallFormatIcons() {
  wxImageList* list = new wxImageList(16, 16, false, 1);

  list->Add(wxIcon(stock_new_small_xpm));

  return(list);
}

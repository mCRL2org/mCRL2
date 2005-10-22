#include <wx/imaglist.h>

#if defined(__WXGTK__) || defined(__WXMAC__)
  /* Images for GTK and MACOS */
  #include "pixmaps/stock_new.xpm"
  #include "pixmaps/stock_open.xpm"
  #include "pixmaps/stock_save.xpm"
  #include "pixmaps/stock_delete.xpm"
  #include "pixmaps/stock_refresh.xpm"
  #include "pixmaps/stock_ok.xpm"
  #include "pixmaps/stock_cancel.xpm"

  /* Icons */
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

#else
 /* Images for the Windows platform */
 wxImageList* LoadMainIcons() {
   ERROR_NOT_YET_IMPLEMENTED
 }
#endif

#ifndef INFODIALOG_H
#define INFODIALOG_H

#include <string>
#include <wx/wx.h>
#include <wx/listctrl.h>

class InfoDialog: public wxDialog {
  public:
    InfoDialog(wxWindow* parent);
    void setLTSInfo(int is, // Initial state
                    int ns, // Number of states
                    int nt, // Number of transitions
                    int nl // Number of labels/actions
                    );

  private:
    wxListCtrl* lts_info;
};

#endif // INFODIALOG_H

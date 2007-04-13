#ifndef SQUADT_GUI_HELP_H__
#define SQUADT_GUI_HELP_H__

#include <wx/wx.h>
#include <wx/html/htmlwin.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <iostream>

#include "../settings_manager.h"

namespace squadt {
  namespace GUI {

    /**
     * \brief The HTML manual browser
     **/
    class manual : public wxFrame {

      private:

        void build();

        void on_window_close(wxCloseEvent&);

      public:

        manual(wxWindow* p);
    };

    manual::manual(wxWindow* p) : wxFrame(p, wxID_ANY, wxT("Manual"), wxDefaultPosition, wxDefaultSize,wxDEFAULT_FRAME_STYLE & ~(wxMAXIMIZE_BOX)) {
      build();

      Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(manual::on_window_close));
    }

    void manual::on_window_close(wxCloseEvent& e) {
      if (e.CanVeto()) {
        e.Veto();

        Show(false);
      }
    }

    void manual::build() {
      wxBoxSizer* s = new wxBoxSizer(wxVERTICAL);

      SetSizer(s);

      wxHtmlWindow* w = new wxHtmlWindow(this);

      w->LoadPage(wxString(global_build_system.get_settings_manager()->path_to_system_settings("squadt/manual.html").c_str(), wxConvLocal));

      s->Add(w, 1, wxEXPAND);
    }
  }
}

#endif

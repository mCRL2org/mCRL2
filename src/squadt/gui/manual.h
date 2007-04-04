#ifndef SQUADT_GUI_HELP_H__
#define SQUADT_GUI_HELP_H__

#include <wx/html/htmlwin.h>
#include <wx/sizer.h>
#include <iostream>

namespace squadt {
  namespace GUI {

    /**
     * \brief The HTML manual browser
     **/
    class manual : protected wxFrame {

      private:

        void build();

        void on_window_close(wxCloseEvent&);

      public:

        manual(wxWindow* p);

        using wxWindow::Show;
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

      w->SetPage(wxT("<html><body>This is where the manual will be added!</body></html>"));

      s->Add(w, 1, wxEXPAND);
    }
  }
}

#endif

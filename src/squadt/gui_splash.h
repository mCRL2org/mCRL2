#ifndef SQUADT_SPLASH_H
#define SQUADT_SPLASH_H

#include <wx/wx.h>
#include <wx/bitmap.h>
#include <wx/frame.h>
#include <wx/gauge.h>
#include <wx/sizer.h>
#include <wx/dcclient.h>
#include <wx/image.h>
#include <wx/panel.h>

namespace squadt {
  namespace GUI {

    /**
     * Displays a splash screen with a progess bar that shows initialisation
     * progess.
     **/
    class splash : public wxFrame {
      private:
        wxGauge* progress_indicator;
        wxPanel* display;

      public:
        /** Constructor */
        inline splash(wxImage*);
    };
 
    /**
     * @param[in] i the image to display
     **/
    inline splash::splash(wxImage* i) :
            wxFrame(0, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER|wxSTAY_ON_TOP|wxFRAME_NO_TASKBAR) {

      wxBoxSizer*      s = new wxBoxSizer(wxVERTICAL);
      progress_indicator = new wxGauge(this, wxID_ANY, 10);
      display            = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(i->GetWidth(),i->GetHeight()));

      s->Add(display, 0, wxALL|wxEXPAND, 0);
      s->Add(progress_indicator, 0, wxALL|wxEXPAND, 0);

      SetSizer(s);

      wxPaintDC(display).DrawBitmap(wxBitmap(wxString(global_settings_manager->path_to_images("logo.jpg").c_str(), wxConvLocal), wxBITMAP_TYPE_JPEG), 0, 0, false);
//      wxPaintDC(display).DrawBitmap(wxBitmap(i), 0, 0, true);

      display->Refresh();

      s->CalcMin();

      Fit();

      Show(true);

      Centre();
    }
  }
}

#endif


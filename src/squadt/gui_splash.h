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

    class picture : public wxPanel {
      private:
        DECLARE_EVENT_TABLE()

        /* \brief a reference to the image to be displayed */
        wxBitmap image;

      private:
        /** \brief Method that paints the image */
        inline void OnPaint(wxPaintEvent& WXUNUSED(event));

      public:

        picture(wxWindow*, wxImage*);
    };

    BEGIN_EVENT_TABLE(picture, wxPanel)
     EVT_PAINT(picture::OnPaint)
    END_EVENT_TABLE()

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

        /** Update progress indicator */
        inline void update(unsigned int);
    };
 
    /**
     * @param p a pointer to a wxWindow that will serve as parent
     * @param i a reference to the image to be displayed
     **/
    inline picture::picture(wxWindow* p, wxImage* i) :
              wxPanel(p, wxID_ANY, wxDefaultPosition, wxSize(i->GetWidth(),i->GetHeight()), wxRAISED_BORDER), image(i) {
    }

    inline void picture::OnPaint(wxPaintEvent& WXUNUSED(event)) {
      wxPaintDC dc(this);

      dc.DrawBitmap(image, 0, 0, false);
    }

    /**
     * @param[in] i the image to display
     **/
    inline splash::splash(wxImage* i) :
            wxFrame(0, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxSTAY_ON_TOP|wxFRAME_NO_TASKBAR) {

      wxBoxSizer*      s = new wxBoxSizer(wxVERTICAL);
      progress_indicator = new wxGauge(this, wxID_ANY, 6);
      display            = new picture(this, i);

      s->Add(display, 0, wxALL|wxEXPAND, 0);
      s->Add(progress_indicator, 0, wxALL|wxEXPAND, 0);

      SetSizer(s);

      s->CalcMin();

      Fit();

      Show(true);

      Centre();
    }

    /**
     * @param p the value by which to increase the progress indicator
     **/
    inline void splash::update(unsigned int p) {
      progress_indicator->SetValue(progress_indicator->GetValue() + p);
    }
  }
}

#endif


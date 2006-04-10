#ifndef SQUADT_SPLASH_H
#define SQUADT_SPLASH_H

#include <wx/wx.h>
#include <wx/bitmap.h>
#include <wx/colour.h>
#include <wx/frame.h>
#include <wx/font.h>
#include <wx/gauge.h>
#include <wx/sizer.h>
#include <wx/dcclient.h>
#include <wx/image.h>
#include <wx/panel.h>
#include <wx/utils.h>

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
      friend class squadt::GUI::picture; 

      private:

        wxPanel*      display;

        wxGauge*      progress_indicator;

        unsigned char number_of_categories;

        unsigned char current_category;

        std::string   category;
        std::string   operation;
        std::string   operand;

        /** \brief Flag that indicates whether progress was made since last update */
        bool          changed;

      public:
        /** \brief Constructor */
        inline splash(wxImage*, unsigned char);

        /** \brief Sets the category */
        inline void set_category(const std::string&, unsigned int = 0);

        /** \brief Update progress indicator */
        inline void set_operation(const std::string&, const std::string&);

        /** \brief Finishes up and hides the splash window */
        inline void set_done();

        /** \brief Calls the wxWidgets Update() method, if required */
        inline void update();
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

      splash& s = *(dynamic_cast < splash* > (GetParent()));

      dc.DrawBitmap(image, 0, 0, false);
      dc.SetTextForeground(*wxBLACK);
      dc.SetBackgroundMode(wxTRANSPARENT);
      dc.SetFont(wxFont(14, wxFONTFAMILY_DECORATIVE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
      dc.DrawText(wxString(s.category.c_str(), wxConvLocal), 380, 30);
      dc.SetFont(wxFont(10, wxFONTFAMILY_DECORATIVE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

      if (!s.operation.empty()) {
        wxString o = wxString(s.operation.c_str(), wxConvLocal);

        o.Append(wxT(": ")).Append(wxString(s.operand.c_str(), wxConvLocal));

        dc.DrawText(o, 395, 70);
      }
    }

    /**
     * @param[in] i the image to display
     * @param[in] n the number of categories
     **/
    inline splash::splash(wxImage* i, unsigned char n) :
            wxFrame(0, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxSTAY_ON_TOP|wxFRAME_NO_TASKBAR),
            number_of_categories(n), current_category(0), changed(false) {

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
     * @param c a string that represents the category
     * @param m the maximum number of operations in this category (0 < m)
     **/
    inline void splash::set_category(const std::string& c, unsigned int m) {
      progress_indicator->SetRange(number_of_categories * (m + 1));
      progress_indicator->SetValue((current_category++ * progress_indicator->GetRange()) / number_of_categories);

      category = c;

      changed = true;
    }

    /**
     * @param o a string that represents an operation description
     * @param s a string that represents the operand
     **/
    inline void splash::set_operation(const std::string& o, const std::string& s) {

      progress_indicator->SetValue(progress_indicator->GetValue() + 1);

      operation = o;
      operand   = s;

      changed = true;
    }

    inline void splash::update() {
      if (changed) {
        changed = false;

        display->Update();
      }
    }

    inline void splash::set_done() {
      category.clear();
      operation.clear();
      operand.clear();

      progress_indicator->SetValue(progress_indicator->GetRange());

      display->Update();

      wxYield();
      wxSleep(1);

      Show(false);

      Destroy();
    }
  }
}

#endif


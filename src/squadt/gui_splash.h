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
    namespace detail {

      /**
       * \brief Basic control for loading a bitmap picture
       **/
      class picture : public wxPanel {
        private:
          /** \brief a reference to the image to be displayed */
          wxBitmap image;
     
        private:
          /** \brief Method that paints the image */
          inline void on_paint(wxPaintEvent& WXUNUSED(event));
     
        public:
     
          picture(wxWindow*, wxImage*);
      };
    }

    /**
     * \brief Basic splash screen that shows initialisation progress
     *
     * Displays a splash screen with a progess bar that shows initialisation
     * progess.
     **/
    class splash : public wxFrame {
      friend class squadt::GUI::detail::picture; 

      private:

        /** \brief The area where the picture is displayed */
        wxPanel*      display;

        /** \brief The progress indicator */
        wxGauge*      progress_indicator;

        /** \brief The number of subsequent categories, for progress */
        unsigned char number_of_categories;

        /** \brief The current */
        unsigned char current_category;

        /** \brief Description of a category */
        std::string   category;

        /** \brief Description of an operation that takes place within a category */
        std::string   operation;

        /** \brief Target of an operation */
        std::string   operand;

        /** \brief Amount to set the progress bar to during the next update */
        int           new_amount;

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
 
    namespace detail {

      /**
       * @param p a pointer to a wxWindow that will serve as parent
       * @param i a reference to the image to be displayed
       **/
      inline picture::picture(wxWindow* p, wxImage* i) :
                wxPanel(p, wxID_ANY, wxDefaultPosition, wxSize(i->GetWidth(),i->GetHeight()), wxRAISED_BORDER), image(*i) {

        Connect(wxEVT_PAINT, wxPaintEventHandler(picture::on_paint));
      }

      inline void picture::on_paint(wxPaintEvent& WXUNUSED(event)) {
        wxPaintDC dc(this);
     
        splash& s = *(dynamic_cast < splash* > (GetParent()));
     
        dc.DrawBitmap(image, 0, 0, false);
        dc.SetTextForeground(*wxBLACK);
        dc.SetBackgroundMode(wxTRANSPARENT);
        dc.SetFont(wxFont(15, wxFONTFAMILY_DECORATIVE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
        dc.DrawText(wxString(s.category.c_str(), wxConvLocal), 380, 30);
        dc.SetFont(wxFont(11, wxFONTFAMILY_DECORATIVE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
     
        if (!s.operation.empty()) {
          wxString o = wxString(s.operation.c_str(), wxConvLocal);
     
          o.Append(wxT(": ")).Append(wxString(s.operand.c_str(), wxConvLocal));
     
          dc.DrawText(o, 395, 70);
        }
      }
    }

    /**
     * @param[in] i the image to display
     * @param[in] n the number of categories
     **/
    inline splash::splash(wxImage* i, unsigned char n) :
            wxFrame(0, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxSTAY_ON_TOP|wxFRAME_NO_TASKBAR),
            number_of_categories(n), current_category(0), new_amount(0), changed(false) {

      wxBoxSizer*      s = new wxBoxSizer(wxVERTICAL);
      progress_indicator = new wxGauge(this, wxID_ANY, 6);
      display            = new detail::picture(this, i);

      s->Add(display, 0, wxALL|wxEXPAND, 0);
      s->Add(progress_indicator, 0, wxALL|wxEXPAND, 0);

      SetSizer(s);

      s->CalcMin();

      Fit();

      Show(true);

      CentreOnScreen();
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
      ++new_amount;

      operation = o;
      operand   = s;

      changed = true;
    }

    /**
     * This function should be called periodically to update the progress bar.
     * It should be called from the main thread. The reason for its existence
     * is so that set_operation() can be called from any other thread
     * (which could otherwise result in mysterious blocking behaviour).
     **/
    inline void splash::update() {
      if (changed) {
        changed = false;

        progress_indicator->SetValue(new_amount);

        display->Refresh();
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


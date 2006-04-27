#include <deque>

#include "gui_project.h"
#include "gui_tool_display.h"

#include <wx/button.h>
#include <wx/event.h>
#include <wx/gauge.h>
#include <wx/radiobut.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

#include <sip/detail/layout_mediator.h>
#include <sip/detail/layout_manager.h>

namespace squadt {
  namespace GUI {

    using ::sip::layout::mediator;

    using namespace sip;
    using namespace sip::layout;

    /** \brief Provides the means of */
    class tool_display_mediator : public sip::layout::mediator {
      
      public:

        class wrapper : public sip::layout::mediator::wrapper {

          private:
            wxObject* target;

          public:

            /** \brief Constructor */
            inline wrapper(wxObject*);

            /** \brief Get target cast to a sizer pointer */
            inline wxSizer* get_sizer() const;

            /** \brief Get target cast to a window pointer */
            inline wxWindow* get_window() const;
        };

      private:

        /** \brief The sizer to which the elements will be attached */
        wxWindow* current_window;

      private:

        /** \brief Helper function for layout managers to attach widgets */
        void attach_to_vertical_box(mediator::wrapper_aptr, sip::layout::constraints const*);

        /** \brief Helper function for layout managers to attach widgets */
        void attach_to_horizontal_box(mediator::wrapper_aptr, sip::layout::constraints const*);

      public:

        /** \brief Constructor */
        inline tool_display_mediator(wxWindow* w, wrapper_aptr d);

        /** \brief Instantiates a vertically aligned box layout manager */
        inline mediator::aptr build_vertical_box();
      
        /** \brief Instantiates a horizonally aligned box layout manager */
        inline mediator::aptr build_horizontal_box();
      
        /** \brief Instantiates a label (static text) */
        inline mediator::wrapper_aptr build_label(std::string const&);
      
        /** \brief Instantiates a label */
        inline mediator::wrapper_aptr build_button(std::string const&);
      
        /** \brief Instantiates a single radio button */
        inline mediator::wrapper_aptr build_radio_button(std::string const&);
      
        /** \brief Instantiates a progress bar */
        inline mediator::wrapper_aptr build_progress_bar(unsigned int const&, unsigned int const&, unsigned int const&);
      
        /** \brief Instantiates a single line text input control */
        inline mediator::wrapper_aptr build_text_field(std::string const&);

        /** \brief Destructor */
        inline ~tool_display_mediator();
    };

    /**
     * @param w the window to which to attach elements
     * @param d the sizer to which elements should be attached
     **/
    tool_display_mediator::tool_display_mediator(wxWindow* w, wrapper_aptr d) : mediator(d), current_window(w) {
    }

    /**
     * @param w the window or sizer to which to attach elements
     **/
    inline tool_display_mediator::wrapper::wrapper(wxObject* w) : target(w) {
    }

    /** \brief Get target cast to a sizer pointer */
    inline wxSizer* tool_display_mediator::wrapper::get_sizer() const {
      return (static_cast < wxSizer* > (target));
    }

    /** \brief Get target cast to a window pointer */
    inline wxWindow* tool_display_mediator::wrapper::get_window() const {
      return (static_cast < wxWindow* > (target));
    }

    /**
     * @param p pointer to the data that represent the window to be attached
     * @param c layout constraints
     **/
    void tool_display_mediator::attach_to_vertical_box(mediator::wrapper_aptr d, sip::layout::constraints const* c) {
      wxWindow* target = reinterpret_cast < wxWindow* > (d.get());
      int       flags  = wxLEFT|wxRIGHT;

      layout::box_vertical::constraints const& cr = *(static_cast < layout::box_vertical::constraints const* > (c));

      using sip::layout::box;

      switch (cr.align) {
        case layout::box < vertical >::left:
          flags |= wxALIGN_LEFT;
          break;
        case layout::box < vertical >::right:
          flags |= wxALIGN_RIGHT;
          break;
        default: /* center */
          flags |= wxALIGN_CENTER;
          break;
      }

      wxSizer* sizer = static_cast < tool_display_mediator::wrapper* > (data.get())->get_sizer();

      if (0 < cr.margin.top) {
        sizer->AddSpacer(cr.margin.bottom);
      }

      sizer->Add(target, 0, flags, (cr.margin.left + cr.margin.right) >> 1);

      if (0 < cr.margin.bottom) {
        sizer->AddSpacer(cr.margin.bottom);
      }

      if (cr.visible == sip::layout::hidden) {
        sizer->Show(target, false);
      }
    }

    /**
     * @param p pointer to the data that represent the window to be attached
     * @param c layout constraints
     **/
    void tool_display_mediator::attach_to_horizontal_box(wrapper_aptr d, sip::layout::constraints const* c) {
      wxWindow* target = reinterpret_cast < wxWindow* > (d.get());
      int       flags  = wxTOP|wxBOTTOM;

      sip::layout::box < horizontal >::constraints const& cr = *(static_cast < sip::layout::box < horizontal >::constraints const* > (c));

      switch (cr.align) {
        case sip::layout::box < sip::layout::horizontal >::top:
          flags |= wxALIGN_TOP;
          break;
        case sip::layout::box < sip::layout::horizontal >::bottom:
          flags |= wxALIGN_BOTTOM;
          break;
        default: /* center */
          flags |= wxALIGN_CENTER;
          break;
      }

      wxSizer* sizer = static_cast < tool_display_mediator::wrapper* > (data.get())->get_sizer();

      if (0 < cr.margin.left) {
        sizer->AddSpacer(cr.margin.left);
      }

      sizer->Add(target, 0, flags, (cr.margin.top + cr.margin.bottom) >> 1);

      if (0 < cr.margin.right) {
        sizer->AddSpacer(cr.margin.right);
      }

      if (cr.visible == sip::layout::hidden) {
        sizer->Show(target, false);
      }
    }

    /**
     * \return a standard auto pointer to a mediator object with current the mediator with which to attach the children
     **/
    mediator::aptr tool_display_mediator::build_vertical_box() {
      wxSizer* t = new wxBoxSizer(wxVERTICAL);

      sip::layout::mediator::aptr m(new tool_display_mediator(current_window, wrapper_aptr(new wrapper(t))));

      m->set_attach(boost::bind(&tool_display_mediator::attach_to_vertical_box,
                                                static_cast < tool_display_mediator* > (m.get()), _1, _2));

      return (m);
    }
    
    /**
     * \return a standard auto pointer to a mediator object with current the mediator with which to attach the children
     **/
    mediator::aptr tool_display_mediator::build_horizontal_box() {
      wxSizer* t = new wxBoxSizer(wxHORIZONTAL);

      sip::layout::mediator::aptr m(new tool_display_mediator(current_window, wrapper_aptr (new wrapper(t))));

      m->set_attach(boost::bind(&tool_display_mediator::attach_to_horizontal_box,
                                                static_cast < tool_display_mediator* > (m.get()), _1, _2));

      return (m);
    }
    
    /**
     * @param[in] s the text of the label
     * @param[in] a the function that is used to attach the element
     **/
    mediator::wrapper_aptr tool_display_mediator::build_label(std::string const& s) {
      wxStaticText* t = new wxStaticText(current_window, wxID_ANY, wxString(s.c_str(), wxConvLocal));

      return (mediator::wrapper_aptr(new wrapper(t)));
    }
    
    /**
     * @param[in] s the text of the label
     * @param[in] a the function that is used to attach the element
     **/
    mediator::wrapper_aptr tool_display_mediator::build_button(std::string const& s) {
      wxButton* t = new wxButton(current_window, wxID_ANY, wxString(s.c_str(), wxConvLocal));

      return (mediator::wrapper_aptr(new wrapper(t)));
    }
    
    /**
     * @param[in] s the text of the label
     * @param[in] a the function that is used to attach the element
     **/
    mediator::wrapper_aptr tool_display_mediator::build_radio_button(std::string const& s) {
      wxRadioButton* t = new wxRadioButton(current_window, wxID_ANY, wxString(s.c_str(), wxConvLocal));

      return (mediator::wrapper_aptr(new wrapper(t)));
    }
    
    /**
     * @param[in] min the minimum value (if supported)
     * @param[in] max the maximum value (if supported)
     * @param[in] max the current value
     * @param[in] a the function that is used to attach the element
     **/
    mediator::wrapper_aptr tool_display_mediator::build_progress_bar(unsigned int const& min, unsigned int const& max, unsigned int const& c) {
      wxGauge* t = new wxGauge(current_window, wxID_ANY, max - min);

      t->SetValue(c);

      return (mediator::wrapper_aptr(new wrapper(t)));
    }
    
    /**
     * @param[in] s the text of the label
     * @param[in] a the function that is used to attach the element
     **/
    mediator::wrapper_aptr tool_display_mediator::build_text_field(std::string const& s) {
      wxTextCtrl* t = new wxTextCtrl(current_window, wxID_ANY, wxString(s.c_str(), wxConvLocal));

      return (mediator::wrapper_aptr(new wrapper(t)));
    }

    tool_display_mediator::~tool_display_mediator() {
    }

    void tool_display::instantiate() {
      wxSizer* root = GetSizer();

      if (root != 0) {
        delete root;
      }

      root = new wxBoxSizer(wxVERTICAL);

      tool_display_mediator m(this, mediator::wrapper_aptr(new tool_display_mediator::wrapper(root)));
      
      current_layout->get_top_manager()->instantiate(&m);

      SetSizer(root);
    }

    /** \brief Set a new layout description */
    void tool_display::set_layout(sip::layout::tool_display::sptr l) {
      current_layout = l;
current_layout->write(std::cerr);
      context->gui_builder.schedule_update(this);
    }
  }
}


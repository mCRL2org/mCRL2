#include <deque>

#include "gui_project.h"
#include "gui_tool_display.h"

#include <wx/button.h>
#include <wx/checkbox.h>
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
    namespace detail {
      using ::sip::layout::mediator;

      using namespace sip;
      using namespace sip::layout;

      /** \brief Translates a sip layout to a functional wxWidgets layout */
      class tool_display_mediator : public sip::layout::mediator {
        
        public:
     
          /**
           * \brief Wrapper around a wxWindow derived class
           **/
          class wrapper : public sip::layout::mediator::wrapper {
            private:
     
              /** \brief A wxWindow or wxSizer derived object stored in the */
              wxObject* target;
     
              /** \brief Whether that what is wrapped is a window */
              bool is_window;
     
            public:
     
              /** \brief Constructor for wrapping a window */
              inline wrapper(wxWindow*);
     
              /** \brief Constructor for wrapping a sizer */
              inline wrapper(wxSizer*);
     
              /** \brief Get target cast to a window pointer */
              inline wxWindow* get_window() const;
     
              /** \brief Get target cast to a window pointer */
              inline wxSizer*  get_sizer() const;
     
              /** \brief Whether a window is wrapped */
              inline bool wraps_window() const;
     
              /** \brief Whether a sizer is wrapped */
              inline bool wraps_sizer() const;
          };

        private:
     
          /** \brief The sizer to which the elements will be attached */
          wxWindow*             current_window;

          /** \brief The object to which change event handlers are connected */
          state_change_handler* change_event_handler;
     
        private:
     
          /** \brief Helper function for layout managers to attach widgets */
          void attach_to_vertical_box(mediator::wrapper_aptr, sip::layout::constraints const*);
     
          /** \brief Helper function for layout managers to attach widgets */
          void attach_to_horizontal_box(mediator::wrapper_aptr, sip::layout::constraints const*);
     
        public:
     
          /** \brief Constructor */
          inline tool_display_mediator(wxWindow* w, wrapper_aptr d, state_change_handler* s);
     
          /** \brief Instantiates a vertically aligned box layout manager */
          inline mediator::aptr build_vertical_box();
        
          /** \brief Instantiates a horizonally aligned box layout manager */
          inline mediator::aptr build_horizontal_box();
        
          /** \brief Instantiates a label (static text) */
          inline mediator::wrapper_aptr build_label(layout::element*, std::string const&);
        
          /** \brief Instantiates a label */
          inline mediator::wrapper_aptr build_button(layout::element*, std::string const&);
        
          /** \brief Instantiates a single radio button */
          inline mediator::wrapper_aptr build_radio_button(layout::element*, std::string const&);
        
          /** \brief Instantiates a single checkbox */
          inline mediator::wrapper_aptr build_checkbox(layout::element*, std::string const&, const bool);
     
          /** \brief Instantiates a progress bar */
          inline mediator::wrapper_aptr build_progress_bar(layout::element*, unsigned int const&, unsigned int const&, unsigned int const&);
        
          /** \brief Instantiates a single line text input control */
          inline mediator::wrapper_aptr build_text_field(layout::element*, std::string const&);
     
          /** \brief Destructor */
          inline ~tool_display_mediator();
      };

      /**
       * @param w the window to which to attach elements
       * @param d the sizer to which elements should be attached
       **/
      tool_display_mediator::tool_display_mediator(wxWindow* w, wrapper_aptr d, state_change_handler* s) :
                                                        mediator(d), current_window(w), change_event_handler(s) {
      }
     
      /**
       * @param w the window or sizer to which to attach elements
       **/
      inline tool_display_mediator::wrapper::wrapper(wxWindow* w) : target(w), is_window(true) {
      }
     
      /**
       * @param w the window or sizer to which to attach elements
       **/
      inline tool_display_mediator::wrapper::wrapper(wxSizer* s) : target(s), is_window(false) {
      }
     
      inline wxWindow* tool_display_mediator::wrapper::get_window() const {
        return (static_cast < wxWindow* > (target));
      }
     
      inline wxSizer*  tool_display_mediator::wrapper::get_sizer() const {
        return (static_cast < wxSizer* > (target));
      }
     
      inline bool tool_display_mediator::wrapper::wraps_window() const {
        return (is_window);
      }
     
      inline bool tool_display_mediator::wrapper::wraps_sizer() const {
        return (!is_window);
      }
     
      /**
       * @param p pointer to the data that represent the window to be attached
       * @param c layout constraints
       **/
      void tool_display_mediator::attach_to_vertical_box(mediator::wrapper_aptr d, sip::layout::constraints const* c) {
        wrapper* sd     = static_cast < wrapper* > (d.get());
        int      flags  = wxLEFT|wxRIGHT;
     
        layout::constraints const& cr = *(static_cast < layout::constraints const* > (c));
     
        using sip::layout::box;
     
        switch (cr.alignment_horizontal) {
          case layout::left:
            flags |= wxALIGN_LEFT;
            break;
          case layout::right:
            flags |= wxALIGN_RIGHT;
            break;
          default: /* center */
            flags |= wxALIGN_CENTER;
            break;
        }
     
        wxSizer* sizer = static_cast < wrapper* > (data.get())->get_sizer();
     
        if (0 < cr.margin.top) {
          sizer->AddSpacer(cr.margin.bottom);
        }
     
        wxSizerItem* new_sizer_item;
     
        if (sd->wraps_window()) {
          new_sizer_item = sizer->Add(sd->get_window(), 0, flags, (cr.margin.left + cr.margin.right) >> 1);
        }
        else {
          new_sizer_item = sizer->Add(sd->get_sizer(), 0, flags, (cr.margin.left + cr.margin.right) >> 1);
        }
     
        if (0 < cr.margin.bottom) {
          sizer->AddSpacer(cr.margin.bottom);
        }
     
        if (cr.visible == sip::layout::hidden) {
          new_sizer_item->Show(false);
        }
      }
     
      /**
       * @param p pointer to the data that represent the window to be attached
       * @param c layout constraints
       **/
      void tool_display_mediator::attach_to_horizontal_box(wrapper_aptr d, sip::layout::constraints const* c) {
        wrapper* sd     = static_cast < wrapper* > (d.get());
        int      flags  = wxTOP|wxBOTTOM;
     
        sip::layout::constraints const& cr = *(static_cast < layout::constraints const* > (c));
     
        switch (cr.alignment_vertical) {
          case sip::layout::top:
            flags |= wxALIGN_TOP;
            break;
          case sip::layout::bottom:
            flags |= wxALIGN_BOTTOM;
            break;
          default: /* center */
            flags |= wxALIGN_CENTER;
            break;
        }
     
        wxSizer* sizer = static_cast < wrapper* > (data.get())->get_sizer();
     
        if (0 < cr.margin.left) {
          sizer->AddSpacer(cr.margin.left);
        }
     
        wxSizerItem* new_sizer_item;
     
        if (sd->wraps_window()) {
          new_sizer_item = sizer->Add(sd->get_window(), 0, flags, (cr.margin.top + cr.margin.bottom) >> 1);
        }
        else {
          new_sizer_item = sizer->Add(sd->get_sizer(), 0, flags, (cr.margin.top + cr.margin.bottom) >> 1);
        }
     
        if (0 < cr.margin.right) {
          sizer->AddSpacer(cr.margin.right);
        }
     
        if (cr.visible == sip::layout::hidden) {
          new_sizer_item->Show(false);
        }
      }
     
      /**
       * \return a standard auto pointer to a mediator object with current the mediator with which to attach the children
       **/
      mediator::aptr tool_display_mediator::build_vertical_box() {
        wxSizer* t = new wxBoxSizer(wxVERTICAL);
     
        sip::layout::mediator::aptr m(new tool_display_mediator(current_window, wrapper_aptr(new wrapper(t)), change_event_handler));
     
        m->set_attach(boost::bind(&tool_display_mediator::attach_to_vertical_box,
                                                  static_cast < tool_display_mediator* > (m.get()), _1, _2));
     
        return (m);
      }
     
      /**
       * \return a standard auto pointer to a mediator object with current the mediator with which to attach the children
       **/
      mediator::aptr tool_display_mediator::build_horizontal_box() {
        wxSizer* t = new wxBoxSizer(wxHORIZONTAL);
     
        sip::layout::mediator::aptr m(new tool_display_mediator(current_window, wrapper_aptr (new wrapper(t)), change_event_handler));
     
        m->set_attach(boost::bind(&tool_display_mediator::attach_to_horizontal_box,
                                                  static_cast < tool_display_mediator* > (m.get()), _1, _2));
     
        return (m);
      }
     
      /**
       * @param[in] s the text of the label
       **/
      mediator::wrapper_aptr tool_display_mediator::build_label(layout::element* e, std::string const& s) {
        wxStaticText* t = new wxStaticText(current_window, wxID_ANY, wxString(s.c_str(), wxConvLocal));
     
        return (mediator::wrapper_aptr(new wrapper(t)));
      }
     
      /**
       * @param[in] s the text of the label
       **/
      mediator::wrapper_aptr tool_display_mediator::build_button(layout::element* e, std::string const& s) {
        wxButton* t;
     
        if (s == "OK") {
          t = new wxButton(current_window, wxID_OK);
        }
        else if (s == "Cancel") {
          t = new wxButton(current_window, wxID_CANCEL);
        }
        else {
          t = new wxButton(current_window, wxID_ANY, wxString(s.c_str(), wxConvLocal));
        }
     
        /* Connect change event */
        change_event_handler->associate(t, e);

        current_window->Connect(t->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
                  wxCommandEventHandler(state_change_handler::button_clicked), 0, change_event_handler);
     
        return (mediator::wrapper_aptr(new wrapper(t)));
      }

      /**
       * @param[in] s the text of the label
       **/
      mediator::wrapper_aptr tool_display_mediator::build_radio_button(layout::element* e, std::string const& s) {
        wxRadioButton* t = new wxRadioButton(current_window, wxID_ANY, wxString(s.c_str(), wxConvLocal));
     
        /* Connect change event */
        change_event_handler->associate(t, e);

        current_window->Connect(t->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED,
                  wxCommandEventHandler(state_change_handler::radio_button_selected), 0, change_event_handler);
     
        return (mediator::wrapper_aptr(new wrapper(t)));
      }
     
      /**
       * @param[in] s the text of the label
       * @param[in] s the status of the checkbox
       **/
      mediator::wrapper_aptr tool_display_mediator::build_checkbox(layout::element* e, std::string const& s, const bool b) {
        wxCheckBox* t = new wxCheckBox(current_window, wxID_ANY, wxString(s.c_str(), wxConvLocal));
     
        t->SetValue(b);
     
        /* Connect change event */
        change_event_handler->associate(t, e);

        current_window->Connect(t->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED,
                  wxCommandEventHandler(state_change_handler::checkbox_clicked), 0, change_event_handler);
     
        return (mediator::wrapper_aptr(new wrapper(t)));
      }
     
      /**
       * @param[in] min the minimum value (if supported)
       * @param[in] max the maximum value (if supported)
       * @param[in] max the current value
       **/
      mediator::wrapper_aptr tool_display_mediator::build_progress_bar(layout::element* e, unsigned int const& min, unsigned int const& max, unsigned int const& c) {
        wxGauge* t = new wxGauge(current_window, wxID_ANY, max - min);
     
        t->SetValue(c);
     
        return (mediator::wrapper_aptr(new wrapper(t)));
      }
     
      /**
       * @param[in] s the text of the label
       **/
      mediator::wrapper_aptr tool_display_mediator::build_text_field(layout::element* e, std::string const& s) {
        wxTextCtrl* t = new wxTextCtrl(current_window, wxID_ANY, wxString(s.c_str(), wxConvLocal));
     
        /* Connect change event */
        change_event_handler->associate(t, e);

        current_window->Connect(t->GetId(), wxEVT_COMMAND_TEXT_UPDATED,
                  wxCommandEventHandler(state_change_handler::text_field_changed), 0, change_event_handler);
     
        return (mediator::wrapper_aptr(new wrapper(t)));
      }
     
      tool_display_mediator::~tool_display_mediator() {
      }

      void state_change_handler::button_clicked(wxCommandEvent& e) {
std::cerr << "YEEEEHAAAA button\n";
//        layout::elements::button* b = static_cast < layout::elements::button* > (element_for_window[e.GetEventObject()]);
      }
     
      void state_change_handler::radio_button_selected(wxCommandEvent& e) {
//        layout::elements::radio_button* r = static_cast < layout::elements::radio_button* > (element_for_window[e.GetEventObject()]);
      }
     
      void state_change_handler::checkbox_clicked(wxCommandEvent& e) {
//        layout::elements::checkbox* c = static_cast < layout::elements::checkbox* > (element_for_window[e.GetEventObject()]);
std::cerr << "YEEEEHAAAA checkbox\n";
      }
     
      void state_change_handler::text_field_changed(wxCommandEvent& e) {
//        layout::elements::text_field* t = static_cast < layout::elements::text_field* > (element_for_window[e.GetEventObject()]);
      }
    }

    void tool_display::build() {
      wxSizer* root(new wxBoxSizer(wxVERTICAL));

      /* Set minimum dimensions */
      root->SetMinSize(GetClientSize().GetWidth(), 50);

      SetSizer(root);
    }

    void tool_display::remove() {
      wxSizer* s = GetParent()->GetSizer();
      
      s->Detach(this);
      s->Layout();

      Destroy();
    }

    /**
     * @param[in] l the new layout specification
     **/
    void tool_display::instantiate(sip::layout::tool_display::sptr l) {
      using namespace detail;

      if (content != 0) {
        GetSizer()->Detach(content);

        delete content;
      }

      event_handler.clear();

      current_layout = l;

      std::auto_ptr < wxSizer > root(new wxBoxSizer(wxVERTICAL));

      tool_display_mediator m(this, mediator::wrapper_aptr(new tool_display_mediator::wrapper(root.get())), &event_handler);
      
      try {
        content = static_cast < tool_display_mediator::wrapper* >
                                                        (current_layout->instantiate(&m).get())->get_sizer();
        GetSizer()->Add(content, 1, wxALL|wxALIGN_CENTER, 2);

        content->RecalcSizes();
        content->Layout();

        Show(true);

        GetParent()->Layout();
        GetParent()->FitInside();
      }
      catch (...) {
        /* Consider every exception a failure to correctly read the layout, and bail */
        remove();
      }
    }

    /**
     * @param[in] l the layout specification
     **/
    void tool_display::set_layout(sip::layout::tool_display::sptr l) {
      context->gui_builder.schedule_update(boost::bind(&tool_display::instantiate, this, l));
    }
  }
}


#include <deque>
#include <utility>

#include "gui_project.h"
#include "gui_tool_display.h"

#include <boost/foreach.hpp>

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/event.h>
#include <wx/gauge.h>
#include <wx/radiobut.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/datetime.h>

#include <sip/detail/layout_mediator.h>
#include <sip/detail/layout_manager.h>

namespace squadt {
  namespace GUI {
    namespace detail {
      using ::sip::layout::mediator;

      using namespace sip;
      using namespace sip::layout;

      #define cmID_MINIMISE  (wxID_HIGHEST + 1)
      #define cmID_CLOSE     (wxID_HIGHEST + 2)

      /** \brief Translates a sip layout to a functional wxWidgets layout */
      class tool_display_mediator : public sip::layout::mediator {
        
        public:
     
          /**
           * \brief Wrapper around a wxWindow derived class
           *
           * Avoids memory leaks by deleting the pointed to window or sizer,
           * unless it was explicitly released with one of the `release_'
           * methods.
           **/
          class wrapper : public sip::layout::mediator::wrapper {
            private:
     
              /** \brief A wxWindow or wxSizer derived object stored in the */
              wxObject* target;
     
              /** \brief Whether that what is wrapped is a window */
              bool is_window;

              /** \brief Whether to clean up should the wrapper be destroyed */
              mutable bool clean_up;
     
            public:
     
              /** \brief Constructor for wrapping a window */
              inline wrapper(wxWindow*);
     
              /** \brief Constructor for wrapping a sizer */
              inline wrapper(wxSizer*);
     
              /** \brief Get target cast to a window pointer */
              inline wxWindow* get_window() const;
     
              /** \brief Get target cast to a window pointer */
              inline wxSizer*  get_sizer() const;
     
              /** \brief Get target cast to a window pointer */
              inline wxWindow* release_window() const;
     
              /** \brief Get target cast to a window pointer */
              inline wxSizer*  release_sizer() const;
     
              /** \brief Whether a window is wrapped */
              inline bool wraps_window() const;
     
              /** \brief Whether a sizer is wrapped */
              inline bool wraps_sizer() const;

              /** \brief Constructor for wrapping a window */
              inline ~wrapper();
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
          inline tool_display_mediator(wxWindow* w, state_change_handler* s);
     
          /** \brief Constructor */
          inline tool_display_mediator(wxWindow* w, wrapper_aptr d, state_change_handler* s);
     
          /** \brief Instantiates a vertically aligned box layout manager */
          inline mediator::aptr build_vertical_box();
        
          /** \brief Instantiates a horizonally aligned box layout manager */
          inline mediator::aptr build_horizontal_box();
        
          /** \brief Instantiates a label (static text) */
          inline mediator::wrapper_aptr build_label(layout::element const*, std::string const&);

          /** \brief Updates a label, (G)UI part */
          void update_label(mediator::wrapper*, std::string const&);

          /** \brief Instantiates a label */
          inline mediator::wrapper_aptr build_button(layout::element const*, std::string const&);
        
          /** \brief Updates a button, (G)UI part */
          void update_button(mediator::wrapper*, std::string const&);

          /** \brief Instantiates a single radio button */
          inline mediator::wrapper_aptr build_radio_button(layout::element const*, std::string const&, bool);
        
          /** \brief Updates a radio button, (G)UI part */
          void update_radio_button(mediator::wrapper*, std::string const&, bool);

          /** \brief Instantiates a single checkbox */
          inline mediator::wrapper_aptr build_checkbox(layout::element const*, std::string const&, const bool);
     
          /** \brief Updates a checkbox, (G)UI part */
          void update_checkbox(mediator::wrapper*, std::string const&, const bool status);

          /** \brief Instantiates a progress bar */
          inline mediator::wrapper_aptr build_progress_bar(layout::element const*, unsigned int const&, unsigned int const&, unsigned int const&);
        
          /** \brief Updates a checkbox, (G)UI part */
          void update_progress_bar(mediator::wrapper*, unsigned int const&, unsigned int const&, unsigned int const&);

          /** \brief Instantiates a single line text input control */
          inline mediator::wrapper_aptr build_text_field(layout::element const*, std::string const&);
     
          /** \brief Updates a radio button, (G)UI part */
          void update_text_field(mediator::wrapper*, std::string const&);

          /** \brief Destructor */
          inline ~tool_display_mediator();
      };

      /**
       * @param w the window to which to attach elements
       * @param s the event handler that deals with change events
       **/
      tool_display_mediator::tool_display_mediator(wxWindow* w, state_change_handler* s) :
                                                        current_window(w), change_event_handler(s) {
      }
     
      /**
       * @param w the window to which to attach elements
       * @param d the sizer to which elements should be attached
       * @param s the event handler that deals with change events
       **/
      tool_display_mediator::tool_display_mediator(wxWindow* w, wrapper_aptr d, state_change_handler* s) :
                                                        mediator(d), current_window(w), change_event_handler(s) {
      }
     
      /**
       * @param w the window or sizer to which to attach elements
       **/
      inline tool_display_mediator::wrapper::wrapper(wxWindow* w) : target(w), is_window(true), clean_up(true) {
      }
     
      /**
       * @param w the window or sizer to which to attach elements
       **/
      inline tool_display_mediator::wrapper::wrapper(wxSizer* s) : target(s), is_window(false), clean_up(true) {
      }
     
      inline wxWindow* tool_display_mediator::wrapper::get_window() const {
        return (static_cast < wxWindow* > (target));
      }
     
      inline wxSizer*  tool_display_mediator::wrapper::get_sizer() const {
        return (static_cast < wxSizer* > (target));
      }
     
      inline wxWindow* tool_display_mediator::wrapper::release_window() const {
        clean_up = false;

        return (static_cast < wxWindow* > (target));
      }
     
      inline wxSizer*  tool_display_mediator::wrapper::release_sizer() const {
        clean_up = false;

        return (static_cast < wxSizer* > (target));
      }
     
      inline bool tool_display_mediator::wrapper::wraps_window() const {
        return (is_window);
      }
     
      inline bool tool_display_mediator::wrapper::wraps_sizer() const {
        return (!is_window);
      }
     
      inline tool_display_mediator::wrapper::~wrapper() {
        if (clean_up) {
          if (is_window) {
            static_cast < wxWindow* > (target)->Destroy();
          }
          else {
            static_cast < wxSizer* > (target)->Clear();
         
            delete target;
          }
        }
      }
     
      /**
       * @param p pointer to the data that represent the window to be attached
       * @param c layout constraints
       **/
      void tool_display_mediator::attach_to_vertical_box(mediator::wrapper_aptr d, sip::layout::constraints const* c) {
        wrapper* sd     = static_cast < wrapper* > (d.get());
        int      flags  = wxLEFT|wxRIGHT;
     
        layout::constraints const& cr = *(static_cast < layout::constraints const* > (c));
     
        if (cr.grow) {
          flags |= wxEXPAND;
        }

        switch (cr.alignment_horizontal) {
          case layout::right:
            flags |= wxALIGN_RIGHT;
            break;
          case layout::center:
            flags |= wxALIGN_CENTER_HORIZONTAL;
            break;
          default:
            flags |= wxALIGN_LEFT;
            break;
        }
     
        wxSizer* sizer = static_cast < wrapper* > (data.get())->get_sizer();
     
        if (0 < cr.margin.top) {
          sizer->AddSpacer(cr.margin.bottom);
        }
     
        wxSizerItem* new_sizer_item;
     
        if (sd->wraps_window()) {
          new_sizer_item = sizer->Add(sd->release_window(), 0, flags, (cr.margin.left + cr.margin.right) >> 1);
        }
        else {
          new_sizer_item = sizer->Add(sd->release_sizer(), 0, flags, (cr.margin.left + cr.margin.right) >> 1);
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
      void tool_display_mediator::attach_to_horizontal_box(mediator::wrapper_aptr d, sip::layout::constraints const* c) {
        wrapper* sd     = static_cast < wrapper* > (d.get());
        int      flags  = wxTOP|wxBOTTOM;

        sip::layout::constraints const& cr = *(static_cast < layout::constraints const* > (c));

        if (cr.grow) {
          flags |= wxEXPAND;
        }

        switch (cr.alignment_vertical) {
          case sip::layout::top:
            flags |= wxALIGN_TOP;
            break;
          case sip::layout::bottom:
            flags |= wxALIGN_BOTTOM;
            break;
          default: /* center */
            flags |= wxALIGN_CENTER_VERTICAL;
            break;
        }

        wxSizer* sizer = static_cast < wrapper* > (data.get())->get_sizer();

        if (0 < cr.margin.left) {
          sizer->AddSpacer(cr.margin.left);
        }

        wxSizerItem* new_sizer_item;

        if (sd->wraps_window()) {
          new_sizer_item = sizer->Add(sd->release_window(), 0, flags, (cr.margin.top + cr.margin.bottom) >> 1);
        }
        else {
          new_sizer_item = sizer->Add(sd->release_sizer(), 0, flags, (cr.margin.top + cr.margin.bottom) >> 1);
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
       * @param[in] e the element that is associated with the new control
       * @param[in] s the text of the label
       **/
      mediator::wrapper_aptr tool_display_mediator::build_label(layout::element const* e, std::string const& s) {
        wxStaticText* t = new wxStaticText(current_window, wxID_ANY, wxString(s.c_str(), wxConvLocal));

        /* Connect change event */
        change_event_handler->associate(t, e);
     
        return (mediator::wrapper_aptr(new wrapper(t)));
      }
     
      /**
       * @param[in] w the element that is associated with the control to update
       * @param[in] s the text of the label
       **/
      void tool_display_mediator::update_label(mediator::wrapper* w, std::string const& s) {
        wxStaticText* t = static_cast < wxStaticText* > (static_cast < wrapper* > (w)->release_window());

        t->SetLabel(wxString(s.c_str(), wxConvLocal));
      }

      /**
       * @param[in] e the element that is associated with the new control
       * @param[in] s the text of the label
       **/
      mediator::wrapper_aptr tool_display_mediator::build_button(layout::element const* e, std::string const& s) {
        wxButton* t;
     
        if (s == "OK") {
          t = new wxButton(current_window, wxID_OK);
        }
        else if (s == "Cancel") {
          t = new wxButton(current_window, wxID_CANCEL);
        }
        else {
          t = new wxButton(current_window, wxID_ANY, wxString(s.c_str(), wxConvLocal), wxDefaultPosition, wxButton::GetDefaultSize());
        }
     
        /* Connect change event */
        change_event_handler->associate(t, e);

        current_window->Connect(t->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
                  wxCommandEventHandler(state_change_handler::button_clicked), 0, change_event_handler);
     
        return (mediator::wrapper_aptr(new wrapper(t)));
      }

      /**
       * @param[in] w the element that is associated with the control to update
       * @param[in] s the text of the label
       **/
      void tool_display_mediator::update_button(mediator::wrapper* w, std::string const& s) {
        wxButton* t = static_cast < wxButton* > (static_cast < wrapper* > (w)->release_window());

        t->SetLabel(wxString(s.c_str(), wxConvLocal));
      }

      /**
       * @param[in] e the element that is associated with the new control
       * @param[in] s the text of the label
       * @param[in] b whether the button is selected or not
       **/
      mediator::wrapper_aptr tool_display_mediator::build_radio_button(layout::element const* e, std::string const& s, bool b) {
        wxRadioButton* t = new wxRadioButton(current_window, wxID_ANY, wxString(s.c_str(), wxConvLocal), wxDefaultPosition, 
                wxDefaultSize, static_cast < layout::elements::radio_button const* > (e)->is_first_in_group() ? wxRB_GROUP : 0);

        if (static_cast < layout::elements::radio_button const* > (e)->is_selected()) {
          t->SetValue(true);
        }
     
        /* Connect change event */
        change_event_handler->associate(t, e);

        current_window->Connect(t->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED,
                  wxCommandEventHandler(state_change_handler::radio_button_selected), 0, change_event_handler);
     
        return (mediator::wrapper_aptr(new wrapper(t)));
      }
     
      /**
       * @param[in] w the element that is associated with the control to update
       * @param[in] s the text of the label
       * @param[in] b whether the button is selected or not
       **/
      void tool_display_mediator::update_radio_button(mediator::wrapper* w, std::string const& s, bool b) {
        wxRadioButton* t = static_cast < wxRadioButton* > (static_cast < wrapper* > (w)->release_window());

        t->SetLabel(wxString(s.c_str(), wxConvLocal));
        t->SetValue(b);
      }

      /**
       * @param[in] e the element that is associated with the new control
       * @param[in] s the text of the label
       * @param[in] b the status of the checkbox
       **/
      mediator::wrapper_aptr tool_display_mediator::build_checkbox(layout::element const* e, std::string const& s, const bool b) {
        wxCheckBox* t = new wxCheckBox(current_window, wxID_ANY, wxString(s.c_str(), wxConvLocal));
     
        t->SetValue(b);
     
        /* Connect change event */
        change_event_handler->associate(t, e);

        current_window->Connect(t->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED,
                  wxCommandEventHandler(state_change_handler::checkbox_clicked), 0, change_event_handler);
     
        return (mediator::wrapper_aptr(new wrapper(t)));
      }
     
      /**
       * @param[in] w the element that is associated with the control to update
       * @param[in] s the text of the label
       * @param[in] b whether the button is selected or not
       **/
      void tool_display_mediator::update_checkbox(mediator::wrapper* w, std::string const& s, bool b) {
        wxCheckBox* t = static_cast < wxCheckBox* > (static_cast < wrapper* > (w)->release_window());

        t->SetLabel(wxString(s.c_str(), wxConvLocal));
        t->SetValue(b);
      }

      /**
       * @param[in] e the element that is associated with the new control
       * @param[in] min the minimum value (if supported)
       * @param[in] max the maximum value (if supported)
       * @param[in] max the current value
       **/
      mediator::wrapper_aptr tool_display_mediator::build_progress_bar(layout::element const* e, unsigned int const& min, unsigned int const& max, unsigned int const& c) {
        wxGauge* t = new wxGauge(current_window, wxID_ANY, max - min, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL);
     
        t->SetValue(c - min);
     
        /* Connect change event */
        change_event_handler->associate(t, e);

        return (mediator::wrapper_aptr(new wrapper(t)));
      }
     
      /**
       * @param[in] w the element that is associated with the control to update
       * @param[in] min the minimum value (if supported)
       * @param[in] max the maximum value (if supported)
       * @param[in] max the current value
       **/
      void tool_display_mediator::update_progress_bar(mediator::wrapper* w, unsigned int const& min, unsigned int const& max, unsigned int const& c) {
        wxGauge* t = static_cast < wxGauge* > (static_cast < wrapper* > (w)->release_window());

        t->SetRange(max - min);
        t->SetValue(c - min);
      }

      /**
       * @param[in] e the element that is associated with the new control
       * @param[in] s the text of the label
       **/
      mediator::wrapper_aptr tool_display_mediator::build_text_field(layout::element const* e, std::string const& s) {
        wxTextCtrl* t = new wxTextCtrl(current_window, wxID_ANY, wxString(s.c_str(), wxConvLocal), wxDefaultPosition, wxSize(200,-1));
     
        /* Connect change event */
        change_event_handler->associate(t, e);

        current_window->Connect(t->GetId(), wxEVT_COMMAND_TEXT_UPDATED,
                  wxCommandEventHandler(state_change_handler::text_field_changed), 0, change_event_handler);
     
        return (mediator::wrapper_aptr(new wrapper(t)));
      }
     
      /**
       * @param[in] w the element that is associated with the control to update
       * @param[in] s the text of the label
       **/
      void tool_display_mediator::update_text_field(mediator::wrapper* w, std::string const& s) {
        wxStaticText* t = static_cast < wxStaticText* > (static_cast < wrapper* > (w)->release_window());

        t->SetLabel(wxString(s.c_str(), wxConvLocal));
      }

      tool_display_mediator::~tool_display_mediator() {
      }

      void state_change_handler::button_clicked(wxCommandEvent& e) {
        layout::elements::button const* b = static_cast < layout::elements::button const* > (element_for_window[e.GetEventObject()]);

        monitor->send_display_data(b);
      }
     
      void state_change_handler::radio_button_selected(wxCommandEvent& e) {
        wxRadioButton*                  wxr = static_cast < wxRadioButton* > (e.GetEventObject());
        layout::elements::radio_button* r   = const_cast < layout::elements::radio_button* >
                        (static_cast < layout::elements::radio_button const* > (element_for_window[e.GetEventObject()]));

        r->set_selected(wxr->GetValue());

        monitor->send_display_data(r);
      }
     
      void state_change_handler::checkbox_clicked(wxCommandEvent& e) {
        wxCheckBox*                 wxc = static_cast < wxCheckBox* > (e.GetEventObject());
        layout::elements::checkbox* c   = const_cast < layout::elements::checkbox* >
                        (static_cast < layout::elements::checkbox const* > (element_for_window[wxc]));

        c->set_status(wxc->GetValue());

        monitor->send_display_data(c);
      }
     
      void state_change_handler::text_field_changed(wxCommandEvent& e) {
        wxTextCtrl*                   wxt = static_cast < wxTextCtrl* > (e.GetEventObject());
        layout::elements::text_field* t   = const_cast < layout::elements::text_field* >
                        (static_cast < layout::elements::text_field const* > (element_for_window[wxt]));

        t->set_text(std::string(wxt->GetValue().fn_str()));

        monitor->send_display_data(t);
      }

      /**
       * @param[in] m pointer to a mediator for translation to wxWidgets controls
       * @param[in] e pointer to the element of which the status was changed
       **/
      void state_change_handler::update(sip::layout::mediator* m, sip::layout::element const* e) {
        element_for_window_map::const_iterator i = std::find_if(element_for_window.begin(), element_for_window.end(), 
                boost::bind(std::equal_to< sip::layout::element const* >(), e, boost::bind(&element_for_window_map::value_type::second, _1)));

        if (i != element_for_window.end()) {
          tool_display_mediator::wrapper w(static_cast < wxWindow* > ((*i).first));

          (*i).second->update(m, &w);
        }
      }
    }

    /**
     * @param[in] p the parent window
     * @param[in] c the project that owns tool display
     * @param[in] s the processor associated with this display
     **/
    tool_display::tool_display(wxWindow* p, GUI::project* c, processor::monitor::sptr& s) :
                                wxPanel(p, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER),
                                context(c), event_handler(s), current_layout(new sip::layout::tool_display), content(0), log(0) {

      build();

      Show(false);

      /* Connect event handlers */
      s->set_display_layout_handler(boost::bind(&GUI::tool_display::schedule_layout_change, this, _1));
      s->set_status_message_handler(boost::bind(&GUI::tool_display::schedule_log_update, this, _1));
    }

    void tool_display::build() {
      wxSizer* root(new wxBoxSizer(wxVERTICAL));

      /* Set minimum dimensions */
      root->SetMinSize(GetClientSize().GetWidth(), 110);

      wxWindow* titlepanel(new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER));

      control_bar = new wxBoxSizer(wxHORIZONTAL);
      control_bar->AddSpacer(4);
      control_bar->Add(new wxStaticText(titlepanel, wxID_ANY, wxT("")), 0, wxTOP|wxBOTTOM|wxLEFT, 2);
      control_bar->AddStretchSpacer(1);
      control_bar->Add(new wxButton(titlepanel, cmID_MINIMISE, wxT("-"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 0, wxALIGN_RIGHT);
      control_bar->Add(new wxButton(titlepanel, cmID_CLOSE, wxT("x"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 0, wxALIGN_RIGHT);

      titlepanel->SetSizer(control_bar);
      root->Add(titlepanel, 0, wxEXPAND);

      SetSizer(root);

      Connect(cmID_MINIMISE, cmID_CLOSE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(tool_display::on_panel_button_clicked));
    }

    void tool_display::set_title(wxString s) {
      control_bar->GetItem(1)->GetWindow()->SetLabel(s);
    }

    void tool_display::on_panel_button_clicked(wxCommandEvent& e) {
      if (e.GetId() == cmID_MINIMISE) {
        if (content != 0) {
          wxSizer* root = GetSizer();

          if (root->IsShown(content)) {
            root->SetMinSize(GetClientSize().GetWidth(), root->GetItem((size_t) 0)->GetSize().GetHeight());

            content->Show(false);

            if (log != 0) {
              log->Show(false);
            }
          }
          else {
            root->SetMinSize(GetClientSize().GetWidth(), 110);

            content->Show(true);

            if (log != 0) {
              log->Show(true);
            }
          }

          GetParent()->Layout();
        }
      }
      else {
        remove();
      }
    }

    void tool_display::remove() {
      /* Ignore all scheduled updates to the tool display */
      current_layout.reset();

      wxSizer* s = GetParent()->GetSizer();
      
      s->Show(this, false, true);
      s->Detach(this);
      s->Layout();

      event_handler.get_monitor()->reset_display_layout_handler();
      event_handler.get_monitor()->reset_status_message_handler();

      /* Toggle scrollbar availability on demand */
      wxSizeEvent size_event(GetParent()->GetSize(), GetParent()->GetId());

      size_event.SetEventObject(GetParent());

      GetParent()->GetParent()->ProcessEvent(size_event);

      /* End tool execution, if it was still running */
      event_handler.get_monitor()->finish();

      Destroy();
    }

    /**
     * @param[in] l the new layout specification
     **/
    void tool_display::instantiate(boost::weak_ptr < sip::layout::tool_display > w, sip::layout::tool_display::sptr l) {
      using namespace detail;

      boost::shared_ptr < sip::layout::tool_display > g(w.lock());

      if (g.get() != 0) {
        if (content != 0) {
          /* Delete controls */
          content->Clear(true);

          GetSizer()->Detach(content);

          delete content;
        }

        current_layout = l;

        std::auto_ptr < wxSizer > root(new wxBoxSizer(wxVERTICAL));

        tool_display_mediator m(this, mediator::wrapper_aptr(new tool_display_mediator::wrapper(root.get())), &event_handler);

        try {
          mediator::wrapper_aptr new_layout(current_layout->instantiate(&m));

          if (new_layout.get() != 0) {
            content = static_cast < tool_display_mediator::wrapper* > (new_layout.get())->release_sizer();

            GetSizer()->Insert(1, content, 1, wxALL|wxALIGN_LEFT, 2);

            content->Layout();

            Show(true);
          }
          else {
            content = 0;
          }

          GetParent()->Layout();

          /* Toggle scrollbar availability on demand */
          wxSizeEvent size_event(GetParent()->GetSize(), GetParent()->GetId());

          size_event.SetEventObject(GetParent());

          GetParent()->GetParent()->ProcessEvent(size_event);
        }
        catch (...) {
          /* Consider every exception a failure to correctly read the layout, and bail */
          std::cerr << "fatal: layout translation of layout specification failed!";

          remove();
        }
      }
    }

    /**
     * @param[in] l the layout elements that have changed
     **/
    void tool_display::update(boost::weak_ptr < sip::layout::tool_display > w, std::vector < sip::layout::element const* > l) {
      using namespace detail;

      boost::shared_ptr < sip::layout::tool_display > g(w.lock());

      if (g.get() != 0) {
        tool_display_mediator m(this, &event_handler);

        BOOST_FOREACH(sip::layout::element const* i, l) {
          event_handler.update(&m, i);
        }

        GetParent()->Layout();
      }
    }

    /**
     * @param[in] l the layout elements that have changed
     **/
    void tool_display::update_log(boost::weak_ptr < sip::layout::tool_display > w, sip::report::sptr l) {
      boost::shared_ptr < sip::layout::tool_display > g(w.lock());

      if (g.get() != 0) {
        wxString stamp = wxDateTime::Now().Format(wxT("%b %e %H:%M:%S "));

        if (log == 0) {
          wxSizer* sizer = GetSizer();

          log = new wxTextCtrl(this, wxID_ANY, stamp + wxString(l->get_description().c_str(), wxConvLocal), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY);
          log->SetSize(-1, 40);

          sizer->Add(log, 0, wxALL|wxEXPAND|wxALIGN_CENTER, 2);

          GetParent()->Layout();

          /* Toggle scrollbar availability on demand */
          wxSizeEvent size_event(GetParent()->GetSize(), GetParent()->GetId());

          size_event.SetEventObject(GetParent());

          GetParent()->GetParent()->ProcessEvent(size_event);
        }
        else {
          log->AppendText(stamp + wxString(l->get_description().c_str(), wxConvLocal));

          log->ShowPosition(log->GetLastPosition());
        }
      }
    }

    /**
     * @param[in] l the layout specification
     **/
    void tool_display::schedule_log_update(sip::report::sptr l) {
      context->gui_builder.schedule_update(boost::bind(&tool_display::update_log, this, current_layout, l));
    }

    /**
     * @param[in] l the layout specification
     **/
    void tool_display::schedule_layout_change(sip::layout::tool_display::sptr l) {
      /** Register handler for updates */
      event_handler.clear();
      event_handler.get_monitor()->set_display_data_handler(l, boost::bind(&GUI::tool_display::schedule_layout_update, this, _1));

      context->gui_builder.schedule_update(boost::bind(&tool_display::instantiate, this, current_layout, l));
    }

    void tool_display::schedule_layout_update(std::vector < sip::layout::element const* > const& l) {
      context->gui_builder.schedule_update(boost::bind(&tool_display::update, this, current_layout, l));
    }
  }
}


// Author(s): Jeroen van der Wulp
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file gui/tool_display.cpp
/// \brief Add your file description here.

#include <deque>
#include <utility>

#include "gui/project.hpp"
#include "gui/tool_display.hpp"

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

#include "tipi/detail/layout_mediator.hpp"
#include "tipi/detail/layout_manager.hpp"

namespace squadt {
  namespace GUI {
    namespace detail {
      using ::tipi::layout::mediator;

      using namespace tipi;
      using namespace tipi::layout;

      #define cmID_MINIMISE  (wxID_HIGHEST + 1)
      #define cmID_CLOSE     (wxID_HIGHEST + 2)

      /** \brief Translates a tipi layout to a functional wxWidgets layout */
      class tool_display_mediator : public tipi::layout::mediator {
        
        public:
     
          /**
           * \brief Wrapper around a wxWindow derived class
           *
           * Avoids memory leaks by deleting the pointed to window or sizer,
           * unless it was explicitly released with one of the `release_'
           * methods.
           **/
          class wrapper : public tipi::layout::mediator::wrapper {
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
          void attach_to_vertical_box(mediator::wrapper_aptr, tipi::layout::properties const*);
     
          /** \brief Helper function for layout managers to attach widgets */
          void attach_to_horizontal_box(mediator::wrapper_aptr, tipi::layout::properties const*);
     
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
          inline mediator::wrapper_aptr build_checkbox(layout::element const*, std::string const&, bool);
     
          /** \brief Updates a checkbox, (G)UI part */
          void update_checkbox(mediator::wrapper*, std::string const&, bool status);

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
       * Helper class for associating objects of tipi::layout::element with
       * their wxWidgets counterpart
       *
       * This class would not have been necessary if wxWidgets provided
       * functionality to register function objects as event handlers
       **/
      template < typename S >
      class event_helper : public wxEvtHandler, public wxClientData {

        protected:

          S& tipi_element;

        public:

          event_helper(tipi::layout::element& s) : tipi_element(static_cast < S& > (s)) {
          }

          void do_changes(wxCommandEvent&);
      };

      /**
       * \param w the window to which to attach elements
       * \param s the event handler that deals with change events
       **/
      tool_display_mediator::tool_display_mediator(wxWindow* w, state_change_handler* s) :
                                                        current_window(w), change_event_handler(s) {
      }
     
      /**
       * \param w the window to which to attach elements
       * \param d the sizer to which elements should be attached
       * \param s the event handler that deals with change events
       **/
      tool_display_mediator::tool_display_mediator(wxWindow* w, wrapper_aptr d, state_change_handler* s) :
                                                        mediator(d), current_window(w), change_event_handler(s) {
      }
     
      /**
       * \param w the window or sizer to which to attach elements
       **/
      inline tool_display_mediator::wrapper::wrapper(wxWindow* w) : target(w), is_window(true), clean_up(true) {
      }
     
      /**
       * \param s the window or sizer to which to attach elements
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
       * \param p pointer to the data that represent the window to be attached
       * \param c layout properties
       **/
      void tool_display_mediator::attach_to_vertical_box(mediator::wrapper_aptr d, tipi::layout::properties const* c) {
        wrapper* sd     = static_cast < wrapper* > (d.get());
        int      flags  = wxLEFT|wxRIGHT;
     
        layout::properties const& cr = *(static_cast < layout::properties const* > (c));
     
        if (cr.m_grow) {
          flags |= wxEXPAND;
        }

        switch (cr.m_alignment_horizontal) {
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
     
        if (0 < cr.m_margin.top) {
          sizer->AddSpacer(cr.m_margin.bottom);
        }
     
        wxSizerItem* new_sizer_item;
     
        if (sd->wraps_window()) {
          new_sizer_item = sizer->Add(sd->release_window(), 0, flags, (cr.m_margin.left + cr.m_margin.right) >> 1);
        }
        else {
          new_sizer_item = sizer->Add(sd->release_sizer(), 0, flags|wxEXPAND, (cr.m_margin.left + cr.m_margin.right) >> 1);
        }
     
        if (0 < cr.m_margin.bottom) {
          sizer->AddSpacer(cr.m_margin.bottom);
        }
     
        if (cr.m_visible == tipi::layout::hidden) {
          new_sizer_item->Show(false);
        }
      }
     
      /**
       * \param p pointer to the data that represent the window to be attached
       * \param c layout properties
       **/
      void tool_display_mediator::attach_to_horizontal_box(mediator::wrapper_aptr d, tipi::layout::properties const* c) {
        wrapper* sd     = static_cast < wrapper* > (d.get());
        int      flags  = wxTOP|wxBOTTOM;

        tipi::layout::properties const& cr = *(static_cast < layout::properties const* > (c));

        if (cr.m_grow) {
          flags |= wxEXPAND;
        }

        switch (cr.m_alignment_vertical) {
          case tipi::layout::top:
            flags |= wxALIGN_TOP;
            break;
          case tipi::layout::bottom:
            flags |= wxALIGN_BOTTOM;
            break;
          default: /* center */
            flags |= wxALIGN_CENTER_VERTICAL;
            break;
        }

        wxSizer* sizer = static_cast < wrapper* > (data.get())->get_sizer();

        if (0 < cr.m_margin.left) {
          sizer->AddSpacer(cr.m_margin.left);
        }

        wxSizerItem* new_sizer_item;

        if (sd->wraps_window()) {
          new_sizer_item = sizer->Add(sd->release_window(), 0, flags, (cr.m_margin.top + cr.m_margin.bottom) >> 1);
        }
        else {
          new_sizer_item = sizer->Add(sd->release_sizer(), 0, flags|wxEXPAND, (cr.m_margin.top + cr.m_margin.bottom) >> 1);
        }

        if (0 < cr.m_margin.right) {
          sizer->AddSpacer(cr.m_margin.right);
        }

        if (cr.m_visible == tipi::layout::hidden) {
          new_sizer_item->Show(false);
        }
      }
     
      /**
       * \return a standard auto pointer to a mediator object with current the mediator with which to attach the children
       **/
      mediator::aptr tool_display_mediator::build_vertical_box() {
        wxSizer* t = new wxBoxSizer(wxVERTICAL);
     
        tipi::layout::mediator::aptr m(new tool_display_mediator(current_window, wrapper_aptr(new wrapper(t)), change_event_handler));
     
        m->set_attach(boost::bind(&tool_display_mediator::attach_to_vertical_box,
                                                  static_cast < tool_display_mediator* > (m.get()), _1, _2));
     
        return (m);
      }
     
      /**
       * \return a standard auto pointer to a mediator object with current the mediator with which to attach the children
       **/
      mediator::aptr tool_display_mediator::build_horizontal_box() {
        wxSizer* t = new wxBoxSizer(wxHORIZONTAL);
     
        tipi::layout::mediator::aptr m(new tool_display_mediator(current_window, wrapper_aptr (new wrapper(t)), change_event_handler));
     
        m->set_attach(boost::bind(&tool_display_mediator::attach_to_horizontal_box,
                                                  static_cast < tool_display_mediator* > (m.get()), _1, _2));
     
        return (m);
      }
     
      /**
       * \param[in] e the element that is associated with the new control
       * \param[in] s the text of the label
       **/
      mediator::wrapper_aptr tool_display_mediator::build_label(layout::element const* e, std::string const& s) {
        wxStaticText* t = new wxStaticText(current_window, wxID_ANY, wxString(s.c_str(), wxConvLocal));

        /* Connect change event */
        change_event_handler->associate(t, e);
     
        return (mediator::wrapper_aptr(new wrapper(t)));
      }
     
      /**
       * \param[in] w the element that is associated with the control to update
       * \param[in] s the text of the label
       **/
      void tool_display_mediator::update_label(mediator::wrapper* w, std::string const& s) {
        wxStaticText* t = static_cast < wxStaticText* > (static_cast < wrapper* > (w)->release_window());

        t->SetLabel(wxString(s.c_str(), wxConvLocal));
      }

      template <>
      void event_helper< layout::elements::button >::do_changes(wxCommandEvent& e) {
        tipi_element.activate();
      }
     
      /**
       * \param[in] e the element that is associated with the new control
       * \param[in] s the text of the label
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

        event_helper< layout::elements::button >* l = new event_helper< layout::elements::button >(const_cast < tipi::layout::element& > (*e));

        t->Connect(t->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
                  wxCommandEventHandler(event_helper< layout::elements::button >::do_changes), l, l);
     
     
        return (mediator::wrapper_aptr(new wrapper(t)));
      }

      /**
       * \param[in] w the element that is associated with the control to update
       * \param[in] s the text of the label
       **/
      void tool_display_mediator::update_button(mediator::wrapper* w, std::string const& s) {
        wxButton* t = static_cast < wxButton* > (static_cast < wrapper* > (w)->release_window());

        t->SetLabel(wxString(s.c_str(), wxConvLocal));
      }

      template <>
      void event_helper< layout::elements::radio_button >::do_changes(wxCommandEvent& e) {
        tipi_element.select();
      }

      /**
       * \param[in] e the element that is associated with the new control
       * \param[in] s the text of the label
       * \param[in] b whether the button is selected or not
       **/
      mediator::wrapper_aptr tool_display_mediator::build_radio_button(layout::element const* e, std::string const& s, bool b) {
        wxRadioButton* t = new wxRadioButton(current_window, wxID_ANY, wxString(s.c_str(), wxConvLocal), wxDefaultPosition, 
                wxDefaultSize, static_cast < layout::elements::radio_button const* > (e)->is_first_in_group() ? wxRB_GROUP : 0);

        if (static_cast < layout::elements::radio_button const* > (e)->is_selected()) {
          t->SetValue(true);
        }
     
        /* Connect change event */
        change_event_handler->associate(t, e);

        event_helper< layout::elements::radio_button >* l = new event_helper< layout::elements::radio_button >(const_cast < tipi::layout::element& > (*e));

        t->Connect(t->GetId(), wxEVT_COMMAND_RADIOBUTTON_SELECTED,
                  wxCommandEventHandler(event_helper< layout::elements::radio_button >::do_changes), l, l);
     
        return (mediator::wrapper_aptr(new wrapper(t)));
      }
     
      /**
       * \param[in] w the element that is associated with the control to update
       * \param[in] s the text of the label
       * \param[in] b whether the button is selected or not
       **/
      void tool_display_mediator::update_radio_button(mediator::wrapper* w, std::string const& s, bool b) {
        wxRadioButton* t = static_cast < wxRadioButton* > (static_cast < wrapper* > (w)->release_window());

        t->SetLabel(wxString(s.c_str(), wxConvLocal));
        t->SetValue(b);
      }

      template <>
      void event_helper< layout::elements::checkbox >::do_changes(wxCommandEvent& e) {
        tipi_element.set_status(static_cast < wxCheckBox* > (e.GetEventObject())->GetValue());
      }

      /**
       * \param[in] e the element that is associated with the new control
       * \param[in] s the text of the label
       * \param[in] b the status of the checkbox
       **/
      mediator::wrapper_aptr tool_display_mediator::build_checkbox(layout::element const* e, std::string const& s, bool b) {
        wxCheckBox* t = new wxCheckBox(current_window, wxID_ANY, wxString(s.c_str(), wxConvLocal));
     
        t->SetValue(b);
     
        /* Connect change event */
        change_event_handler->associate(t, e);

        event_helper< layout::elements::checkbox >* l = new event_helper< layout::elements::checkbox >(const_cast < tipi::layout::element& > (*e));

        t->Connect(t->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED,
                  wxCommandEventHandler(event_helper< layout::elements::checkbox >::do_changes), l, l);
     
        return (mediator::wrapper_aptr(new wrapper(t)));
      }
     
      /**
       * \param[in] w the element that is associated with the control to update
       * \param[in] s the text of the label
       * \param[in] b whether the button is selected or not
       **/
      void tool_display_mediator::update_checkbox(mediator::wrapper* w, std::string const& s, bool b) {
        wxCheckBox* t = static_cast < wxCheckBox* > (static_cast < wrapper* > (w)->release_window());

        t->SetLabel(wxString(s.c_str(), wxConvLocal));
        t->SetValue(b);
      }

      /**
       * \param[in] e the element that is associated with the new control
       * \param[in] min the minimum value (if supported)
       * \param[in] max the maximum value (if supported)
       * \param[in] c the current value
       **/
      mediator::wrapper_aptr tool_display_mediator::build_progress_bar(layout::element const* e, unsigned int const& min, unsigned int const& max, unsigned int const& c) {
        wxGauge* t = new wxGauge(current_window, wxID_ANY, max - min, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL);
     
        // SetRange() avoids a bug with range setting in the constructor (wx-2.8)
        t->SetRange(max - min);
        t->SetValue(c - min);
     
        /* Connect change event */
        change_event_handler->associate(t, e);

        return (mediator::wrapper_aptr(new wrapper(t)));
      }
     
      /**
       * \param[in] w the element that is associated with the control to update
       * \param[in] min the minimum value (if supported)
       * \param[in] max the maximum value (if supported)
       * \param[in] c the current value
       **/
      void tool_display_mediator::update_progress_bar(mediator::wrapper* w, unsigned int const& min, unsigned int const& max, unsigned int const& c) {
        wxGauge* t = static_cast < wxGauge* > (static_cast < wrapper* > (w)->release_window());

        t->SetRange(max - min);
        t->SetValue(c - min);
      }

      template <>
      void event_helper< layout::elements::text_field >::do_changes(wxCommandEvent& e) {
        tipi_element.set_text(std::string(static_cast < wxTextCtrl* > (e.GetEventObject())->GetValue().fn_str()));
      }

      /**
       * \param[in] e the element that is associated with the new control
       * \param[in] s the text of the label
       **/
      mediator::wrapper_aptr tool_display_mediator::build_text_field(layout::element const* e, std::string const& s) {
        wxTextCtrl* t = new wxTextCtrl(current_window, wxID_ANY, wxString(s.c_str(), wxConvLocal), wxDefaultPosition, wxSize(200,-1));
     
        /* Connect change event */
        change_event_handler->associate(t, e);

        event_helper< layout::elements::text_field >* l = new event_helper< layout::elements::text_field >(const_cast < tipi::layout::element& > (*e));

        t->Connect(t->GetId(), wxEVT_COMMAND_TEXT_UPDATED,
                  wxCommandEventHandler(event_helper< layout::elements::text_field >::do_changes), l, l);
     
        return (mediator::wrapper_aptr(new wrapper(t)));
      }
     
      /**
       * \param[in] w the element that is associated with the control to update
       * \param[in] s the text of the label
       **/
      void tool_display_mediator::update_text_field(mediator::wrapper* w, std::string const& s) {
        wxStaticText* t = static_cast < wxStaticText* > (static_cast < wrapper* > (w)->release_window());

        t->SetLabel(wxString(s.c_str(), wxConvLocal));
      }

      tool_display_mediator::~tool_display_mediator() {
      }

      /**
       * \param[in] m pointer to a mediator for translation to wxWidgets controls
       * \param[in] e pointer to the element of which the status was changed
       **/
      void state_change_handler::update(tipi::layout::mediator* m, tipi::layout::element const* e) {
        for (element_for_window_map::const_iterator i = element_for_window.begin(); i != element_for_window.end(); ++i) {
          if (i->second == e) {
            tool_display_mediator::wrapper w(static_cast < wxWindow* > ((*i).first));
         
            (*i).second->update(m, &w);
          }
        }
      }
    }

    /**
     * \param[in] p the parent window
     * \param[in] c the project that owns tool display
     * \param[in] s the processor associated with this display
     **/
    tool_display::tool_display(wxWindow* p, GUI::project* c, boost::shared_ptr < processor::monitor >& s) :
                                wxPanel(p, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER),
                                context(c), event_handler(s), current_layout(new tipi::layout::tool_display), content(0), log(0) {

      build();

      /* Connect event handlers */
      s->set_display_layout_handler(boost::bind(&GUI::tool_display::schedule_layout_change, this, _1));
      s->set_status_message_handler(boost::bind(&GUI::tool_display::schedule_log_update, this, _1));
      event_handler.send_display_update = boost::bind(&tipi::controller::communicator::send_display_update, boost::static_pointer_cast < tipi::controller::communicator > (s).get(), _1, boost::static_pointer_cast < tipi::display const > (current_layout));
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

      // Set state to minimised
      root->SetMinSize(GetClientSize().GetWidth(), root->GetItem((size_t) 0)->GetSize().GetHeight());

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
      else if (e.GetId() == cmID_CLOSE) {
        GetParent()->GetSizer()->Show(this, false, true);

        remove();
      }
    }

    tool_display::~tool_display() {
      remove(false);
    }

    /**
     * \param[in] u whether or not to schedule a GUI update
     **/
    void tool_display::remove(bool u) {
      struct local {
        static void trampoline(tool_display* display) {
          display->current_layout.reset();

          wxSizer* s = display->GetParent()->GetSizer();

          s->Show(display, false, true);
          s->Detach(display);
          s->Layout();
          display->GetParent()->Refresh();

          display->toggle_scrollbar_helper();

          display->Destroy();
        }
      };

      /* Ignore all scheduled updates to the tool display */
      event_handler.get_monitor()->reset_display_update_handler();
      event_handler.get_monitor()->reset_display_layout_handler();
      event_handler.get_monitor()->reset_status_message_handler();

      if (u) {
        context->gui_builder.schedule_update(boost::bind(&local::trampoline, this));
      }

      /* End tool execution, if it was still running */
      event_handler.get_monitor()->finish();
    }

      /* Toggle scrollbar availability on demand */
    void tool_display::toggle_scrollbar_helper() {
      wxSizeEvent size_event(GetParent()->GetSize(), GetParent()->GetId());

      size_event.SetEventObject(GetParent());

      GetParent()->GetParent()->ProcessEvent(size_event);
    }

    /**
     * \param[in] w weak pointer to the interface object
     * \param[in] l the new tool_display object
     **/
    void tool_display::instantiate(boost::weak_ptr < tipi::layout::tool_display > w, tipi::layout::tool_display::sptr l) {
      using namespace detail;

      boost::shared_ptr < tipi::layout::tool_display > g(w.lock());

      if (g.get() != 0) {
        if (content != 0) {
          /* Forcibly refresh display to prevent a crash on Mac OS X */
          Refresh();
          Update();
          wxTheApp->Yield();

          Freeze();

          /* Delete controls */
          content->Clear(true);

          GetSizer()->Detach(content);

          delete content;

          content = 0;

          Layout();

          Thaw();

          /* Forcibly refresh display to remove display artifacts on Mac OS X and Windows */
          GetParent()->Refresh();
          GetParent()->Update();
          wxTheApp->Yield();
        }

        current_layout = l;

        // Tie handler to the new layout object
        event_handler.send_display_update = boost::bind(&tipi::controller::communicator::send_display_update,
                boost::static_pointer_cast < tipi::controller::communicator > (event_handler.monitor).get(), _1, boost::static_pointer_cast < tipi::display const > (current_layout));

        std::auto_ptr < wxSizer > root(new wxBoxSizer(wxVERTICAL));

        tool_display_mediator m(this, mediator::wrapper_aptr(new tool_display_mediator::wrapper(root.get())), &event_handler);

        try {
          mediator::wrapper_aptr new_layout(current_layout->instantiate(&m));

          if (new_layout.get() != 0) {
            content = static_cast < tool_display_mediator::wrapper* > (new_layout.get())->release_sizer();

            GetSizer()->Insert(1, content, 1, wxALL|wxALIGN_LEFT, 2);

            Layout();

            if (!current_layout->get_visibility()) {
              // Show minimised
              root->SetMinSize(GetClientSize().GetWidth(), root->GetItem((size_t) 0)->GetSize().GetHeight());
            }

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
     * \param[in] w weak pointer to this object
     * \param[in] l the layout elements that have changed
     **/
    void tool_display::update(boost::weak_ptr < tipi::layout::tool_display > w, std::vector < tipi::layout::element const* > l) {
      using namespace detail;

      tool_display_mediator m(this, &event_handler);

      BOOST_FOREACH(tipi::layout::element const* i, l) {
        event_handler.update(&m, i);
      }

      if (w.lock().get()) {
        GetParent()->Layout();
      }
    }

    /**
     * \param[in] w weak pointer to this object
     * \param[in] l the layout elements that have changed
     **/
    void tool_display::update_log(boost::weak_ptr < tipi::layout::tool_display > w, tipi::report::sptr l) {
      boost::shared_ptr < tipi::layout::tool_display > g(w.lock());

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

          /* Show the log */
          log->Show(true);
        }
        else {
          log->AppendText(stamp + wxString(l->get_description().c_str(), wxConvLocal));

          log->ShowPosition(log->GetLastPosition());
        }
      }
    }

    /**
     * \param[in] l the layout specification
     **/
    void tool_display::schedule_log_update(tipi::report::sptr l) {
      context->gui_builder.schedule_update(boost::bind(&tool_display::update_log, this, current_layout, l));
    }

    /**
     * \param[in] l the layout specification
     **/
    void tool_display::schedule_layout_change(tipi::layout::tool_display::sptr l) {
      /* Register handler for updates */
      event_handler.clear();
      event_handler.get_monitor()->set_display_update_handler(l, boost::bind(&GUI::tool_display::schedule_layout_update, this, _1));

      context->gui_builder.schedule_update(boost::bind(&tool_display::instantiate, this, current_layout, l));
    }

    void tool_display::schedule_layout_update(std::vector < tipi::layout::element const* > const& l) {
      context->gui_builder.schedule_update(boost::bind(&tool_display::update, this, current_layout, l));
    }
  }
}


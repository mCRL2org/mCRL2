// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING).
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file gui/tool_display.cpp
/// \brief Add your file description here.

#include <deque>
#include <utility>

#include <tipi/detail/layout_mediator.hpp>
#include <tipi/layout_manager.hpp>

#include <boost/foreach.hpp>

#include "gui/project.hpp"
#include "gui/tool_display.hpp"

#include <wx/button.h>
#include <wx/colour.h>
#include <wx/checkbox.h>
#include <wx/event.h>
#include <wx/gauge.h>
#include <wx/radiobut.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/datetime.h>

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

          /** \brief Handler for events to translate changes in the tipi hierarchy to the wxWidgets hierarchy */
          tipi::layout::basic_event_handler& m_event_handler;
     
          /** \brief The sizer to which the elements will be attached */
          wxWindow*                          current_window;

        public:
     
          /** \brief Constructor */
          tool_display_mediator(tipi::layout::basic_event_handler& e, wxWindow* w);
     
          /** \brief Constructor */
          tool_display_mediator(tipi::layout::basic_event_handler& e, wxWindow* w, wrapper_aptr d);
     
          /** \brief Instantiates a vertically aligned box layout manager */
          mediator::aptr build_vertical_box();
        
          /** \brief Instantiates a horizontally aligned box layout manager */
          mediator::aptr build_horizontal_box();
        
          /** \brief Instantiates a label (static text) */
          mediator::wrapper_aptr build(layout::elements::label const&);

          /** \brief Instantiates a button */
          mediator::wrapper_aptr build(layout::elements::button const&);
        
          /** \brief Instantiates a single radio button */
          mediator::wrapper_aptr build(layout::elements::radio_button const&);
        
          /** \brief Instantiates a single checkbox */
          mediator::wrapper_aptr build(layout::elements::checkbox const&);
     
          /** \brief Instantiates a progress bar */
          mediator::wrapper_aptr build(layout::elements::progress_bar const&);
        
          /** \brief Instantiates a single line text input control */
          mediator::wrapper_aptr build(layout::elements::text_field const&);
      };

      /**
       * \internal
       * Helper class for associating objects of tipi::layout::element with
       * their wxWidgets counterpart
       *
       * This class exists purely because wxWidgets does not provide
       * functionality to register function objects as event handlers
       **/
      template < typename S, typename W >
      class event_helper : public wxEvtHandler, public wxClientData {

        protected:

          S& tipi_element;
          W& wx_element;

        public:

          event_helper(S const& s, W& w) : tipi_element(const_cast < S& > (s)), wx_element(w) {
          }

          // if seperate handling of more than one event per control is interesting more variants of this function can be introduced
          void connect(wxEventType const& e) {
            // For processing updates to the display that originate from local user interaction
            wx_element.Connect(e, wxCommandEventHandler(event_helper::export_wx_changes), this, this);
          }

          void export_wx_changes(wxCommandEvent&);
      };

      /**
       * \param w the window to which to attach elements
       * \param s the event handler that deals with change events
       **/
      tool_display_mediator::tool_display_mediator(tipi::layout::basic_event_handler& e, wxWindow* w) :
                                                        m_event_handler(e), current_window(w) {
      }
     
      /**
       * \param w the window to which to attach elements
       * \param d the sizer to which elements should be attached
       * \param s the event handler that deals with change events
       **/
      tool_display_mediator::tool_display_mediator(tipi::layout::basic_event_handler& e, wxWindow* w, wrapper_aptr d) :
                                                        mediator(d), m_event_handler(e), current_window(w) {
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
       * \return a standard auto pointer to a mediator object with current the mediator with which to attach the children
       **/
      mediator::aptr tool_display_mediator::build_vertical_box() {
        struct trampoline {
          static void attach(wxSizer* sizer, mediator::wrapper_aptr d, tipi::layout::properties const* c) {
            wrapper& target = static_cast < wrapper& > (*d);
            int      flags  = wxLEFT|wxRIGHT;
           
            layout::properties const& cr = *(static_cast < layout::properties const* > (c));
           
            if (cr.m_grow || !target.wraps_window()) {
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
           
            if (0 < cr.m_margin.top) {
              sizer->AddSpacer(cr.m_margin.top);
            }

            wxSizerItem* new_sizer_item;

            if (cr.m_visible != tipi::layout::hidden) {
              if (target.wraps_window()) {
                 new_sizer_item = sizer->Add(target.release_window(), 0, flags, (cr.m_margin.left + cr.m_margin.right) >> 1);
              }
              else {
                 new_sizer_item = sizer->Add(target.release_sizer(), 0, flags, (cr.m_margin.left + cr.m_margin.right) >> 1);
              }
            }
            else {
              // Window is not connected and will be deleted when the auto pointer goes out of scope
              if (target.wraps_window()) {
                wxWindow* s = target.release_window();

                sizer->AddSpacer(s->GetBestSize().GetHeight());

                delete s;
              }
              else {
                wxSizer* s = target.release_sizer();

                sizer->AddSpacer(s->CalcMin().GetHeight());

                delete s;
              }
              /// d.reset() does not work properly with XCode 2.4.1
            }
           
            if (0 < cr.m_margin.bottom) {
              sizer->AddSpacer(cr.m_margin.bottom);
            }
          }
        };

        wxSizer* t = new wxBoxSizer(wxVERTICAL);

        tipi::layout::mediator::aptr m(new tool_display_mediator(m_event_handler, current_window, wrapper_aptr(new wrapper(t))));
     
        m->set_attach(boost::bind(&trampoline::attach, t, _1, _2));
     
        return (m);
      }
     
      /**
       * \return a standard auto pointer to a mediator object with current the mediator with which to attach the children
       **/
      mediator::aptr tool_display_mediator::build_horizontal_box() {
        struct trampoline {
          static void attach(wxSizer* sizer, mediator::wrapper_aptr d, tipi::layout::properties const* c) {
            wrapper& target = static_cast < wrapper& > (*d);
            int      flags  = wxTOP|wxBOTTOM;
           
            tipi::layout::properties const& cr(*(static_cast < layout::properties const* > (c)));
           
            if (cr.m_grow || !target.wraps_window()) {
              flags |= wxEXPAND;
            }
           
            switch (cr.m_alignment_vertical) {
              case layout::top:
                flags |= wxALIGN_TOP;
                break;
              case layout::bottom:
                flags |= wxALIGN_BOTTOM;
                break;
              default: /* center */
                flags |= wxALIGN_CENTER_VERTICAL;
                break;
            }
           
            if (0 < cr.m_margin.left) {
              sizer->AddSpacer(cr.m_margin.left);
            }
           
            wxSizerItem* new_sizer_item;

            if (cr.m_visible != tipi::layout::hidden) {
              if (target.wraps_window()) {
                 new_sizer_item = sizer->Add(target.release_window(), 0, flags, (cr.m_margin.top + cr.m_margin.bottom) >> 1);
              }
              else {
                 new_sizer_item = sizer->Add(target.release_sizer(), 0, flags, (cr.m_margin.top + cr.m_margin.bottom) >> 1);
              }
            }
            else {
              // Window is not connected and will be deleted when the auto pointer goes out of scope
              if (target.wraps_window()) {
                wxWindow* s = target.release_window();

                sizer->AddSpacer(s->GetBestSize().GetWidth());

                delete s;
              }
              else {
                wxSizer* s = target.release_sizer();

                sizer->AddSpacer(s->CalcMin().GetWidth());

                delete s;
              }
              /// d.reset() does not work properly with XCode 2.4.1
            }
           
            if (0 < cr.m_margin.right) {
              sizer->AddSpacer(cr.m_margin.right);
            }
          }
        };

        wxSizer* t = new wxBoxSizer(wxHORIZONTAL);

        tipi::layout::mediator::aptr m(new tool_display_mediator(m_event_handler, current_window, wrapper_aptr (new wrapper(t))));
     
        m->set_attach(boost::bind(&trampoline::attach, t, _1, _2));
     
        return (m);
      }
     
      /**
       * \param[in] e the element that is associated with the new control
       **/
      mediator::wrapper_aptr tool_display_mediator::build(layout::elements::label const& e) {
        struct trampoline {
          static void import(layout::elements::label const& tipi_element, wxStaticText& wx_element) {
            wx_element.SetLabel(wxString(tipi_element.get_text().c_str(), wxConvLocal));
          }
        };

        wxStaticText* t = new wxStaticText(current_window, wxID_ANY, wxString(e.get_text().c_str(), wxConvLocal));

        // For processing updates to the display that originate at the tool side (with scheduler)
        m_event_handler.add(&e, boost::bind(&trampoline::import, boost::cref(e), boost::ref(*t)));

        return (mediator::wrapper_aptr(new wrapper(t)));
      }
     
      template <>
      void event_helper< layout::elements::button, wxButton >::export_wx_changes(wxCommandEvent& e) {
        tipi_element.activate();
      }
     
      /**
       * \param[in] e the element that is associated with the new control
       * \param[in] s the text of the label
       **/
      mediator::wrapper_aptr tool_display_mediator::build(layout::elements::button const& e) {
        struct trampoline {
          static void import(layout::elements::button const& tipi_element, wxButton& wx_element) {
            wx_element.SetLabel(wxString(tipi_element.get_label().c_str(), wxConvLocal));
          }
        };

        wxButton* t;
     
        if (e.get_label() == "OK") {
          t = new wxButton(current_window, wxID_OK);
        }
        else if (e.get_label() == "Cancel") {
          t = new wxButton(current_window, wxID_CANCEL);
        }
        else {
          t = new wxButton(current_window, wxID_ANY, wxString(e.get_label().c_str(), wxConvLocal), wxDefaultPosition, wxButton::GetDefaultSize());
        }

        // For processing updates to the display that originate at the tool side
        m_event_handler.add(&e, boost::bind(&trampoline::import, boost::cref(e), boost::ref(*t)));
     
        event_helper< layout::elements::button, wxButton >* l =
                new event_helper< layout::elements::button, wxButton >(e, *t);

        l->connect(wxEVT_COMMAND_BUTTON_CLICKED);

        return (mediator::wrapper_aptr(new wrapper(t)));
      }

      template <>
      void event_helper< layout::elements::radio_button, wxRadioButton >::export_wx_changes(wxCommandEvent& e) {
        tipi_element.select();
      }

      /**
       * \param[in] e the element that is associated with the new control
       **/
      mediator::wrapper_aptr tool_display_mediator::build(layout::elements::radio_button const& e) {
        using layout::elements::radio_button;

        struct trampoline {
          static void import(layout::elements::radio_button const& tipi_element, wxRadioButton& wx_element) {
            wx_element.SetLabel(wxString(tipi_element.get_label().c_str(), wxConvLocal));
            wx_element.SetValue(tipi_element.is_selected());
          }
        };

        bool first_in_group = true;

        // Establish whether this is the first radio button in the group that is instantiated
        for (radio_button const* i = &e.connected_to(); i != &e; i = &i->connected_to()) {
          if (m_event_handler.has_handler(i)) {
            first_in_group = false;

            break;
          }
        }

        wxRadioButton* t = new wxRadioButton(current_window, wxID_ANY,
                wxString(e.get_label().c_str(), wxConvLocal),wxDefaultPosition, wxDefaultSize, (first_in_group) ? wxRB_GROUP : 0);

        if (e.is_selected()) {
          t->SetValue(true);
        }

        // For processing updates to the display that originate at the tool side
        m_event_handler.add(&e, boost::bind(&trampoline::import, boost::cref(e), boost::ref(*t)));
     
        event_helper< radio_button, wxRadioButton >* l = new event_helper< radio_button, wxRadioButton >(e, *t);

        // For processing updates to the display that originate from local user interaction
        l->connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED);
     
        return (mediator::wrapper_aptr(new wrapper(t)));
      }

      template <>
      void event_helper< layout::elements::checkbox, wxCheckBox >::export_wx_changes(wxCommandEvent& e) {
        tipi_element.set_status(wx_element.GetValue());
      }

      /**
       * \param[in] e the element that is associated with the new control
       **/
      mediator::wrapper_aptr tool_display_mediator::build(layout::elements::checkbox const& e) {
        struct trampoline {
          static void import(layout::elements::checkbox const& tipi_element, wxCheckBox& wx_element) {
            wx_element.SetLabel(wxString(tipi_element.get_label().c_str(), wxConvLocal));
            wx_element.SetValue(tipi_element.get_status());
          }
        };

        wxCheckBox* t = new wxCheckBox(current_window, wxID_ANY, wxString(e.get_label().c_str(), wxConvLocal));
     
        t->SetValue(e.get_status());
     
        // For processing updates to the display that originate at the tool side
        m_event_handler.add(&e, boost::bind(&trampoline::import, boost::cref(e), boost::ref(*t)));
     
        event_helper< layout::elements::checkbox, wxCheckBox >* l =
                new event_helper< layout::elements::checkbox, wxCheckBox >(e, *t);

        l->connect(wxEVT_COMMAND_CHECKBOX_CLICKED);
     
        return (mediator::wrapper_aptr(new wrapper(t)));
      }
     
      /**
       * \param[in] e the element that is associated with the new control
       **/
      mediator::wrapper_aptr tool_display_mediator::build(layout::elements::progress_bar const& e) {
        struct trampoline {
          static void import(layout::elements::progress_bar const& tipi_element, wxGauge& wx_element) {
            wx_element.SetRange(tipi_element.get_maximum() - tipi_element.get_minimum());
            wx_element.SetValue(tipi_element.get_value() - tipi_element.get_minimum());
          }
        };

        wxGauge* t = new wxGauge(current_window, wxID_ANY, e.get_maximum() - e.get_minimum(), wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL);
     
        // SetRange() avoids a bug with range setting in the constructor (wx-2.8)
        t->SetRange(e.get_maximum() - e.get_minimum());
        t->SetValue(e.get_value() - e.get_minimum());
     
        // For processing updates to the display that originate at the tool side
        m_event_handler.add(&e, boost::bind(&trampoline::import, boost::cref(e), boost::ref(*t)));
     
        return (mediator::wrapper_aptr(new wrapper(t)));
      }
     
      template <>
      void event_helper< layout::elements::text_field, wxTextCtrl >::export_wx_changes(wxCommandEvent& e) {
        tipi_element.set_text(std::string(wx_element.GetValue().fn_str()));
      }

      /**
       * \param[in] e the element that is associated with the new control
       **/
      mediator::wrapper_aptr tool_display_mediator::build(layout::elements::text_field const& e) {
        struct trampoline {
          static void import(layout::elements::text_field const& tipi_element, wxTextCtrl& wx_element) {
            wx_element.SetLabel(wxString(tipi_element.get_text().c_str(), wxConvLocal));
          }
        };

        wxTextCtrl* t = new wxTextCtrl(current_window, wxID_ANY, wxString(e.get_text().c_str(), wxConvLocal), wxDefaultPosition, wxSize(200,-1));
     
        // For processing updates to the display that originate at the tool side
        m_event_handler.add(&e, boost::bind(&trampoline::import, boost::cref(e), boost::ref(*t)));
     
        event_helper< layout::elements::text_field, wxTextCtrl >* l =
                new event_helper< layout::elements::text_field, wxTextCtrl >(e, *t);

        l->connect(wxEVT_COMMAND_TEXT_UPDATED);
     
        return (mediator::wrapper_aptr(new wrapper(t)));
      }
    }

    /**
     * \param[in] p the parent window
     * \param[in] c the project that owns tool display
     * \param[in] s the processor associated with this display
     **/
    tool_display::tool_display(wxWindow* p, GUI::project* c, boost::shared_ptr < processor::monitor >& s) :
                                wxPanel(p, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER),
                                m_project(c), m_layout(new tipi::layout::tool_display), m_content(0), m_monitor(s), m_log(0) {

      build();

      /* Connect event handlers */
      m_monitor->set_display_layout_handling(boost::bind(&GUI::tool_display::schedule_layout_change, this, _1),
                                             boost::bind(&GUI::tool_display::schedule_layout_update, this, _1));
      m_monitor->set_status_message_handler(boost::bind(&GUI::tool_display::schedule_log_update, this, _1));
    }

    void tool_display::build() {
      wxSizer* root(new wxBoxSizer(wxVERTICAL));

      SetBackgroundColour(*wxWHITE);

      /* Set minimum dimensions */
      root->SetMinSize(GetClientSize().GetWidth(), 110);

      wxWindow* titlepanel(new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER));

      m_control_bar = new wxBoxSizer(wxHORIZONTAL);
      m_control_bar->AddSpacer(4);
      m_control_bar->Add(new wxStaticText(titlepanel, wxID_ANY, wxT("")), 0, wxTOP|wxBOTTOM|wxLEFT, 2);
      m_control_bar->AddStretchSpacer(1);
      m_control_bar->Add(new wxButton(titlepanel, cmID_MINIMISE, wxT("-"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 0, wxALIGN_RIGHT);
      m_control_bar->Add(new wxButton(titlepanel, cmID_CLOSE, wxT("x"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), 0, wxALIGN_RIGHT);

      titlepanel->SetBackgroundColour(*wxLIGHT_GREY);
      titlepanel->SetSizer(m_control_bar);
      root->Add(titlepanel, 0, wxEXPAND);

      // Set state to minimised
      root->SetMinSize(GetClientSize().GetWidth(), root->GetItem((size_t) 0)->GetSize().GetHeight());

      SetSizer(root);

      Connect(cmID_MINIMISE, cmID_CLOSE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(tool_display::on_panel_button_clicked));
    }

    void tool_display::set_title(wxString s) {
      m_control_bar->GetItem(1)->GetWindow()->SetLabel(s);
    }

    void tool_display::on_panel_button_clicked(wxCommandEvent& e) {
      if (e.GetId() == cmID_MINIMISE) {
        if (m_content != 0) {
          wxSizer* root = GetSizer();

          if (root->IsShown(m_content)) {
            root->SetMinSize(GetClientSize().GetWidth(), root->GetItem((size_t) 0)->GetSize().GetHeight());

            m_content->Show(false);

            if (m_log != 0) {
              m_log->Show(false);
            }
          }
          else {
            root->SetMinSize(GetClientSize().GetWidth(), 110);

            m_content->Show(true);

            if (m_log != 0) {
              m_log->Show(true);
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
          wxSizer* s = display->GetParent()->GetSizer();

          s->Show(display, false, true);
          s->Detach(display);
          s->Layout();
          display->GetParent()->Refresh();

          display->toggle_scrollbar_helper();

          display->Destroy();
        }
      };

      m_layout.reset();
      m_event_handler.clear();

      if (u) {
        m_project->gui_builder.schedule_update(boost::bind(&local::trampoline, this));
      }

      /* End tool execution, if it was still running */
      m_monitor->finish();
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
        if (m_content != 0) {
          /* Forcibly refresh display to prevent a crash on Mac OS X */
          Refresh();
          Update();
          wxTheApp->Yield();

          Freeze();

          /* Delete controls */
          m_content->Clear(true);

          GetSizer()->Detach(m_content);

          delete m_content;

          m_content = 0;

          Layout();

          Thaw();

          /* Forcibly refresh display to remove display artifacts on Mac OS X and Windows */
          GetParent()->Refresh();
          GetParent()->Update();
          wxTheApp->Yield();
        }

        m_event_handler.clear();
        m_layout = l;

        std::auto_ptr < wxSizer > root(new wxBoxSizer(wxVERTICAL));

        tool_display_mediator m(m_event_handler, this, mediator::wrapper_aptr(new tool_display_mediator::wrapper(root.get())));

        try {
          mediator::wrapper_aptr new_layout(m_layout->instantiate(&m));

          if (new_layout.get() != 0) {
            m_content = static_cast < tool_display_mediator::wrapper* > (new_layout.get())->release_sizer();

            GetSizer()->Insert(1, m_content, 1, wxALL|wxALIGN_LEFT, 2);

            Layout();

            if (!m_layout->get_visibility()) {
              // Show minimised
              root->SetMinSize(GetClientSize().GetWidth(), root->GetItem((size_t) 0)->GetSize().GetHeight());
            }

            Show(true);
          }
          else {
            m_content = 0;
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

      boost::shared_ptr < tipi::layout::tool_display > p(w.lock()); 

      if (p) {
        tool_display_mediator m(m_event_handler, this);
       
        BOOST_FOREACH(tipi::layout::element const* i, l) {
          m_event_handler.execute_handlers(i, false);
        }
       
        GetParent()->Layout();

        /* Toggle scrollbar availability on demand */
        wxSizeEvent size_event(GetParent()->GetSize(), GetParent()->GetId());

        size_event.SetEventObject(GetParent());

        GetParent()->GetParent()->ProcessEvent(size_event);
      }
    }

    /**
     * \param[in] w weak pointer to this object
     * \param[in] l the layout elements that have changed
     **/
    void tool_display::update_log(boost::shared_ptr< tipi::report > l) {
      wxString stamp = wxDateTime::Now().Format(wxT("%b %e %H:%M:%S "));

      if (m_log == 0) {
        wxSizer* sizer = GetSizer();

        m_log = new wxTextCtrl(this, wxID_ANY, stamp + wxString(l->get_description().c_str(), wxConvLocal), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY);
        m_log->SetSize(-1, 40);

        sizer->Add(m_log, 0, wxALL|wxEXPAND|wxALIGN_CENTER, 2);

        GetParent()->Layout();

        /* Toggle scrollbar availability on demand */
        wxSizeEvent size_event(GetParent()->GetSize(), GetParent()->GetId());

        size_event.SetEventObject(GetParent());

        GetParent()->GetParent()->ProcessEvent(size_event);

        /* Show the log */
        m_log->Show(true);
      }
      else {
        m_log->AppendText(stamp + wxString(l->get_description().c_str(), wxConvLocal));

        m_log->ShowPosition(m_log->GetLastPosition());
      }
    }

    /**
     * \param[in] l the layout specification
     **/
    void tool_display::schedule_log_update(boost::shared_ptr< tipi::report > l) {
      m_project->gui_builder.schedule_update(boost::bind(&tool_display::update_log, this, l));
    }

    /**
     * \param[in] l the layout specification
     **/
    void tool_display::schedule_layout_change(boost::shared_ptr < tipi::layout::tool_display > l) {
      m_project->gui_builder.schedule_update(boost::bind(&tool_display::instantiate, this, boost::weak_ptr< tipi::layout::tool_display >(m_layout), l));
    }

    void tool_display::schedule_layout_update(std::vector < tipi::layout::element const* > const& l) {
      m_project->gui_builder.schedule_update(boost::bind(&tool_display::update, this, boost::weak_ptr< tipi::layout::tool_display >(m_layout), l));
    }
  }
}


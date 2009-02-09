// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file gui/tool_display.cpp
/// \brief Add your file description here.

#include "wx.hpp" // precompiled headers

#include <deque>
#include <utility>

#include <boost/foreach.hpp>

#include "tipi/layout_manager.hpp"
#include "tipi/layout_elements.hpp"
#include "tipi/tool_display.hpp"

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

class tipi_export_visitor_impl : public tipi::layout::basic_event_handler {

  protected:

    wxWindow* m_current_window;

  public:

    tipi_export_visitor_impl() {
    }
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

    void connect(wxEventType const& e) {
      wx_element.Connect(e, wxCommandEventHandler(event_helper::export_wx_changes), this, this);
    }

    void export_wx_changes(wxCommandEvent&);
};

template <>
void event_helper< tipi::layout::elements::button, wxButton >::export_wx_changes(wxCommandEvent& e) {
  tipi_element.activate();
}

template <>
void event_helper< tipi::layout::elements::radio_button, wxRadioButton >::export_wx_changes(wxCommandEvent& e) {
  tipi_element.select();
}

template <>
void event_helper< tipi::layout::elements::checkbox, wxCheckBox >::export_wx_changes(wxCommandEvent& e) {
  tipi_element.set_status(wx_element.GetValue());
}

template <>
void event_helper< tipi::layout::elements::text_field, wxTextCtrl >::export_wx_changes(wxCommandEvent& e) {
  tipi_element.set_text(std::string(wx_element.GetValue().fn_str()));
}

namespace utility {

  // code could be more compact and clear but needs to be this way to circumvent buttom-up construction bug in wxWidgets
  template <>
  template <>
  std::auto_ptr< wxObject > visitor< tipi_export_visitor_impl, std::auto_ptr< wxObject > >::visit(tipi::tool_display const& t) {
    if (t.manager() != 0) {
      return do_visit(*t.manager());
    }

    return std::auto_ptr< wxObject >();
  }

  template <>
  template <>
  std::auto_ptr< wxObject > visitor< tipi_export_visitor_impl, std::auto_ptr< wxObject > >::visit(tipi::layout::vertical_box const& b) {
    std::auto_ptr< wxSizer > sizer(new wxBoxSizer(wxVERTICAL));

    for (tipi::layout::box< tipi::layout::vertical_alignment >::children_list::const_iterator i = b.m_children.begin(); i != b.m_children.end(); ++i) {
      tipi::layout::properties const& properties = i->layout_properties;

      if (0 < properties.m_margin.m_top) {
        sizer->AddSpacer(properties.m_margin.m_top);
      }

      std::auto_ptr< wxObject > new_child(do_visit(*i->layout_element));

      if (properties.m_visible == tipi::layout::hidden) {
        if (dynamic_cast< tipi::layout::manager const* > (i->layout_element) != 0) {
          sizer->AddSpacer(static_cast< wxSizer* > (new_child.get())->CalcMin().GetHeight());
        }
        else {
          sizer->AddSpacer(static_cast< wxWindow* > (new_child.get())->GetBestSize().GetHeight());
        }
      }
      else {
        int flags = wxLEFT|wxRIGHT;

        switch (properties.m_alignment_horizontal) {
          case tipi::layout::right:
            flags |= wxALIGN_RIGHT;
            break;
          case tipi::layout::center:
            flags |= wxALIGN_CENTER_HORIZONTAL;
            break;
          default:
            flags |= wxALIGN_LEFT;
            break;
        }

        if (dynamic_cast< tipi::layout::manager const* > (i->layout_element) != 0) {
          flags |= wxEXPAND;

          sizer->Add(static_cast< wxSizer* > (new_child.release()), 0, flags, (properties.m_margin.m_left + properties.m_margin.m_right) >> 1);
        }
        else {
          flags |= (properties.m_grow) ? wxEXPAND : 0;

          sizer->Add(static_cast< wxWindow* > (new_child.release()), 0, flags, (properties.m_margin.m_left + properties.m_margin.m_right) >> 1);
        }
      }

      if (0 < properties.m_margin.m_bottom) {
        sizer->AddSpacer(properties.m_margin.m_bottom);
      }
    }

    return std::auto_ptr< wxObject > (sizer);
  }

  /**
   * \return a standard auto pointer to a mediator object with current the mediator with which to attach the children
   **/
  template <>
  template <>
  std::auto_ptr< wxObject > visitor< tipi_export_visitor_impl, std::auto_ptr< wxObject > >::visit(tipi::layout::horizontal_box const& b) {
    std::auto_ptr< wxSizer > sizer(new wxBoxSizer(wxHORIZONTAL));

    for (tipi::layout::box< tipi::layout::horizontal_alignment >::children_list::const_iterator i = b.m_children.begin(); i != b.m_children.end(); ++i) {
      tipi::layout::properties const& properties = i->layout_properties;

      if (0 < properties.m_margin.m_left) {
        sizer->AddSpacer(properties.m_margin.m_left);
      }

      std::auto_ptr< wxObject > new_child(do_visit(*i->layout_element));

      if (properties.m_visible == tipi::layout::hidden) {
        if (dynamic_cast< tipi::layout::manager const* > (i->layout_element) != 0) {
          sizer->AddSpacer(static_cast< wxSizer* > (new_child.get())->CalcMin().GetWidth());
        }
        else {
          sizer->AddSpacer(static_cast< wxWindow* > (new_child.get())->GetBestSize().GetWidth());
        }
      }
      else {
        int flags = wxTOP|wxBOTTOM;

        switch (properties.m_alignment_vertical) {
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

        if (dynamic_cast< tipi::layout::manager const* > (i->layout_element) != 0) {
          flags |= wxEXPAND;

          sizer->Add(static_cast< wxSizer* > (new_child.release()), 0, flags, (properties.m_margin.m_top + properties.m_margin.m_bottom) >> 1);
        }
        else {
          flags |= (properties.m_grow) ? wxEXPAND : 0;

          sizer->Add(static_cast< wxWindow* > (new_child.release()), 0, flags, (properties.m_margin.m_top + properties.m_margin.m_bottom) >> 1);
        }
      }

      if (0 < properties.m_margin.m_right) {
        sizer->AddSpacer(properties.m_margin.m_right);
      }
    }

    return std::auto_ptr< wxObject > (sizer);
  }

  /**
   * \param[in] e the element that is associated with the new control
   **/
  template <>
  template <>
  std::auto_ptr< wxObject > visitor< tipi_export_visitor_impl, std::auto_ptr< wxObject > >::visit(tipi::layout::elements::label const& e) {
    struct trampoline {
      static void import(tipi::layout::elements::label const& tipi_element, wxStaticText& wx_element) {
        wx_element.SetLabel(wxString(tipi_element.get_text().c_str(), wxConvLocal));
      }
    };

    std::auto_ptr< wxStaticText > label(new wxStaticText(m_current_window, wxID_ANY, wxString(e.get_text().c_str(), wxConvLocal)));

    // For processing updates to the display that originate at the tool side (with scheduler)
    tipi::layout::basic_event_handler::add(&e, boost::bind(&trampoline::import, boost::cref(e), boost::ref(*label)));

    return std::auto_ptr< wxObject > (label);
  }

  /**
   * \param[in] e the element that is associated with the new control
   * \param[in] s the text of the label
   **/
  template <>
  template <>
  std::auto_ptr< wxObject > visitor< tipi_export_visitor_impl, std::auto_ptr< wxObject > >::visit(tipi::layout::elements::button const& e) {
    struct trampoline {
      static void import(tipi::layout::elements::button const& tipi_element, wxButton& wx_element) {
        wx_element.SetLabel(wxString(tipi_element.get_label().c_str(), wxConvLocal));
      }
    };

    std::auto_ptr< wxButton > button;

    if (e.get_label() == "OK") {
      button.reset(new wxButton(m_current_window, wxID_OK));
    }
    else if (e.get_label() == "Cancel") {
      button.reset(new wxButton(m_current_window, wxID_CANCEL));
    }
    else {
      button.reset(new wxButton(m_current_window, wxID_ANY, wxString(e.get_label().c_str(), wxConvLocal), wxDefaultPosition, wxButton::GetDefaultSize()));
    }

    // For processing updates to the display that originate at the tool side
    tipi::layout::basic_event_handler::add(&e, boost::bind(&trampoline::import, boost::cref(e), boost::ref(*button)));

    event_helper< tipi::layout::elements::button, wxButton >* l(
            new event_helper< tipi::layout::elements::button, wxButton >(e, *button));

    l->connect(wxEVT_COMMAND_BUTTON_CLICKED);

    return std::auto_ptr< wxObject > (button);
  }

  /**
   * \param[in] e the element that is associated with the new control
   **/
  template <>
  template <>
  std::auto_ptr< wxObject > visitor< tipi_export_visitor_impl, std::auto_ptr< wxObject > >::visit(tipi::layout::elements::radio_button const& e) {
    using tipi::layout::elements::radio_button;

    struct trampoline {
      static void import(tipi::layout::elements::radio_button const& tipi_element, wxRadioButton& wx_element) {
        wx_element.SetLabel(wxString(tipi_element.get_label().c_str(), wxConvLocal));
        wx_element.SetValue(tipi_element.is_selected());
      }
    };

    bool first_in_group = true;

    // Establish whether this is the first radio button in the group that is instantiated
    for (radio_button const* i = &e.connected_to(); i != &e; i = &i->connected_to()) {
      if (tipi::layout::basic_event_handler::has_handler(i)) {
        first_in_group = false;

        break;
      }
    }

    std::auto_ptr< wxRadioButton > button(new wxRadioButton(m_current_window, wxID_ANY,
            wxString(e.get_label().c_str(), wxConvLocal),wxDefaultPosition, wxDefaultSize, (first_in_group) ? wxRB_GROUP : 0));

    if (e.is_selected()) {
      button->SetValue(true);
    }

    // For processing updates to the display that originate at the tool side
    tipi::layout::basic_event_handler::add(&e, boost::bind(&trampoline::import, boost::cref(e), boost::ref(*button)));

    event_helper< radio_button, wxRadioButton >* l = new event_helper< radio_button, wxRadioButton >(e, *button);

    // For processing updates to the display that originate from local user interaction
    l->connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED);

    return std::auto_ptr< wxObject > (button);
  }

  /**
   * \param[in] e the element that is associated with the new control
   **/
  template <>
  template <>
  std::auto_ptr< wxObject > visitor< tipi_export_visitor_impl, std::auto_ptr< wxObject > >::visit(tipi::layout::elements::checkbox const& e) {
    struct trampoline {
      static void import(tipi::layout::elements::checkbox const& tipi_element, wxCheckBox& wx_element) {
        wx_element.SetLabel(wxString(tipi_element.get_label().c_str(), wxConvLocal));
        wx_element.SetValue(tipi_element.get_status());
      }
    };

    std::auto_ptr< wxCheckBox > checkbox(new wxCheckBox(m_current_window, wxID_ANY, wxString(e.get_label().c_str(), wxConvLocal)));

    checkbox->SetValue(e.get_status());

    // For processing updates to the display that originate at the tool side
    tipi::layout::basic_event_handler::add(&e, boost::bind(&trampoline::import, boost::cref(e), boost::ref(*checkbox)));

    event_helper< tipi::layout::elements::checkbox, wxCheckBox >* l =
            new event_helper< tipi::layout::elements::checkbox, wxCheckBox >(e, *checkbox);

    l->connect(wxEVT_COMMAND_CHECKBOX_CLICKED);

    return std::auto_ptr< wxObject > (checkbox);
  }

  /**
   * \param[in] e the element that is associated with the new control
   **/
  template <>
  template <>
  std::auto_ptr< wxObject > visitor< tipi_export_visitor_impl, std::auto_ptr< wxObject > >::visit(tipi::layout::elements::progress_bar const& e) {
    struct trampoline {
      static void import(tipi::layout::elements::progress_bar const& tipi_element, wxGauge& wx_element) {
        wx_element.SetRange(tipi_element.get_maximum() - tipi_element.get_minimum());
        wx_element.SetValue(tipi_element.get_value() - tipi_element.get_minimum());
      }
    };

    std::auto_ptr< wxGauge > progress_bar(new wxGauge(m_current_window, wxID_ANY, e.get_maximum() - e.get_minimum(), wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL));

    // SetRange() avoids a bug with range setting in the constructor (wx-2.8)
    progress_bar->SetRange(e.get_maximum() - e.get_minimum());
    progress_bar->SetValue(e.get_value() - e.get_minimum());

    // For processing updates to the display that originate at the tool side
    tipi::layout::basic_event_handler::add(&e, boost::bind(&trampoline::import, boost::cref(e), boost::ref(*progress_bar)));

    return std::auto_ptr< wxObject > (progress_bar);
  }

  /**
   * \param[in] e the element that is associated with the new control
   **/
  template <>
  template <>
  std::auto_ptr< wxObject > visitor< tipi_export_visitor_impl, std::auto_ptr< wxObject > >::visit(tipi::layout::elements::text_field const& e) {
    struct trampoline {
      static void import(tipi::layout::elements::text_field const& tipi_element, wxTextCtrl& wx_element) {
        wx_element.SetLabel(wxString(tipi_element.get_text().c_str(), wxConvLocal));
      }
    };

    std::auto_ptr< wxTextCtrl > text_field(new wxTextCtrl(m_current_window, wxID_ANY, wxString(e.get_text().c_str(), wxConvLocal), wxDefaultPosition, wxSize(200,-1)));

    // For processing updates to the display that originate at the tool side
    tipi::layout::basic_event_handler::add(&e, boost::bind(&trampoline::import, boost::cref(e), boost::ref(*text_field)));

    event_helper< tipi::layout::elements::text_field, wxTextCtrl >* l =
            new event_helper< tipi::layout::elements::text_field, wxTextCtrl >(e, *text_field);

    l->connect(wxEVT_COMMAND_TEXT_UPDATED);

    return std::auto_ptr< wxObject > (text_field);
  }

  template <>
  bool visitor< tipi_export_visitor_impl, std::auto_ptr< wxObject > >::initialise() {
    register_visit_method< const tipi::tool_display >();
    register_visit_method< const tipi::layout::vertical_box >();
    register_visit_method< const tipi::layout::horizontal_box >();
    register_visit_method< const tipi::layout::elements::label >();
    register_visit_method< const tipi::layout::elements::button >();
    register_visit_method< const tipi::layout::elements::radio_button >();
    register_visit_method< const tipi::layout::elements::progress_bar >();
    register_visit_method< const tipi::layout::elements::checkbox >();
    register_visit_method< const tipi::layout::elements::text_field >();

    return true;
  }
}

namespace squadt {
  namespace GUI {
    #define cmID_MINIMISE  (wxID_HIGHEST + 1)
    #define cmID_CLOSE     (wxID_HIGHEST + 2)

    /** \brief Translates a tipi layout to a functional wxWidgets layout */
    class tool_display_mediator : public ::utility::visitor< tipi_export_visitor_impl, std::auto_ptr< wxObject > > {

      public:

        /** \brief Constructor */
        tool_display_mediator(tool_display& w) {
          m_current_window = &w;
        }

        /** \param[in] w The sizer to which the elements will be attached */
        std::auto_ptr< wxSizer > instantiate_layout(tipi::display const& t) {
          return std::auto_ptr< wxSizer >(static_cast< wxSizer* > (do_visit(t).release()));
        }
    };

    /**
     * \param[in] p the parent window
     * \param[in] c the project that owns tool display
     * \param[in] s the processor associated with this display
     **/
    tool_display::tool_display(wxWindow* p, GUI::project* c, boost::shared_ptr < processor::monitor >& s) :
                                wxPanel(p, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER),
                                m_project(c), m_layout(new tipi::tool_display), m_content(0),
                                m_monitor(s), m_log(0), m_mediator(new tool_display_mediator(*this)) {

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

      toggle_scrollbar_helper();
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

    /**
     * \param[in] u whether or not to schedule a GUI update
     **/
    void tool_display::remove(bool u) {
      struct local {
        static void trampoline(tool_display* display) {
          wxSizer* s = display->GetParent()->GetSizer();

          s->Show(display, false, true);
          s->Detach(display);

          display->toggle_scrollbar_helper();

          display->Destroy();
        }
      };

      m_layout.reset();
      m_mediator->clear();

      if (u) {
        m_project->schedule_update(boost::bind(&local::trampoline, this));
      }

      /* End tool execution, if it was still running */
      m_monitor->finish();
    }

    /* Toggle scrollbar availability on demand */
    void tool_display::toggle_scrollbar_helper() {
      wxSizeEvent size_event(GetParent()->GetSize(), GetParent()->GetId());

      size_event.SetEventObject(GetParent());

      GetParent()->GetParent()->ProcessEvent(size_event);
      GetParent()->Layout();
      GetParent()->Refresh();
    }

    /**
     * \param[in] w weak pointer to the interface object
     * \param[in] l the new tool_display object
     **/
    void tool_display::instantiate(boost::weak_ptr < tipi::tool_display > w, boost::shared_ptr< tipi::tool_display > l) {
      boost::shared_ptr < tipi::tool_display > g(w.lock());

      if (g.get() != 0) {
        if (m_content != 0) {
          /* Forcibly refresh display to prevent a crash on Mac OS X */
          Refresh();
          Update();

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
        }

        // clear event handlers
        m_mediator->clear();
        m_layout = l;

        try {
          m_content = boost::static_pointer_cast< tool_display_mediator >(m_mediator)->instantiate_layout(*m_layout).release();

          if (m_content) {
            GetSizer()->Insert(1, m_content, 1, wxALL|wxALIGN_LEFT, 2);

            Layout();

            if (!m_layout->visible()) {
              // Show minimised
              m_content->SetMinSize(GetClientSize().GetWidth(), m_content->GetItem((size_t) 0)->GetSize().GetHeight());
            }

            Show(true);
          }

          /* Toggle scrollbar availability on demand */
          toggle_scrollbar_helper();
        }
        catch (...) {
          /* Consider every exception a failure to correctly read the layout, and bail */
          std::cerr << "fatal: layout translation from specification failed!" << std::endl;

          remove();
        }
      }
    }

    /**
     * \param[in] w weak pointer to this object
     * \param[in] l the layout elements that have changed
     **/
    void tool_display::update(boost::weak_ptr< tipi::tool_display > w, std::vector < tipi::layout::element const* > l) {
      boost::shared_ptr < tipi::tool_display > p(w.lock());

      if (p) {
        BOOST_FOREACH(tipi::layout::element const* i, l) {
          m_mediator->execute_handlers(i, false);
        }

        /* Toggle scrollbar availability on demand */
        toggle_scrollbar_helper();
      }
    }

    /**
     * \param[in] w weak pointer to this object
     * \param[in] l the layout elements that have changed
     **/
    void tool_display::update_log(boost::shared_ptr< tipi::report > l) {
      wxString stamp = wxDateTime::Now().Format(wxT("%b %d %H:%M:%S "));

      if (m_log == 0) {
        wxSizer* sizer = GetSizer();

        m_log = new wxTextCtrl(this, wxID_ANY, stamp + wxString(l->description().c_str(), wxConvLocal), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY);
        m_log->SetSize(-1, 40);

        sizer->Add(m_log, 0, wxALL|wxEXPAND|wxALIGN_CENTER, 2);

        /* Toggle scrollbar availability on demand */
        toggle_scrollbar_helper();

        /* Show the log */
        m_log->Show(true);
      }
      else {
        m_log->AppendText(stamp + wxString(l->description().c_str(), wxConvLocal));

        m_log->ShowPosition(m_log->GetLastPosition());
      }
    }

    /**
     * \param[in] l the layout specification
     **/
    void tool_display::schedule_log_update(boost::shared_ptr< tipi::report > l) {
      m_project->schedule_update(boost::bind(&tool_display::update_log, this, l));
    }

    /**
     * \param[in] l the layout specification
     **/
    void tool_display::schedule_layout_change(boost::shared_ptr < tipi::tool_display > l) {
      boost::shared_ptr< tipi::tool_display > guard(m_layout);

      if (guard) {
        boost::weak_ptr< tipi::tool_display > target(guard);

        m_project->schedule_update(boost::bind(&tool_display::instantiate, this, target, l));
      }
    }

    void tool_display::schedule_layout_update(std::vector < tipi::layout::element const* > const& l) {
      boost::shared_ptr< tipi::tool_display > guard(m_layout);

      if (guard) {
        boost::weak_ptr< tipi::tool_display > target(guard);

        m_project->schedule_update(boost::bind(&tool_display::update, this, target, l));
      }
    }
  }
}


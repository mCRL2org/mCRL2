// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file gui/tool_display.h
/// \brief Add your file description here.

#ifndef GUI_TOOL_DISPLAY_H
#define GUI_TOOL_DISPLAY_H

#include <map>

#include <tipi/report.hpp>
#include <tipi/tool_display.hpp>

#include "../processor.hpp"

#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/event.h>
#include <wx/textctrl.h>

namespace squadt {
  namespace GUI {

    class project;

    /**
     * @brief Display window associated with a tool
     **/
    class tool_display : public wxPanel {
      friend class GUI::project;
      friend class GUI::project::builder;

      private:

        /** \brief The GUI project view to which this display `belongs' */
        GUI::project*                                     m_project;

        /** \brief Abstract description of the current layout of this panel */
        boost::shared_ptr < tipi::layout::tool_display >  m_layout;

        /** \brief Sizer that contains the content part */
        wxSizer*                                          m_content;

        /** \brief Sizer of the tool display control buttons */
        wxSizer*                                          m_control_bar;

        /** \brief Connected monitor */
        boost::shared_ptr < processor::monitor >          m_monitor;

        /** \brief Sizer of the tool display control buttons */
        wxTextCtrl*                                       m_log;

        /** \brief Sizer of the tool display control buttons */
        tipi::layout::basic_event_handler                 m_event_handler;

      private:

        /** \brief Initialises widgets */
        void build();

        /** \brief Builds the specified layout within this window */
        void instantiate(boost::weak_ptr < tipi::layout::tool_display >, tipi::layout::tool_display::sptr l);

        /** \brief Update the (G)UI state for a list of elements */
        void update(boost::weak_ptr < tipi::layout::tool_display >, std::vector < tipi::layout::element const* >);

        /** \brief Update the log with incoming status messages */
        void update_log(boost::shared_ptr< tipi::report > l);

        /** \brief Set a new layout description */
        void schedule_layout_change(boost::shared_ptr< tipi::layout::tool_display >);

        /** \brief Set a new layout description */
        void schedule_layout_update(std::vector < tipi::layout::element const* > const&);

        /** \brief Set a new layout description */
        void schedule_log_update(boost::shared_ptr< tipi::report >);

        /** \brief Handler for panel control button events */
        void on_panel_button_clicked(wxCommandEvent&);

        /** \brief Helper function to ensure a scrollbar is (in)visible when necessary */
        void toggle_scrollbar_helper();

      public:

        /** \brief Constructor */
        tool_display(wxWindow*, project*, boost::shared_ptr < processor::monitor >& s);

        /** \brief Sets the title of the display window */
        void set_title(wxString);

        /** \brief Removes itself from the parent window */
        void remove(bool = true);

        /** \brief Destructor */
        ~tool_display();
    };
  }
}

#endif

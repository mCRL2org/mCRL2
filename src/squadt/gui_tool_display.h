#ifndef GUI_TOOL_DISPLAY_H
#define GUI_TOOL_DISPLAY_H

#include <map>

#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/event.h>

#include <sip/detail/layout_tool_display.h>

#include "processor.h"

namespace squadt {
  namespace GUI {

    class project;

    namespace detail {

      class tool_display;
      class tool_display_mediator;

      /** \brief Handles state events of widgets */
      class state_change_handler : public wxEvtHandler {
        friend class tool_display;
        friend class detail::tool_display_mediator;

        private:
     
          /** \brief Associates a sip layout element with a wxWidgets control */
          typedef std::map < wxObject*, sip::layout::element const* > element_for_window_map;
     
        private:
     
          /** \brief Associated processor */
          processor::monitor::sptr  monitor;

          /** \brief Associates a sip layout element with a wxWidgets control */
          element_for_window_map     element_for_window;
     
        private:
     
          /** \brief Handler for when a button was clicked */
          void button_clicked(wxCommandEvent&);
     
          /** \brief Handler for when radio buttons changed state */
          void radio_button_selected(wxCommandEvent&);
     
          /** \brief Handler for when radio buttons changed state */
          void checkbox_clicked(wxCommandEvent&);
     
          /** \brief Handler for when radio buttons changed state */
          void text_field_changed(wxCommandEvent&);

        public:

          /** \brief Constructor */
          state_change_handler(processor::monitor::sptr&);

          /** \brief Clears the element_for_window map */
          inline void clear();

          /** \brief Associate a sip layout element pointer with a wxWindow pointer */
          inline void associate(wxObject*, sip::layout::element const*);

          /** \brief Update the (G)UI state for a specific element */
          void update(sip::layout::mediator* m, sip::layout::element const*);

          /** \brief Gets the monitor for the associated process */
          processor::monitor::sptr& get_monitor();
      };
    }

    /**
     * @brief Display window associated with a tool
     **/
    class tool_display : public wxPanel {
      friend class GUI::project;
      friend class GUI::project::builder;

      private:

        /** \brief The GUI project view to which this display `belongs' */
        GUI::project*                   context;

        /** \brief The event handler */
        detail::state_change_handler    event_handler;

        /** \brief Abstract description of the layout of this panel */
        sip::layout::tool_display::sptr current_layout;

        /** \brief Sizer that contains the content part */
        wxSizer*                        content;

      private:

        /** \brief Builds the specified layout within this window */
        void instantiate(sip::layout::tool_display::sptr l);

        /** \brief Intialises widgets */
        void build();

        /** \brief Update the (G)UI state for a list of elements */
        void update(std::vector < sip::layout::element const* >);

        /** \brief Set a new layout description */
        void schedule_layout_change(sip::layout::tool_display::sptr);

        /** \brief Set a new layout description */
        void schedule_layout_update(std::vector < sip::layout::element const* > const&);

      public:

        /** \brief Constructor */
        tool_display(wxWindow*, project*, processor::monitor::sptr& s);

        /** \brief Removes itself from the parent window */
        void remove();
    };

    namespace detail {

      /**
       * @param[in] s the processor associated with this display
       **/
      inline state_change_handler::state_change_handler(processor::monitor::sptr& s) : monitor(s) {
      }

      inline void state_change_handler::state_change_handler::clear() {
        element_for_window.clear();
      }

      inline void state_change_handler::state_change_handler::associate(wxObject* o, sip::layout::element const* e) {
        element_for_window[o] = e;
      }

      inline processor::monitor::sptr& state_change_handler::get_monitor() {
        return (monitor);
      }
    }
  }
}

#endif

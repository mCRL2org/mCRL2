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
        friend class detail::tool_display_mediator;

        private:
     
          /** \brief Associated processor */
          processor::reporter::sptr                     owner;

          /** \brief Associates a sip layout element with a wxWidgets control */
          std::map < wxObject*, sip::layout::element* > element_for_window;
     
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
          state_change_handler(processor::reporter::sptr&);

          /** \brief Clears the element_for_window map */
          inline void clear();

          /** \brief Associate a sip layout element pointer with a wxWindow pointer */
          inline void associate(wxObject*, sip::layout::element*);
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

      public:

        /** \brief Constructor */
        inline tool_display(wxWindow*, project*, processor::reporter::sptr& s);

        /** \brief Removes itself from the parent window */
        void remove();

        /** \brief Set a new layout description */
        void set_layout(sip::layout::tool_display::sptr);
    };

    /**
     * @param[in] p the parent window
     * @param[in] v the project that owns tool display
     * @param[in] s the processor associated with this display
     **/
    inline tool_display::tool_display(wxWindow* p, GUI::project* c, processor::reporter::sptr& s) :
                                wxPanel(p, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER),
                                context(c), event_handler(s), content(0) {

      build();

      Show(false);
    }

    namespace detail {

      /**
       * @param[in] s the processor associated with this display
       **/
      inline state_change_handler::state_change_handler(processor::reporter::sptr& s) : owner(s) {
      }

      inline void state_change_handler::state_change_handler::clear() {
        element_for_window.clear();
      }

      inline void state_change_handler::state_change_handler::associate(wxObject* o, sip::layout::element* e) {
        element_for_window[o] = e;
      }
    }
  }
}

#endif

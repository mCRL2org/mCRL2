#ifndef GUI_TOOL_DISPLAY_H
#define GUI_TOOL_DISPLAY_H

#include <wx/wx.h>
#include <wx/panel.h>

#include <sip/detail/layout_tool_display.h>

namespace squadt {
  namespace GUI {

    class project;

    /**
     * @brief Display window associated with a tool
     **/
    class tool_display : wxPanel {
      friend class GUI::project;
      friend class GUI::project::builder;

      private:

        /** \brief The GUI project view to which this display `belongs' */
        GUI::project*                   context;

        /** \brief Abstract description of the layout of this panel */
        sip::layout::tool_display::sptr current_layout;

        /** \brief Sizer that contains the content part */
        wxSizer*                        content;

      private:

        /** \brief Builds the specified layout within this window */
        void instantiate();

        /** \brief Intialises widgets */
        void build();

      public:

        /** \brief Constructor */
        inline tool_display(wxWindow*, project*);

        /** \brief Removes itself from the parent window */
        void remove();

        /** \brief Set a new layout description */
        void set_layout(sip::layout::tool_display::sptr);
    };

    /**
     * @param[in] p the parent window
     * @param[in] v the project that owns tool display
     **/
    inline tool_display::tool_display(wxWindow* p, GUI::project* c) :
                                wxPanel(p, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER),
                                context(c), content(0) {

      build();

      Show(false);
    }
  }
}

#endif

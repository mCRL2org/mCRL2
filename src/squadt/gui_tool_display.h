#ifndef GUI_TOOL_DISPLAY_H
#define GUI_TOOL_DISPLAY_H

#include <wx/wx.h>
#include <wx/panel.h>

#include <sip/detail/layout_tool_display.h>

namespace squadt {
  namespace GUI {

    /**
     * @brief Display window associated with a tool
     **/
    class tool_display : wxPanel {
      public:

        /** \brief Constructor */
        inline tool_display(wxWindow*);

        /** \brief Removes itself from the parent window */
        inline void remove();

        /** \brief Builds the specified layout within this window */
        void instantiate(sip::layout::tool_display&);
    };

    /**
     * @param[in] p the parent window
     **/
    inline tool_display::tool_display(wxWindow* p) : wxPanel(p, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER) {
    }

    inline void tool_display::remove() {
      wxSizer* s = GetParent()->GetSizer();
      
      s->Detach(this);
      s->Layout();

      Destroy();
    }
  }
}

#endif

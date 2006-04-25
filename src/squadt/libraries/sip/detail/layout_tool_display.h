#ifndef LAYOUT_TOOL_DISPLAY
#define LAYOUT_TOOL_DISPLAY

#include "sip/detail/basic_datatype.h"
#include "sip/detail/layout_base.h"
#include "sip/detail/layout_manager.h"
#include "sip/detail/layout_elements.h"

namespace sip {
  namespace layout {

    /** \brief Basic container class for controller-side layout definitions */
    class tool_display {
      private:

        /** \brief the layout manager that contains all widgits for this display */
        layout::manager::sptr top_manager;

      public:

        /** \brief Get the layout manager that contains all widgits for this display */
        inline const layout::manager::sptr get_top_manager() const;
    };

    inline const layout::manager::sptr tool_display::get_top_manager() const {
      return (top_manager);
    }
  }
}

#endif

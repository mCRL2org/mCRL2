#ifndef DISPLAY_LAYOUT_H
#define DISPLAY_LAYOUT_H

#include "sip/detail/basic_datatype.h"
#include "sip/detail/layout_base.h"
#include "sip/detail/layout_manager.h"
#include "sip/detail/layout_widgets.h"

namespace sip {
  namespace layout {

    /** \brief Basic container class for controller-side layout definitions */
    class display_layout {
      private:
        /** \brief the layout manager that contains all widgits for this display */
        layout_manager_ptr top_manager;

      public:
        /** \brief Constructor */
        display_layout();
    };
  }
}

#endif

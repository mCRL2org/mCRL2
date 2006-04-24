#include "gui_tool_display.h"

#include <sip/detail/layout_mediator.h>

namespace squadt {
  namespace GUI {

    using namespace sip::layout;

    /** \brief Provides the means of */
    class tool_display_mediator : public sip::layout::mediator {

      private:

        /** \brief Instantiates a vertically aligned box layout manager */
        inline void build_vertical_box(box < vertical >::alignment&);
      
        /** \brief Instantiates a horizonally aligned box layout manager */
        inline void build_horizontal_box(box < horizontal >::alignment&);
      
        /** \brief Instantiates a label (static text) */
        inline void build_label(std::string&);
      
        /** \brief Instantiates a label */
        inline void build_button(std::string&);
      
        /** \brief Instantiates a single radio button */
        inline void build_radio_button(std::string&);
      
        /** \brief Instantiates a progress bar */
        inline void build_progress_bar(unsigned int, unsigned int, unsigned int);
      
        /** \brief Instantiates a single line text input control */
        inline void build_text_field(std::string&);
    };

    inline void build_vertical_box(box < vertical >::alignment&) {
    }
    
    /** \brief Instantiates a horizonally aligned box layout manager */
    inline void build_horizontal_box(box < horizontal >::alignment&) {
    }
    
    /** \brief Instantiates a label (static text) */
    inline void build_label(std::string&) {
    }
    
    /** \brief Instantiates a label */
    inline void build_button(std::string&) {
    }
    
    /** \brief Instantiates a single radio button */
    inline void build_radio_button(std::string&) {
    }
    
    /** \brief Instantiates a progress bar */
    inline void build_progress_bar(unsigned int, unsigned int, unsigned int) {
    }
    
    /** \brief Instantiates a single line text input control */
    inline void build_text_field(std::string&) {
    }
  }
}


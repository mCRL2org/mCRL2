#ifndef LAYOUT_MEDIATOR_H
#define LAYOUT_MEDIATOR_H

#include <sip/detail/layout_manager.h>

namespace sip {
  namespace layout {

    /** \brief Abstract base class for interaction with a layout element */
    class mediator {
      private:

        /** \brief Instantiates a vertically aligned box layout manager */
        virtual void build_vertical_box(box< vertical >::alignment&);

        /** \brief Instantiates a horizonally aligned box layout manager */
        virtual void build_horizontal_box(box< horizontal >::alignment&);

        /** \brief Instantiates a label (static text) */
        virtual void build_label(std::string&);

        /** \brief Instantiates a label */
        virtual void build_button(std::string&);

        /** \brief Instantiates a single radio button */
        virtual void build_radio_button(std::string&);

        /** \brief Instantiates a progress bar */
        virtual void build_progress_bar(unsigned int, unsigned int, unsigned int);

        /** \brief Instantiates a single line text input control */
        virtual void build_text_field(std::string&);

        virtual ~mediator() = 0;
    };

    inline mediator::~mediator() {
    }
  }
}

#endif

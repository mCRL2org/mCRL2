#ifndef LAYOUT_BASE_H
#define LAYOUT_BASE_H

#include <boost/shared_ptr.hpp>

#include <sip/detail/layout_mediator.h>

namespace sip {
  namespace layout {

    /** \brief Abstract base class for layout elements */
    class element {
      public:

        /** \brief Convenience type for hiding shared pointer implementation */
        typedef boost::shared_ptr < element > sptr;

      public:

        /** \brief Abstract destructor */
        virtual ~element() = 0;

        /** \brief Writes the state of the element to stream */
        virtual void state_write() = 0;

        /** \brief Read state of the element with a reader */
        virtual void state_read();

        /** \brief Recursively serialises the state of the object to a stream */
        virtual void element_write() = 0; 

        /** \brief Recursively builds the state of the object */
        virtual void element_read(); 

        /** \brief Set the callback function that is used to instantiate a layout element */
        virtual mediator::wrapper_aptr instantiate(layout::mediator*);
    };
  }
}
#endif


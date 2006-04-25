#ifndef LAYOUT_BASE_H
#define LAYOUT_BASE_H

#include <iosfwd>

#include <boost/shared_ptr.hpp>

#include <sip/detail/layout_mediator.h>
#include <sip/detail/option.h>

namespace sip {

  namespace layout {

    /** \brief Abstract base class for layout elements */
    class element {

      protected:
        /** \brief Unique identifier for an object */
        sip::option::identifier  id;

      public:

        /** \brief Convenience type for hiding shared pointer implementation */
        typedef boost::shared_ptr < element > sptr;

      public:

        /** \brief Abstract destructor */
        virtual ~element() = 0;

        /** \brief Writes the state of the element to stream */
        virtual void write_state(std::ostream&) = 0;

        /** \brief Read state of the element with a reader */
        virtual void read_state();

        /** \brief Recursively serialises the state of the object to a stream */
        virtual void write_structure(std::ostream&) = 0; 

        /** \brief Recursively builds the state of the object */
        virtual void read_structure(); 

        /** \brief Set the callback function that is used to instantiate a layout element */
        virtual mediator::wrapper_aptr instantiate(layout::mediator*);
    };
  }
}
#endif


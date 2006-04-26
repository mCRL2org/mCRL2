#ifndef LAYOUT_BASE_H
#define LAYOUT_BASE_H

#include <iosfwd>

#include <boost/shared_ptr.hpp>

#include <xml2pp/text_reader.h>

#include <sip/detail/layout_mediator.h>
#include <sip/detail/object.h>

namespace sip {
  namespace layout {

    /** \brief Abstract base class for layout elements */
    class element {

      protected:

        /** \brief Unique identifier for an object */
        sip::object::identifier  id;

      public:

        /** \brief Convenience type for hiding shared pointer implementation */
        typedef boost::shared_ptr < element > sptr;

      public:

        /** \brief Abstract destructor */
        virtual ~element() = 0;

        /** \brief Set the elements id */
        inline void set_id(sip::object::identifier);

        /** \brief Set the elements id */
        inline void set_id(sip::object::identifier&);

        /** \brief Writes the state of the element to stream */
// TODO   virtual void write_state(std::ostream&) = 0;

        /** \brief Read state of the element with a reader */
// TODO   virtual void read_state();

        /** \brief Recursively serialises the state of the object to a stream */
        virtual void write_structure(std::ostream&) = 0; 

        /** \brief Recursively builds the state of the object */
        static element* read_structure(std::string&); 

        /** \brief Recursively builds the state of the object */
        static element* read_structure(xml2pp::text_reader&); 

        /** \brief Set the callback function that is used to instantiate a layout element */
        virtual mediator::wrapper_aptr instantiate(layout::mediator*) = 0;
    };

    inline element::~element() {
    }

    inline void element::set_id(sip::object::identifier& i) {
      id = i;
    }

    inline void element::set_id(sip::object::identifier i) {
      id = i;
    }
  }
}

#endif


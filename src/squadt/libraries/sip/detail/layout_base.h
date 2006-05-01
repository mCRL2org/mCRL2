#ifndef LAYOUT_BASE_H
#define LAYOUT_BASE_H

#include <iostream>
#include <memory>

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

        /** \brief Convenience type for hiding auto pointer implementation */
        typedef std::auto_ptr < element > aptr;

      protected:

        /** \brief writes the element level attributes to stream */
        inline void write_attributes(std::ostream&);

        /** \brief Resets private members to defaults */
        inline void clear();

      public:

        /** \brief Constructor */
        element();

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

        /** \brief Reads element specific data */
        virtual void read(xml2pp::text_reader&); 

        /** \brief Recursively builds the state of the object */
        static element::aptr static_read_structure(std::string&); 

        /** \brief Recursively builds the state of the object */
        static element::aptr static_read_structure(xml2pp::text_reader&); 

        /** \brief Recursively builds the state of the object */
        virtual void read_structure(xml2pp::text_reader&) = 0; 

        /** \brief Set the callback function that is used to instantiate a layout element */
        virtual mediator::wrapper_aptr instantiate(layout::mediator*) const = 0;

        /** \brief Abstract destructor */
        virtual ~element() = 0;
    };

    inline element::element() : id(0) {
    }

    inline element::~element() {
    }

    inline void element::clear() {
      id = 0;
    }

    /**
     * @param[in] o the stream to write to
     **/
    inline void element::write_attributes(std::ostream& o) {
      if (id != 0) {
        o << " id=\"" << id << "\"";
      }
    }

    inline void element::set_id(sip::object::identifier& i) {
      id = i;
    }

    inline void element::set_id(sip::object::identifier i) {
      id = i;
    }

    inline void element::read(xml2pp::text_reader& r) {
      r.get_attribute(&id, "id");
    }
  }
}

#endif


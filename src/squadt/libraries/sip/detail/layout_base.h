#ifndef LAYOUT_BASE_H
#define LAYOUT_BASE_H

#include <iostream>
#include <sstream>
#include <memory>

#include <boost/function.hpp>

#include <xml2pp/text_reader.h>

#include <sip/detail/layout_mediator.h>
#include <sip/detail/object.h>

namespace sip {
  namespace layout {

    /** \brief Abstract base class for layout elements */
    class element {

      public:

        /** \brief Type used for element identification */
        typedef long                        identifier;

        /** \brief Function type for event handlers */
        typedef boost::function < void () > event_handler;

        /** \brief Convenience type for hiding auto pointer implementation */
        typedef std::auto_ptr < element > aptr;

      protected:

        /** \brief Unique identifier for an object */
        identifier id;

      protected:

        /** \brief writes the element level attributes to stream */
        inline void write_attributes(std::ostream&) const;

        /** \brief Resets private members to defaults */
        inline void clear();

      public:

        /** \brief Constructor */
        element();

        /** \brief Set the elements id */
        inline void set_id(element::identifier);

        /** \brief Get the elements id */
        inline element::identifier get_id();

        /** \brief Recursively serialises the state of the object to a stream */
        virtual void write_structure(std::ostream&) const = 0; 

        inline std::string read_state() const;

        /** \brief Reads element specific data */
        virtual void read(xml2pp::text_reader&); 

        /** \brief Recursively builds the state of the object */
        static element::aptr static_read_structure(std::string&); 

        /** \brief Recursively builds the state of the object */
        static element::aptr static_read_structure(xml2pp::text_reader&); 

        /** \brief Recursively builds the state of the object */
        virtual void read_structure(xml2pp::text_reader&) = 0; 

        /** \brief Set the callback function that is used to instantiate a layout element */
        virtual mediator::wrapper_aptr instantiate(layout::mediator*) = 0;

        /** \brief Synchronise with instantiation that is part of a (G)UI */
        virtual void update(layout::mediator*, mediator::wrapper*) const;

        /** \brief Abstract destructor */
        virtual ~element() = 0;
    };

    inline element::element() : id(reinterpret_cast < element::identifier > (this)) {
    }

    inline element::~element() {
    }

    inline void element::clear() {
      id = 0;
    }

    /**
     * @param[in] o the stream to write to
     **/
    inline void element::write_attributes(std::ostream& o) const {
      if (id != 0) {
        o << " id=\"" << id << "\"";
      }
    }

    inline void element::set_id(element::identifier i) {
      id = i;
    }

    inline element::identifier element::get_id() {
      return(id);
    }

    inline void element::read(xml2pp::text_reader& r) {
      r.get_attribute(&id, "id");
    }

    inline std::string element::read_state() const {
      std::ostringstream s;
    
      write_structure(s);
    
      return (s.str());
    }

    /**
     * @param[in] m the mediator object to use
     * @param[in] t pointer to the associated (G)UI object
     **/
    inline void element::update(layout::mediator* m, layout::mediator::wrapper* t) const {
      std::cerr << "No specific update method implemented!\n";
    }
  }
}

#endif


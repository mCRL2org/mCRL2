#ifndef LAYOUT_BASE_H
#define LAYOUT_BASE_H

#include <memory>
#include <map>

#include <boost/function.hpp>

#include <xml2pp/text_reader.h>

#include <sip/detail/layout_mediator.h>

namespace sip {
  namespace tool {
    class communicator;
  }

  namespace layout {

    class basic_event_handler;

    class read_context;

    class element;

    /** \brief Abstract base class for layout elements */
    class element {

      public:

        /** \brief Type used for element identification */
        typedef long                        identifier;

        /** \brief Function type for event handlers */
        typedef boost::function < void () > event_handler;

        /** \brief Convenience type for hiding auto pointer implementation */
        typedef std::auto_ptr < element >   aptr;

        /**
         * \brief Wrapper class for deserialisation
         **/
        class read_context {

          private:

            /** \brief A map for id to element lookup */
            std::map < const element::identifier, element* > id_to_element;

          public:

            /** \brief A xml2pp::text_reader object */
            xml2pp::text_reader&                  reader;

          public:

            /** \brief Constructor */
            inline read_context(xml2pp::text_reader&);

            /** \brief Relates an identifier to an element */
            inline void relate_id_to_element(const element::identifier, element*);

            /** \brief Get the element for an identifier **/
            inline element* element_for_id(const element::identifier id);
        };

      private:

        /** brief The global event handler for all element objects, unless they adopt another one */
        static basic_event_handler  global_event_handler;

      protected:

        /** \brief Unique identifier for an element object */
        identifier id;

        /** brief The current event handler for this object */
        basic_event_handler*        current_event_handler;

      protected:

        /** \brief writes the element level attributes to stream */
        void write_attributes(std::ostream&) const;

      public:

        /** \brief Constructor */
        element();

        /** \brief Set the elements id */
        void set_id(element::identifier);

        /** \brief Get the elements id */
        element::identifier get_id();

        /** \brief Recursively serialises the state of the object to a stream */
        virtual void write_structure(std::ostream&) const = 0; 

        /** \brief Gets a complete structure specification for a layout element */
        std::string get_state() const;

        /** \brief Reads element specific data */
        virtual void read(read_context&); 

        /** \brief Recursively builds the state of the object */
        static element::aptr static_read_structure(std::string&); 

        /** \brief Recursively builds the state of the object */
        static element::aptr static_read_structure(read_context&); 

        /** \brief Recursively builds the state of the object */
        virtual void read_structure(read_context&) = 0; 

        /** \brief Set the callback function that is used to instantiate a layout element */
        virtual mediator::wrapper_aptr instantiate(layout::mediator*) = 0;

        /** \brief Synchronise with instantiation that is part of a (G)UI */
        virtual void update(layout::mediator*, mediator::wrapper*) const;

        /** \brief Immediately send an update using a tool communicator */
        static void update(tool::communicator*, layout::element const*);

        /** \brief Awaits the next change event */
        void await_change() const;

        /** \brief Set the event handler object that will dispatch the events for this object */
        void set_event_handler(basic_event_handler* e);

        /** \brief Recursively traverses layout structure to find an element by its id */
        virtual element* find(element::identifier);

        /** \brief Abstract destructor */
        virtual ~element() = 0;
    };

    inline element::read_context::read_context(xml2pp::text_reader& r) : reader(r) {
    }

    /**
     * @param[in] id the id for which to return the element
     * @param[in] e the element
     **/
    inline void element::read_context::relate_id_to_element(const element::identifier id, element* e) {
      id_to_element[id] = e;
    }

    /**
     * @param[in] id the id for which to return the element
     *
     * \pre element must exist in id_to_element
     **/
    inline element* element::read_context::element_for_id(const element::identifier id) {
      return(id_to_element[id]);
    }
  }
}

#endif


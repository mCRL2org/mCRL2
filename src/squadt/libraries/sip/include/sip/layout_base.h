#ifndef LAYOUT_BASE_H
#define LAYOUT_BASE_H

#include <memory>
#include <map>

#include <boost/function.hpp>

#include <utility/visitor.h>

#include <sip/detail/layout_mediator.h>
#include <sip/visitors.h>

namespace sip {
  namespace tool {
    class communicator;
  }

  namespace layout {

    class basic_event_handler;

    class element;

    /** \brief Abstract base class for layout elements */
    class element : public utility::base_visitable {
      friend class sip::store_visitor;
      friend class sip::restore_visitor;

      public:

        /** \brief Function type for event handlers */
        typedef boost::function < void () >    event_handler;

        /** \brief Convenience type for hiding auto pointer implementation */
        typedef std::auto_ptr < element >      aptr;

      private:

        /** \brief The global event handler for all element objects, unless they adopt another one */
        static basic_event_handler  global_event_handler;

      protected:

        /** \brief The current event handler for this object */
        basic_event_handler*        m_event_handler;

        /** \brief Whether or not the element should grow */
        bool                        m_grow;

        /** \brief Whether or not the element is active */
        bool                        m_enabled;

      public:

        /** \brief Constructor */
        element();

        /** \brief Set whether the element is allowed to grow or not */
        void set_grow(bool b);

        /** \brief Get whether the element is allowed to grow or not */
        bool get_grow();

        /** \brief Enables user interaction */
        void set_enabled(bool b);

        /** \brief Disables user interaction */
        bool get_enabled();

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

        /** \brief Abstract destructor */
        virtual ~element() = 0;
    };

    template < typename T >
    class base_element_impl : public utility::visitable< T > {
    };

    template < typename T >
    class element_impl : public utility::visitable< T >, public sip::layout::element {
    };

    inline void element::set_enabled(bool b) {
      m_enabled = b;
    }

    inline bool element::get_enabled() {
      return (m_enabled);
    }

    inline void element::set_grow(bool b) {
      m_grow = b;
    }

    inline bool element::get_grow() {
      return (m_grow);
    }
  }
}

#endif


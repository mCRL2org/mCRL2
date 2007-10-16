//  Copyright 2007 Jeroen van der Wulp. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tipi/layout_base.hpp

#ifndef LAYOUT_BASE_H
#define LAYOUT_BASE_H

#include <memory>
#include <map>

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include "tipi/utility/generic_visitor.hpp"

#include "tipi/detail/layout_mediator.hpp"

namespace tipi {
  namespace tool {
    class communicator;
  }
  namespace controller {
    class communicator_impl;
  }

  class display;

  namespace layout {
    class basic_event_handler;

    class element;

    /** \brief Abstract base class for layout elements */
    class element : public ::utility::visitable, public boost::noncopyable {

      template < typename R, typename S >
      friend class ::utility::visitor;

      friend class ::tipi::layout::manager;
      friend class ::tipi::display;

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

      private:

        /** 
         * \ brief Factory function for elements
         **/
        template < typename T >
        static boost::shared_ptr < T > create();

      protected:

        /** \brief Activate all handlers */
        void activate_handlers(bool = true);

        /** \brief Set the event handler object that will dispatch the events for this object */
        void set_event_handler(basic_event_handler& e);

        /** \brief Get the event handler object that will dispatch the events for this object */
        basic_event_handler& get_event_handler() const;

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

        /** \brief Awaits the next change event */
        void await_change() const;

        /** \brief Awaits the next change event */
        void on_change(boost::function < void (const void*) >) const;

        /** \brief Abstract destructor */
        virtual ~element() = 0;
    };

    template < typename T >
    inline boost::shared_ptr < T > element::create() {
      boost::shared_ptr < ::tipi::layout::element > p(new T());

      return boost::static_pointer_cast< T > (p);
    }

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


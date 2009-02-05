// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tipi/detail/event_handlers.hpp

#ifndef TIPI_EVENT_HANDLERS_H
#define TIPI_EVENT_HANDLERS_H

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

namespace tipi {
  namespace layout {

    template < typename T >
    class basic_event_handler_impl;

    /**
     * \brief A basic event handler that executes other event handlers that can be registered
     *
     * Multiple handlers for the same id are invoked in arbitrary order.
     *
     * \attention The event handler should always outlive execution of all event handlers.
     **/
    class basic_event_handler {

      private:

        /** \brief implementation object (handle body idiom) */
        boost::shared_ptr< basic_event_handler_impl< const void* > > impl;

      public:

        /** \brief Basic type of event handler functions */
        typedef boost::function < void (const void*) > handler_function;

      public:

        /** \brief Constructor */
        basic_event_handler();

        /** \brief Set a global handler */
        void add(handler_function);

        /** \brief Register an arbitrary handler for a specific object */
        void add(const void*, handler_function);

        /** \brief Whether a specific handler is registered for the object */
        bool has_handler(const void*) const;

        /** \brief Moves registered event handlers that match the id to another object */
        void transfer(basic_event_handler&, const void* = 0);

        /** \brief Remove the global handler */
        void remove();

        /** \brief Remove the handlers for a specific object */
        void remove(const void*);

        /** \brief Process an event for a specific object */
        void process(const void*, bool = true, bool = false);

        /** \brief Execute handlers for a specific object */
        void execute_handlers(const void*, bool);

        /** \brief Block until the next event has been processed */
        void await_change(const void*);

        /** \brief Remove all stored non-global handlers */
        void clear();

        /** \brief Wakes all blocked threads */
        void shutdown();

        /** \brief Constructor */
        ~basic_event_handler();
    };
  }
}
#endif

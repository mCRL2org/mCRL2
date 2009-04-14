// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// \file event_handlers.cpp

#include "boost.hpp" // precompiled headers

#include <map>
#include <deque>
#include <utility>

#include "boost/bind.hpp"
#include "boost/foreach.hpp"
#include "boost/shared_ptr.hpp"

// Workaround for building with Cygwin
#if defined(__CYGWIN__)
#include "boost/asio.hpp"
#endif

#include "boost/thread/thread.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/thread/condition.hpp"

#include <tipi/layout_elements.hpp>
#include <tipi/detail/event_handlers.hpp>

namespace tipi {
  namespace layout {

    /**
     * \cond INTERNAL_DOCS
     **/
    template < typename T >
    class basic_event_handler_impl {
      friend class basic_event_handler;

      private:

        /** \brief Basic type of event handler functions */
        typedef basic_event_handler::handler_function                 handler_function;

        /** \brief Type for the map that associates a number of handlers to a pointer */
        typedef std::multimap< T, handler_function >                  handler_map;

        /** \brief Type for the map that associates a number of handlers to a pointer */
        typedef std::map< T, boost::shared_ptr < boost::condition > > waiter_map;

        /** \brief Type for the queue of events to be processed */
        typedef std::deque< std::pair< T, bool > >                    event_queue;

      private:

        /** \brief For waiter events, and for processing events in a mutual exclusive manner */
        boost::mutex      m_wait_lock;

        /** \brief The list of functions that are to be executed  */
        event_queue       m_events;

        /** \brief The global (default) handler */
        handler_function  m_global_handler;

        /** \brief The list of functions that are to be executed  */
        handler_map       m_handlers;

        /** \brief The synchronisation constructs for waking up waiters */
        waiter_map        m_waiters;

        /** \brief Thread activity flag for handler execution */
        bool              m_handler_execution_thread;

        /** \brief Whether the event handler is still active (or in state of shutdown) */
        bool              m_active;

      public:

        /** \brief Destructor */
        ~basic_event_handler_impl();

      private:

        basic_event_handler_impl() : m_handler_execution_thread(false), m_active(true) {
        }

        /** \brief Wakes up all waiters that match an identifier or all waiters if the identifier is 0 */
        void wake(T = 0);

        /** \brief Set a global handler */
        void add(handler_function);

        /** \brief Register an arbitrary handler for a specific object */
        void add(T, handler_function);

        /** \brief Whether a specific handler is registered for the object */
        bool has_handler(T) const;

        /** \brief Moves registered event handlers that match the id to another object */
        void transfer(basic_event_handler_impl&, T = 0);

        /** \brief Remove the global handler */
        void remove();

        /** \brief Remove the handlers for a specific object */
        void remove(T);

        /** \brief Process an event for a specific object */
        void process(boost::shared_ptr< basic_event_handler_impl > p, T, bool = true, bool = false);

        /** \brief Execute handlers for a specific object */
        void execute_handlers(boost::shared_ptr < basic_event_handler_impl > p);

        /** \brief Execute handlers for a specific object */
        void execute_handlers(const T id, bool b);

        /** \brief Block until the next event has been processed */
        void await_change(T);

        /** \brief Remove all stored non-global handlers */
        void clear();

        /** \brief Move to state of shutdown */
        void shutdown();
    };

    /**
     * \param[in] id a pointer that serves as an identifier for the originator of the event
     *
     * \pre m_wait_lock is in the locked state
     **/
    template < typename T >
    inline void basic_event_handler_impl< T >::wake(T id) {
      typename waiter_map::iterator w = m_waiters.find(id);

      if (w != m_waiters.end()) {
        (*w).second->notify_all();

        m_waiters.erase(w);
      }
    }

    /**
     * \param[in] h a function object that is to be invoked at an event
     **/
    template < typename T >
    inline void basic_event_handler_impl< T >::add(handler_function h) {
      boost::mutex::scoped_lock l(m_wait_lock);

      m_global_handler = h;
    }

    /**
     * \param[in] id a pointer that serves as an identifier for the originator of the event
     * \param[in] h a function object that is to be invoked at an event
     **/
    template < typename T >
    inline void basic_event_handler_impl< T >::add(T id, handler_function h) {
      boost::mutex::scoped_lock l(m_wait_lock);

      m_handlers.insert(std::make_pair(id, h));
    }

    template < typename T >
    inline bool basic_event_handler_impl< T >::has_handler(T id) const {
      return m_handlers.count(id) != 0;
    }

    /**
     * \param[in] e the target event handler object to which the handlers should be relocated
     * \param[in] id a pointer that serves as an identifier for the originator of the event
     **/
    template < typename T >
    inline void basic_event_handler_impl< T >::transfer(basic_event_handler_impl& e, T id) {
      if (&e != this) {
        boost::mutex::scoped_lock l(m_wait_lock);
        boost::mutex::scoped_lock k(e.m_wait_lock);

        std::pair< typename handler_map::iterator, typename handler_map::iterator > p = m_handlers.equal_range(id);

        e.m_handlers.insert(p.first, p.second);

        m_handlers.erase(p.first, p.second);

        typename waiter_map::iterator w = m_waiters.find(id);

        if (w != m_waiters.end()) {
          /* Transfer m_waiters */
          e.m_waiters.insert(*w);

          m_waiters.erase(w);
        }
      }
    }

    /**
     * \param[in] id a pointer that serves as an identifier for the originator of the event
     * \param[in] b whether or not to execute the global handler
     * \param[in] w whether or not to block until processing completes
     **/
    template < typename T >
    inline void basic_event_handler_impl< T >::process(boost::shared_ptr < basic_event_handler_impl > p, T id, bool b, bool w) {
      if (w) {
        if (!m_handler_execution_thread) {
          boost::mutex::scoped_lock l(m_wait_lock);

          m_events.push_back(std::make_pair(id, b));

          m_handler_execution_thread = true;

          boost::thread(boost::bind(&basic_event_handler_impl::execute_handlers, this, p));
        }
      }
      else {
        execute_handlers(id, b);
      }
    }

    /**
     * \param[in] id a pointer that serves as an identifier for the originator of the event
     * \param[in] b whether or not to execute the global handler
     **/
    template < typename T >
    inline void basic_event_handler_impl< T >::execute_handlers(boost::shared_ptr < basic_event_handler_impl > p) {
      boost::mutex::scoped_lock l(m_wait_lock);

      while (0 < m_events.size()) {
        std::pair< T, bool > event(m_events.front());

        try {
          m_events.pop_front();

          if (event.second && !m_global_handler.empty()) {
            m_global_handler(event.first);
          }

          std::pair < typename handler_map::const_iterator,
                      typename handler_map::const_iterator > range(m_handlers.equal_range(event.first));

          BOOST_FOREACH(typename handler_map::value_type p, range) {
            p.second(event.first);
          }

          wake(event.first);
        }
        catch (std::exception& e) {
          std::cerr << e.what() << std::endl;
        }
      }

      m_handler_execution_thread = false;
    }

    template < typename T >
    void basic_event_handler_impl< T >::execute_handlers(const T id, bool b) {
      boost::mutex::scoped_lock l(m_wait_lock);

      if (b && !m_global_handler.empty()) {
        m_global_handler(id);
      }

      std::pair < typename handler_map::const_iterator,
                  typename handler_map::const_iterator > range(m_handlers.equal_range(id));

      BOOST_FOREACH(typename handler_map::value_type p, range) {
        p.second(id);
      }

      wake(id);
    }

    template < typename T >
    inline void basic_event_handler_impl< T >::remove() {
      boost::mutex::scoped_lock l(m_wait_lock);

      m_global_handler.clear();
    }

    /**
     * \param[in] id a pointer that serves as an identifier for the originator of the event
     **/
    template < typename T >
    inline void basic_event_handler_impl< T >::remove(T id) {
      boost::mutex::scoped_lock l(m_wait_lock);

      std::pair < typename handler_map::iterator, typename handler_map::iterator > p = m_handlers.equal_range(id);

      m_handlers.erase(p.first, p.second);

      wake(id);
    }

    /**
     * \param[in] id a pointer that serves as an identifier for the originator of the event
     * \throw std::runtime_error on wake after shutdown
     * \attention also unblocks if the object is destroyed
     **/
    template < typename T >
    inline void basic_event_handler_impl< T >::await_change(T id) {
      boost::mutex::scoped_lock l(m_wait_lock);

      boost::shared_ptr < boost::condition > anchor;

      typename waiter_map::iterator w = m_waiters.find(id);

      if (w == m_waiters.end()) {
        /* Create new waiter */
        anchor = boost::shared_ptr < boost::condition > (new boost::condition);

        m_waiters[id] = anchor;
      }
      else {
        anchor = (*w).second;
      }

      /* The condition with which synchronisation is performed */
      anchor->wait(l);

      if (!m_active) {
        throw std::runtime_error("Waiting for event failed due to premature shutdown.");
      }
    }

    template < typename T >
    inline void basic_event_handler_impl< T >::clear() {
      boost::mutex::scoped_lock l(m_wait_lock);

      m_handlers.clear();
    }

    template < typename T >
    inline void basic_event_handler_impl< T >::shutdown() {
      boost::mutex::scoped_lock l(m_wait_lock);

      m_handlers.clear();

      m_active = false;

      wake();
    }

    template < typename T >
    inline basic_event_handler_impl< T >::~basic_event_handler_impl() {
      shutdown();
    }
    /// \endcond

    basic_event_handler::basic_event_handler() : impl(new basic_event_handler_impl< const void* >) {
    }

    /**
     * \param[in] h a function object that is to be invoked at an event
     **/
    void basic_event_handler::add(handler_function h) {
      impl->add(h);
    }

    /**
     * \param[in] id a pointer that serves as an identifier for the originator of the event
     * \param[in] h a function object that is to be invoked at an event
     **/
    void basic_event_handler::add(const void* id, handler_function h) {
      impl->add(id, h);
    }

    bool basic_event_handler::has_handler(const void* id) const {
      return impl->has_handler(id);
    }

    /**
     * \param[in] e the target event handler object to which the m_handlers should be relocated
     * \param[in] id a pointer that serves as an identifier for the originator of the event
     **/
    void basic_event_handler::transfer(basic_event_handler& e, const void* id) {
      impl->transfer(*e.impl, id);
    }

    /**
     * \param[in] id a pointer that serves as an identifier for the originator of the event
     * \param[in] b whether or not to execute the global handler
     * \param[in] w whether or not to block until processing completes
     **/
    void basic_event_handler::process(const void* id, bool b, bool w) {
      impl->process(impl, id, b, w);
    }

    /**
     * \param[in] id a pointer that serves as an identifier for the originator of the event
     * \param[in] b whether or not to execute the global handler
     **/
    void basic_event_handler::execute_handlers(const void* id, bool b) {
      impl->execute_handlers(id, b);
    }

    void basic_event_handler::remove() {
      impl->remove();
    }

    /**
     * \param[in] id a pointer that serves as an identifier for the originator of the event
     **/
    void basic_event_handler::remove(const void* id) {
      impl->remove(id);
    }

    /**
     * \param[in] id a pointer that serves as an identifier for the originator of the event
     *
     * \attention also unblocks if the object is destroyed
     **/
    void basic_event_handler::await_change(const void* id) {
      impl->await_change(id);
    }

    void basic_event_handler::clear() {
      impl->clear();
    }

    void basic_event_handler::shutdown() {
      impl->shutdown();
    }

    basic_event_handler::~basic_event_handler() {
      shutdown();
    }
  }
}

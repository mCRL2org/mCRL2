// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file include/tipi/detail/basic_messenger.ipp

#ifndef BASIC_MESSENGER_IPP
#define BASIC_MESSENGER_IPP

#include <algorithm>
#include <functional>
#include <sstream>
#include <set>
#include <deque>
#include <map>
#include <iostream>

#include "tipi/detail/transport/detail/transporter.ipp"
#include "tipi/detail/basic_messenger.hpp"
#include "tipi/detail/utility/standard_utility.hpp"
#include "tipi/common.hpp"

#include "boost/ref.hpp"
#include "boost/bind.hpp"
#include "boost/foreach.hpp"
#include "boost/thread/thread.hpp"
#include "boost/thread/condition_variable.hpp"

#include <workarounds.h>

/// \cond INTERNAL_DOCS
/**
 * Workaround for older compilers to force instantiation of
 * boost::recursive_mutex::scoped_lock::unlock
 *
 *  The problem is that the indirect use of this method through boost::bind
 *  does not cause some compilers to instantiate this method
 */
inline bool workaround() {
  boost::recursive_mutex              l;
  boost::recursive_mutex::scoped_lock c(l);

  c.unlock();

  return true;
}

namespace tipi {

  namespace messaging {

    static size_t maximum_buffer_size = 64;

    /**
     * \brief Abstract communicator class that divides an incoming data stream in messages
     *
     * M is the type of a messenger::message or derived type
     */
    template < class M >
    class basic_messenger_impl : public transport::transporter_impl {
      friend class basic_messenger< M >;

      private:

        /** \brief Type for callback functions to handle events */
        typedef typename basic_messenger< M >::handler_type  handler_type;

        /** \brief Strict weak order on handler_type (for use with std::set) */
        class compare_handlers {
          public:

            /** \brief Comparison method */
            inline bool operator()(handler_type const& l, handler_type const& r) const {
              return &l < &r;
            }
        };

        class waiter_data;

        /** \brief Set of handlers */
        typedef std::set < handler_type, compare_handlers >                                    handler_set;

        /** \brief Type for the map used to associate a handler to a message type */
        typedef std::map < typename M::message_type, handler_set >                        handler_map;

        /** \brief Type for the map used to associate a handler to a lock primitive */
        typedef std::map < typename M::message_type, boost::shared_ptr < waiter_data > >  waiter_map;

        /** \brief Type for the message queue */
        typedef std::deque < boost::shared_ptr < const M > >                                   message_queue_t;

      private:

        class delivery_data {

          private:

            mutable boost::mutex      m_lock;

            /** \brief The current task queue (messages to be delivered) */
            message_queue_t           m_tasks;

            boost::function< void() > m_handler;

            boost::thread             m_delivery_thread;

            bool                      m_active;

          public:

            inline delivery_data(boost::function< void() > const& handler) : m_handler(handler), m_active(false) {
              struct trampoline {
                static void execute(bool& active, boost::function< void() > const& handler) {
                  handler();

                  active = false;
                }
              };

              m_handler = boost::bind(&trampoline::execute, boost::ref(m_active), handler);
            }

            inline void push(boost::shared_ptr< const M > const& m) {
              boost::mutex::scoped_lock l(m_lock);

              m_tasks.push_back(m);

              if (!m_active) {
                m_delivery_thread = boost::thread(m_handler);

                m_active = true;
              }
            }

            inline boost::shared_ptr< const M > pop() {
              boost::mutex::scoped_lock l(m_lock);

              boost::shared_ptr< const M > m(m_tasks.front());

              m_tasks.pop_front();

              return m;
            }

            inline size_t size() const {
              boost::mutex::scoped_lock l(m_lock);

              return m_tasks.size();
            }

            inline void clear() {
              boost::mutex::scoped_lock l(m_lock);

              m_tasks.clear();
            }

            inline void stop() {
              boost::mutex::scoped_lock l(m_lock);

              if (m_active) {
                m_delivery_thread.join();
              }

              m_tasks.clear();
            }
        };

        /** \brief Monitor synchronisation construct */
        class waiter_data {

          public:

            /** \brief boost::mutex synchronisation primitive */
            boost::mutex                                     mutex;

            /** \brief boost::condition synchronisation primitive */
            boost::condition_variable                        condition;

          private:

            boost::shared_ptr< const M >                     message;

          public:

            /** \brief Wake up with timouut all treads that are blocked on the condition */
            inline void wake(boost::shared_ptr< const M > const& m) {
              message = m;

              wake();
            }

            /** \brief Wake up all treads that are blocked on the condition */
            inline void wake() {
              boost::mutex::scoped_lock l(mutex);

              condition.notify_all();
            }

            inline boost::shared_ptr< const M > get_message() const {
              return message;
            }
        };

      private:

        /** \brief Handlers based on message types */
        handler_map                        handlers;

        /** \brief For blocking until delivery (used with function await_message) */
        waiter_map                         waiters;

        /** \brief Used to ensure any element t (in M::message_type) in waiters is assigned to at most once */
        boost::recursive_mutex             waiter_lock;

        /** \brief The current message queue (unhandled messages end up here) */
        message_queue_t                    message_queue;

        /** \brief Buffer that holds content until a message is complete */
        std::string                        buffer;

        /** \brief Whether or not a message start tag has been matched after the most recent message end tag */
        bool                               message_open;

        /** \brief The number of tag elements (of message::tag) that have been matched at the last delivery */
        unsigned char                      partially_matched;

        /** \brief Task queue and on demand thread creation for delivery tasks */
        boost::shared_ptr< delivery_data > m_delivery_data;

      protected:

        /** \brief The component used for logging */
        boost::shared_ptr< utility::logger > logger;

      protected:

        /** \brief Type for callback functions to handle events */
        typedef typename M::end_point                        end_point;

        /** \brief Standard (clog) logging component */
        static boost::shared_ptr < utility::logger > get_default_logger() {
          static utility::logger the_default_logger;

          return boost::shared_ptr< utility::logger >(&the_default_logger, ::utility::trivial_deleter< utility::logger >());
        }

      private:

        /** \brief Helper function that services the handlers */
        void service_handlers();

        /** \brief Remove a message from the queue */
        void remove_message(boost::shared_ptr < const M >& p);

        /** \brief Queues incoming messages */
        virtual void deliver(std::istream&, transport::basic_transceiver const*);

        /** \brief Queues incoming messages */
        virtual void deliver(std::string const&, transport::basic_transceiver const*);

      public:

        /** \brief Default constructor */
        basic_messenger_impl();

        /** \brief Default constructor */
        basic_messenger_impl(boost::shared_ptr< utility::logger >&);

        /** \brief Send a message */
        void send_message(M const&);

        /** \brief Wait until the next message of a certain type arrives */
        boost::shared_ptr < const M > await_message(typename M::message_type);

        /** \brief Wait until the next message of a certain type arrives */
        boost::shared_ptr < const M > await_message(typename M::message_type, long const&);

        /** \brief Wait until the first message of type t has arrived */
        boost::shared_ptr < const M > find_message(const typename M::message_type);

        /** \brief Breaks all connections */
        void disconnect();

        /** \brief Set the handler for a type */
        void add_handler(const typename M::message_type, handler_type);

        /** \brief Clears the message queue */
        void clear_queue();

        /** \brief Clears the handlers for a message type */
        void clear_handlers(const typename M::message_type);

        /** \brief Remove a specific handlers for a message type */
        void remove_handler(const typename M::message_type, handler_type);

        /** \brief Destructor */
        virtual ~basic_messenger_impl() {

          disconnect();

          m_delivery_data->stop();

          boost::recursive_mutex::scoped_lock w(waiter_lock);

          // Unblock all waiters;
          BOOST_FOREACH(typename waiter_map::value_type waiter, waiters) {
            waiter.second->wake();
          }

          waiters.clear();
          message_queue.clear();
        }
    };

    /**
     * \param[in] c pointer to an implementation object
     **/
    template < typename M >
    basic_messenger< M >::basic_messenger(boost::shared_ptr < basic_messenger_impl< M > > const& c) : transport::transporter(c) {
    }

    /** Get the current logger */
    template < typename M >
    utility::logger& basic_messenger< M >::get_logger() {
      return *boost::static_pointer_cast < basic_messenger_impl< M > > (impl)->logger;
    }

    /** Returns the standard logger */
    template < typename M >
    utility::logger& basic_messenger< M >::get_default_logger() {
      return *basic_messenger_impl< M >::get_default_logger();
    }

    /** Disconnects from all peers */
    template < typename M >
    void basic_messenger< M >::disconnect() {
      boost::static_pointer_cast < basic_messenger_impl< M > > (impl)->disconnect();
    }

    /**
     * \param[in] m the message that is to be sent
     **/
    template < typename M >
    void basic_messenger< M >::send_message(const M& m) {
      boost::static_pointer_cast < basic_messenger_impl< M > > (impl)->send_message(m);
    }

    /**
     * \param[in] h the handler function that is to be executed
     * \param[in] t the message type on which delivery h is to be executed
     **/
    template < typename M >
    void basic_messenger< M >::add_handler(const typename M::message_type t, handler_type h) {
      boost::static_pointer_cast < basic_messenger_impl< M > > (impl)->add_handler(t, h);
    }

    /**
     * \param[in] t the message type for which to clear the event handler
     **/
    template < typename M >
    void basic_messenger< M >::clear_handlers(const typename M::message_type t) {
      boost::static_pointer_cast < basic_messenger_impl< M > > (impl)->clear_handlers(t);
    }

    /**
     * \param[in] t the message type for which to clear the event handler
     * \param[in] h the handler to remove
     **/
    template < typename M >
    void basic_messenger< M >::remove_handler(const typename M::message_type t, handler_type h) {
      boost::static_pointer_cast < basic_messenger_impl< M > > (impl)->remove_handler(t, h);
    }

    /**
     * \param[in] t the type of the message
     **/
    template < typename M >
    boost::shared_ptr< const M > basic_messenger< M >::await_message(typename M::message_type t) {
      return boost::static_pointer_cast < basic_messenger_impl< M > > (impl)->await_message(t);
    }

    /**
     * \param[in] t the type of the message
     * \param[in] ts the maximum time to wait in seconds
     **/
    template < typename M >
    boost::shared_ptr< const M > basic_messenger< M >::await_message(typename M::message_type t, long const& ts) {
      return boost::static_pointer_cast < basic_messenger_impl< M > > (impl)->await_message(t, ts);
    }

    /**
     * \param[in] l a logger object used to write logging messages to
     *
     * \pre l != 0
     **/
    template < class M >
    inline basic_messenger_impl< M >::basic_messenger_impl(boost::shared_ptr < utility::logger >& l) :
       message_open(false), partially_matched(0), logger(l) {

       m_delivery_data.reset(new delivery_data(boost::bind(&basic_messenger_impl< M >::service_handlers, this)));
    }

    template < class M >
    inline basic_messenger_impl< M >::basic_messenger_impl() :
       message_open(false), partially_matched(0), logger(get_default_logger()) {

       m_delivery_data.reset(new delivery_data(boost::bind(&basic_messenger_impl< M >::service_handlers, this)));
    }

    template < class M >
    inline void basic_messenger_impl< M >::disconnect() {

      transporter_impl::disconnect();
    }

    /**
     * \param d a stream that contains the data to be delived
     * \param o a pointer to the transceiver on which the data was received
     **/
    template < class M >
    void basic_messenger_impl< M >::deliver(std::istream& d, transport::basic_transceiver const* o) {
      std::ostringstream s;

      s << d.rdbuf() << std::flush;

      std::string content = s.str();

      deliver(content, o);
    }

    /**
     * \param m the message that is to be sent
     **/
    template < class M >
    inline void basic_messenger_impl< M >::send_message(const M& m) {
      logger->log(1, boost::format("sent     id : %u, type : %s\n") % getpid() % as_string(m.get_type()));
      logger->log(2, boost::format(" data : \"%s\"\n") % m.to_string());

      send(tipi::visitors::store(m));
    }

    /**
     * \param h the handler function that is to be executed
     * \param t the message type on which delivery h is to be executed
     **/
    template < class M >
    inline void basic_messenger_impl< M >::add_handler(const typename M::message_type t, handler_type h) {
      boost::recursive_mutex::scoped_lock w(waiter_lock);

      if (handlers.count(t) == 0) {
        handlers[t] = std::set < handler_type, compare_handlers >();
      }

      handlers[t].insert(h);
    }

    /**
     * \param[in] t the message type for which to clear the event handler
     **/
    template < class M >
    inline void basic_messenger_impl< M >::clear_queue() {
      boost::recursive_mutex::scoped_lock w(waiter_lock);

      message_queue.clear();
    }

    /**
     * \param t the message type for which to clear the event handler
     **/
    template < class M >
    inline void basic_messenger_impl< M >::clear_handlers(const typename M::message_type t) {
      boost::recursive_mutex::scoped_lock w(waiter_lock);

      if (handlers.count(t) != 0) {
        handlers.erase(t);
      }
    }

    /**
     * \param t the message type for which to clear the event handler
     * \param h the handler to remove
     **/
    template < class M >
    inline void basic_messenger_impl< M >::remove_handler(const typename M::message_type t, handler_type h) {
      boost::recursive_mutex::scoped_lock w(waiter_lock);

      if (handlers.count(t) != 0) {
        handlers[t].erase(h);
      }
    }

    /**
     * \param data a stream that contains the data to be delivered
     * \param o a pointer to the transceiver on which the data was received
     *
     * \attention Works under the assumption that tag_message_close.size() < data.size()
     **/
    template < class M >
    void basic_messenger_impl< M >::deliver(const std::string& data, transport::basic_transceiver const* o) {
      static const std::string tag_message_open("<message ");
      static const std::string tag_message_close("</message>");

      std::string::const_iterator i = data.begin();

      while (i != data.end()) {
        std::string::const_iterator j = i;

        if (message_open) {
          /* The start message tag was matched before */

          if (0 < partially_matched) {
            /* A prefix of the close message tag was matched before */
            j = std::mismatch(tag_message_close.begin() + partially_matched, tag_message_close.end(), j).first;

            const size_t c = (j - tag_message_close.begin()) - partially_matched;

            if (j == tag_message_close.end()) {
              /* Signal that message is closed */
              message_open = false;

              i += tag_message_close.size() - c;
            }

            partially_matched = 0;
          }

          if (message_open) {
            /* Continuing search for the end of the current message; next: try to match close tag */
            size_t n = data.find(tag_message_close, i - data.begin());

            if (n != std::string::npos) {
              /* End message sequence matched; signal message close */
              message_open = false;

              j = data.begin() + n + tag_message_close.size();

              /* Append data to buffer */
              buffer.append(i, j);

              i = j;
            }
            else {
              const std::string::const_iterator b = data.end();
              const size_t                      s = data.size() - (std::min)(tag_message_close.size(), data.size());

              /* End message sequence not matched look for partial match in data[(i - tag_message_close.size())..i] */
              n = data.substr(s).rfind('<');

              if (n != std::string::npos) {
                const std::string::const_iterator k = data.begin() + s + n;

                j = std::mismatch(k, b, tag_message_close.begin()).first;

                if (j == b) {
                  partially_matched = (j - k);
                }
              }

              /* Append */
              buffer.append(i, b);

              i = b;
            }
          }

          if (!message_open) {
            /* End message sequence matched; move message from buffer to queue  */
            std::string new_string;

            new_string.swap(buffer);

            if (!new_string.empty()) {
              boost::shared_ptr< M > message(new M(o));

              tipi::visitors::restore(*message, new_string);

              logger->log(1, boost::format("received id : %u, type : %u\n") % getpid() % as_string(message->get_type()));
              logger->log(2, boost::format(" data : \"%s\"\n") % message->to_string());
              logger->log(4, boost::format(" raw  : \"%s\"\n") % new_string);

              m_delivery_data->push(message);
            }
          }
        }
        else {
          if (0 < partially_matched) {
            const std::string::const_iterator k = tag_message_open.begin() + partially_matched;

            /* Part of a start message tag was matched */
            j = std::mismatch(k, tag_message_open.end(), i).first;

            if (j == tag_message_open.end()) {
              i = data.begin() + tag_message_open.size() - partially_matched;

              buffer.assign(tag_message_open);

              message_open = true;
            }

            partially_matched = 0;
          }

          if (!message_open) {
            size_t n = data.find(tag_message_open, i - data.begin());

            if (n != std::string::npos) {
              /* Skip message tag */
              i = data.begin() + n;

              message_open = true;
            }
            else {
              const std::string::const_iterator b = data.end();
              const size_t                      s = data.size() - (std::min)(tag_message_open.size(), data.size());

              n = data.substr(s).rfind('<');

              if (n != std::string::npos) {
                const std::string::const_iterator k = data.begin() + s + n;

                /* End message sequence not matched look for partial match in data[(i - tag_close.size())..i] */
                j = std::mismatch(k, b, tag_message_open.begin()).first;

                if (j == b) {
                  partially_matched = (j - k);
                }
              }

              i = b;
            }
          }
        }
      }
    }

    /**
     * \param t the type of the message
     **/
    template < class M >
    boost::shared_ptr< const M > basic_messenger_impl< M >::find_message(typename M::message_type t) {
      using namespace boost;

      boost::recursive_mutex::scoped_lock w(waiter_lock);

      boost::shared_ptr < const M > p;

      if (t == M::any()) {
        if (0 < message_queue.size()) {
          p = message_queue.front();
        }
      }
      else {
        for (typename message_queue_t::iterator i = message_queue.begin(); i != message_queue.end(); ++i) {
          if ((*i)->get_type() == t) {
            p = *i;
          }
        }
      }

      return (p);
    }

    /**
     * \param p a reference to a message that points to the message that should be removed from the queue
     * \pre the message must be in the queue
     **/
    template < class M >
    inline void basic_messenger_impl< M >::remove_message(boost::shared_ptr< const M >& p) {
      using namespace boost;

      for (typename message_queue_t::iterator i = message_queue.begin(); i != message_queue.end(); ++i) {
        if (*i == p) {
          message_queue.erase(i);

          return;
        }
      }
    }

    /**
     * \attention Meant to be called from a separate thread
     **/
    template < class M >
    inline void basic_messenger_impl< M >::service_handlers() {
      while (0 < m_delivery_data->size()) {
        boost::shared_ptr< const M > m(m_delivery_data->pop());

        typename M::message_type id = m->get_type();

        boost::recursive_mutex::scoped_lock ww(waiter_lock);

        if (handlers.count(id)) {
          handler_set hs(handlers[id]);

          BOOST_FOREACH(handler_type h, hs) {
            h(m);
          }
        }
        if (id != M::any() && handlers.count(M::any())) {
          handler_set hs(handlers[id]);

          BOOST_FOREACH(handler_type h, hs) {
            h(m);
          }
        }

        if (0 < waiters.count(id)) {
          waiters[id]->wake(m);

          waiters.erase(id);
        }
        if (id != M::any() && 0 < waiters.count(M::any())) {
          waiters[M::any()]->wake(m);

          waiters.erase(M::any());
        }
        else if (waiters.count(id) == 0) {
          /* Put message into queue */
          message_queue.push_back(m);

          if (maximum_buffer_size < message_queue.size()) {
            message_queue.pop_front();
          }
        }
      }
    }

    /**
     * \param[in] t the type of the message
     * \param[in] ts the maximum time to wait in seconds
     **/
    template < class M >
    boost::shared_ptr< const M > basic_messenger_impl< M >::await_message(typename M::message_type t, long const& ts) {
      using namespace boost;

      boost::recursive_mutex::scoped_lock w(waiter_lock);

      boost::shared_ptr < const M > p(find_message(t));

      if (!p) {
        boost::shared_ptr< waiter_data > waiter(waiters[t]);

        if (!waiter) {
          waiter.reset(new waiter_data);

          waiters[t] = waiter;
        }

        boost::mutex::scoped_lock ww(waiter->mutex);

        w.unlock();

        waiter->condition.timed_wait(ww, boost::get_system_time() + boost::posix_time::seconds(ts));

        p = waiter->get_message();

        if (!p) {
          throw std::runtime_error("Communication failure or connection aborted!");
        }
      }
      else {
        remove_message(p);
      }

      return (p);
    }

    /**
     * \param[in] t the type of the message
     **/
    template < class M >
    boost::shared_ptr< const M > basic_messenger_impl< M >::await_message(typename M::message_type t) {
      using namespace boost;

      boost::recursive_mutex::scoped_lock w(waiter_lock);

      boost::shared_ptr< const M > p(find_message(t));

      if (!p) {
        boost::shared_ptr< waiter_data > waiter(waiters[t]);

        if (!waiter) {
          waiter.reset(new waiter_data);

          waiters[t] = waiter;
        }

        boost::mutex::scoped_lock ww(waiter->mutex);

        w.unlock();

        waiter->condition.wait(ww);

        p = waiter->get_message();

        if (!p) {
          throw std::runtime_error("Communication failure or connection aborted!");
        }
      }
      else {
        remove_message(p);
      }

      return (p);
    }
  }
}
/// \endcond

#endif


#ifndef BASIC_MESSENGER_TCC
#define BASIC_MESSENGER_TCC

#include <algorithm>
#include <functional>
#include <sstream>
#include <set>
#include <deque>
#include <map>
#include <iostream>

#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/xtime.hpp>

#include <transport/transporter.h>

#include <sip/detail/basic_messenger.h>
#include <sip/detail/exception.h>

namespace sip {

  namespace messaging {

    /**
     * \brief Abstract communicator class that divides an incoming data stream in messages
     *
     * M is the type of a messenger::message or derived type
     */
    template < class M >
    class basic_messenger_impl : public transport::transporter {
      friend class basic_messenger< M >;

      private:

        /** \brief Type for callback functions to handle events */
        typedef typename basic_messenger< M >::handler_type  handler_type;

        /** \brief Strict weak order on handler_type (for use with std::set) */
        class compare_handlers {
          public:
  
            inline bool operator()(handler_type const&, handler_type const&);
        };

        /** \brief Monitor synchronisation construct */
        class waiter_data {
       
          private:
       
            /** boost::mutex synchronisation primitive */
            boost::mutex                               mutex;
       
            /** boost::condition synchronisation primitive */
            boost::condition                           condition;
       
            /** pointers to local message variables */
            std::vector < boost::shared_ptr < M >* >   pointers;
       
          public:
       
            /** \brief Constructor */
            waiter_data(boost::shared_ptr < M >&);
       
            /** \brief Block until the next message has been delivered, or the object is destroyed */
            void wait(boost::function < void () >);
       
            /** \brief Wake up all treads that are blocked via wait(), and delivers a message */
            void wait(boost::function < void () >, long const&);
       
            /** \brief Wake up all treads that are blocked via wait(), and delivers a message */
            void wake(boost::shared_ptr < M > const&);
       
            /** \brief Wake up all treads that are blocked via wait() */
            void wake();
       
            /** \brief Destructor */
            ~waiter_data();
        };
      private:

        /** \brief Set of handlers */
        typedef std::set < handler_type, compare_handlers >                                    handler_set;

        /** \brief Type for the map used to associate a handler to a message type */
        typedef std::map < typename M::type_identifier_t, handler_set >                        handler_map;

        /** \brief Type for the map used to associate a handler to a lock primitive */
        typedef std::map < typename M::type_identifier_t, boost::shared_ptr < waiter_data > >  waiter_map;

        /** \brief Type for the message queue */
        typedef std::deque < boost::shared_ptr < M > >                                         message_queue_t;

        /** \brief The XML-like tag used for wrapping the content */
        static const std::string                                                               tag_open;

        /** \brief The XML-like tag used for wrapping the content */
        static const std::string                                                               tag_close;

        /** \brief Standard (clog) logging component */
        static utility::print_logger                                                           standard_error_logger;

      private:

        /** \brief Handlers based on message types */
        handler_map                handlers;
 
        /** \brief For blocking until delivery (used with function await_message) */
        waiter_map                 waiters;
 
        /** \brief Used to ensure any element t (in M::type_identifier_t) in waiters is assigned to at most once */
        boost::recursive_mutex     waiter_lock;

        /** \brief The current task queue (messages to be delivered) */
        message_queue_t            task_queue;

        /** \brief The current message queue (unhandled messages end up here) */
        message_queue_t            message_queue;

        /** \brief Buffer that holds content until a message is complete */
        std::string                buffer;
 
        /** \brief Whether or not a message start tag has been matched after the most recent message end tag */
        bool                       message_open;

        /** \brief Whether or not a delivery thread is active */
        volatile bool              delivery_thread_active;

        /** \brief The number of tag elements (of message::tag) that have been matched at the last delivery */
        unsigned char              partially_matched;

      protected:

        /** \brief The component used for logging */
        utility::logger*           logger;

      private:

        /** \brief Helper function that services the handlers */
        void service_handlers();

        /** \brief Remove a message from the queue */
        void remove_message(boost::shared_ptr < M >& p);

      public:

        /** \brief Default constructor */
        basic_messenger_impl(utility::logger* = &standard_error_logger);

        /** \brief Destroys all connections */
        void disconnect();
 
        /** \brief Queues incoming messages */
        virtual void deliver(std::istream&, typename M::end_point);
 
        /** \brief Queues incoming messages */
        virtual void deliver(const std::string&, typename M::end_point);
 
        /* \brief Wait until the next message of a certain type arrives */
        const boost::shared_ptr < M > await_message(typename M::type_identifier_t);
 
        /* \brief Wait until the next message of a certain type arrives */
        const boost::shared_ptr < M > await_message(typename M::type_identifier_t, long const&);
 
        /** \brief Send a message */
        void send_message(const M&);
 
        /** \brief Wait until the first message of type t has arrived */
        boost::shared_ptr < M > find_message(const typename M::type_identifier_t);
 
        /** \brief Set the handler for a type */
        void add_handler(const typename M::type_identifier_t, handler_type);

        /** \brief Clears the handlers for a message type */
        void clear_handlers(const typename M::type_identifier_t);

        /** \brief Remove a specific handlers for a message type */
        void remove_handler(const typename M::type_identifier_t, handler_type);

        /** \brief Destructor */
        ~basic_messenger_impl();
    };

    /**
     * @param[in] l a logger object used to write logging messages to
     *
     * \pre l != 0
     **/
    template < class M >
    inline basic_messenger_impl< M >::basic_messenger_impl(utility::logger* l) :
       message_open(false), partially_matched(0), logger(l) {
    }

    template < class M >
    basic_messenger_impl< M >::~basic_messenger_impl() {
      boost::recursive_mutex::scoped_lock w(waiter_lock);

      // Unblock all waiters
      BOOST_FOREACH(typename waiter_map::value_type w, waiters) {
        w.second->wake();
      }
    }

    template < class M >
    inline void basic_messenger_impl< M >::disconnect() {

      // Unblock all waiters;
      BOOST_FOREACH(typename waiter_map::value_type w, waiters) {
        w.second->wake();
      }

      transporter::disconnect();
    }

    /**
     * @param d a stream that contains the data to be delived
     * @param o a pointer to the transceiver on which the data was received
     **/
    template < class M >
    void basic_messenger_impl< M >::deliver(std::istream& d, typename M::end_point o) {
      std::ostringstream s;
 
      s << d.rdbuf() << std::flush;
 
      std::string content = s.str();
 
      deliver(content, o);
    }

    /**
     * @param m the message that is to be sent
     **/
    template < class M >
    inline void basic_messenger_impl< M >::send_message(const M& m) {
      logger->log(1, boost::format("sent     id : %u, type : %u, data : \"%s\"\n") % getpid() % m.get_type() % m.to_xml());

      send(tag_open + m.to_xml() + tag_close);
    }
 
    template < class M >
    inline bool basic_messenger_impl< M >::compare_handlers::operator()(handler_type const& l, handler_type const& r) {
      return (&l < &r);
    }

    /**
     * @param h the handler function that is to be executed
     * @param t the message type on which delivery h is to be executed
     **/
    template < class M >
    inline void basic_messenger_impl< M >::add_handler(const typename M::type_identifier_t t, handler_type h) {
      boost::recursive_mutex::scoped_lock w(waiter_lock);
      
      if (handlers.count(t) == 0) {
        handlers[t] = std::set < handler_type, compare_handlers >();
      }

      handlers[t].insert(h);
    }

    /**
     * @param t the message type for which to clear the event handler
     **/
    template < class M >
    inline void basic_messenger_impl< M >::clear_handlers(const typename M::type_identifier_t t) {
      boost::recursive_mutex::scoped_lock w(waiter_lock);

      if (handlers.count(t) != 0) {
        handlers.erase(t);
      }
    }
 
    /**
     * @param t the message type for which to clear the event handler
     * @param h the handler to remove
     **/
    template < class M >
    inline void basic_messenger_impl< M >::remove_handler(const typename M::type_identifier_t t, handler_type h) {
      boost::recursive_mutex::scoped_lock w(waiter_lock);

      if (handlers.count(t) != 0) {
        handlers[t].erase(h);
      }
    }

    /**
     * @param data a stream that contains the data to be delived
     * @param o a pointer to the transceiver on which the data was received
     *
     * \pre A message is of the form tag_open...tag_close
     * \pre Both tag_close == tag_open start with == '<' and do not further contain this character 
     *
     * \attention Works under the assumption that tag_close.size() < data.size()
     **/
    template < class M >
    void basic_messenger_impl< M >::deliver(const std::string& data, typename M::end_point o) {
      std::string::const_iterator i = data.begin();

      while (i != data.end()) {
        std::string::const_iterator j = i;
 
        if (message_open) {
          /* The start message tag was matched before */

          if (0 < partially_matched) {
            /* A prefix of the close message tag was matched before */
            j              = std::mismatch(tag_close.begin() + partially_matched, tag_close.end(), j).first;

            const size_t c = (j - tag_close.begin()) - partially_matched;

            if (j == tag_close.end()) {
              /* Signal that message is closed */
              message_open = false;

              /* Remove previously matched part from buffer */
              buffer.resize(buffer.size() - c);

              i += tag_close.size() - c;
            }

            partially_matched = 0;
          }

          if (message_open) {
            /* Continuing search for the end of the current message; next: try to match close tag */
            size_t n = data.find(tag_close, i - data.begin());
           
            if (n != std::string::npos) {
              /* End message sequence matched; signal message close */
              message_open = false;

              j = data.begin() + n;

              /* Append data to buffer */
              buffer.append(i, j);

              /* Skip close tag */
              i = j + tag_close.size();
            }
            else {
              const std::string::const_iterator b = data.end();
              const size_t                      s = data.size() - std::min(tag_close.size(), data.size());

              /* End message sequence not matched look for partial match in data[(i - tag_close.size())..i] */
              n = data.substr(s).rfind('<');

              if (n != std::string::npos) {
                const std::string::const_iterator k = data.begin() + s + n;
               
                j = std::mismatch(k, b, tag_close.begin()).first;
               
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

            typename M::type_identifier_t t = M::extract_type(new_string);

            logger->log(1, boost::format("received id : %u, type : %u, data : \"%s\"\n") % getpid() % t % new_string);

            task_queue.push_back(boost::shared_ptr< M >(new M(new_string, t, o)));

            if (task_queue.size() == 1) {
              boost::recursive_mutex::scoped_lock w(waiter_lock);

              if (!delivery_thread_active) {
                delivery_thread_active = true;

                /* Start delivery thread */
                boost::thread thread(boost::bind(&basic_messenger_impl< M >::service_handlers, this));
              }
            }
          }
        }
        else {
          if (0 < partially_matched) {
            const std::string::const_iterator k = tag_open.begin() + partially_matched;

            /* Part of a start message tag was matched */
            j = std::mismatch(k, tag_open.end(), i).first;

            assert (j == tag_open.end());

            if (j == tag_open.end()) {
              i = data.begin() + tag_open.size() - partially_matched;

              message_open = true;
            }

            partially_matched = 0;
          }

          if (!message_open) {
            size_t n = data.find(tag_open, i - data.begin());
           
            if (n != std::string::npos) {
              /* Skip message tag */
              i = data.begin() + n + tag_open.size();
           
              message_open = true;
            }
            else {
              const std::string::const_iterator b = data.end();
              const size_t                      s = data.size() - std::min(tag_open.size(), data.size());

              n = data.substr(s).rfind('<');

              if (n != std::string::npos) {
                const std::string::const_iterator k = data.begin() + s + n;
               
                /* End message sequence not matched look for partial match in data[(i - tag_close.size())..i] */
                j = std::mismatch(k, b, tag_open.begin()).first;
               
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
     * @param t the type of the message
     **/
    template < class M >
    boost::shared_ptr< M > basic_messenger_impl< M >::find_message(typename M::type_identifier_t t) {
      using namespace boost;

      boost::recursive_mutex::scoped_lock w(waiter_lock);

      boost::shared_ptr < M > p;

      if (t == M::message_any) {
        if (0 < message_queue.size()) {
          p = message_queue.front();
        }
      }
      else {
        typename message_queue_t::iterator i = std::find_if(message_queue.begin(),
                        message_queue.end(),
                        bind(std::equal_to<typename M::type_identifier_t>(), t,
                                bind(&M::get_type, bind(&boost::shared_ptr < M >::get, _1))));

        if (i != message_queue.end()) {
          p = *i;
        }
      }

      return (p);
    }

    /**
     * @param p a reference to message_ptr that points to the message that should be removed from the queue
     * \pre the message must be in the queue
     **/
    template < class M >
    inline void basic_messenger_impl< M >::remove_message(boost::shared_ptr < M >& p) {
      using namespace boost;

      message_queue.erase(std::find_if(message_queue.begin(),
                      message_queue.end(),
                      bind(std::equal_to< M* >(),
                              bind(&boost::shared_ptr< M >::get, p),
                              bind(&boost::shared_ptr< M >::get, _1))));
    }

    /**
     * \attention Meant to be called from a separate thread
     *
     * @param m reference to the pointer to a message to deliver
     * @param o the transceiver that delivered the message
     **/
    template < class M >
    inline void basic_messenger_impl< M >::service_handlers() {

      boost::recursive_mutex::scoped_lock w(waiter_lock);

      while (0 < task_queue.size()) {
        boost::shared_ptr < M > m(task_queue.front());
     
        task_queue.pop_front();
     
        typename M::type_identifier_t id = m->get_type();
     
        if (handlers.count(id)) {
          BOOST_FOREACH(handler_type h, handlers[id]) {
            h(m);
          }
        }
        if (id != M::message_any && handlers.count(M::message_any)) {
          BOOST_FOREACH(handler_type h, handlers[M::message_any]) {
            h(m);
          }
        }
       
        if (0 < waiters.count(id)) {
          waiters[id]->wake(m);
       
          waiters.erase(id);
        }
        if (id != M::message_any && 0 < waiters.count(M::message_any)) {
          waiters[M::message_any]->wake(m);
       
          waiters.erase(M::message_any);
        }
        else if (waiters.count(id) == 0) {
          /* Put message into queue */
          message_queue.push_back(m);
       
          if (16 < message_queue.size()) {
            message_queue.pop_front();
          }
        }
      }

      delivery_thread_active = false;
    }

    /**
     * @param[in] m reference to the pointer to a message to deliver
     **/
    template < class M >
    basic_messenger_impl< M >::waiter_data::waiter_data(boost::shared_ptr < M >& m) {
      pointers.push_back(&m);
    }

    /**
     * @param[in] m reference to the pointer to a message to deliver
     **/
    template < class M >
    void basic_messenger_impl< M >::waiter_data::wake(boost::shared_ptr < M > const& m) {
      boost::mutex::scoped_lock l(mutex);

      BOOST_FOREACH(boost::shared_ptr < M >* i, pointers) {
        *i = m;
      }

      condition.notify_all();
    }

    /**
     * @param[in] m reference to the pointer to a message to deliver
     **/
    template < class M >
    void basic_messenger_impl< M >::waiter_data::wake() {
      boost::mutex::scoped_lock l(mutex);

      condition.notify_all();
    }

    /**
     * @param[in] h a function, called after lock on mutex is obtained and before notification
     **/
    template < class M >
    void basic_messenger_impl< M >::waiter_data::wait(boost::function < void () > h) {
      boost::mutex::scoped_lock l(mutex);

      h();

      condition.wait(l);
    }

    /**
     * @param[in] h a function, called after lock on mutex is obtained and before notification
     * @param[in] ts the maximum time to wait in seconds
     **/
    template < class M >
    void basic_messenger_impl< M >::waiter_data::wait(boost::function < void () > h, long const& ts) {
      using namespace boost;

      mutex::scoped_lock l(mutex);

      h();

      xtime time;

      xtime_get(&time, boost::TIME_UTC);

      time.sec += ts;

      condition.timed_wait(l, time);
    }

    /**
     * @param[in] m reference to the pointer to a message to deliver
     **/
    template < class M >
    basic_messenger_impl< M >::waiter_data::~waiter_data() {
    }

    /**
     * @param[in] t the type of the message
     * @param[in] ts the maximum time to wait in seconds
     **/
    template < class M >
    const boost::shared_ptr< M > basic_messenger_impl< M >::await_message(typename M::type_identifier_t t, long const& ts) {
      using namespace boost;

      boost::recursive_mutex::scoped_lock w(waiter_lock);

      boost::shared_ptr < M > p(find_message(t));

      if (p.get() == 0) {
        if (waiters.count(t) == 0) {
          waiters[t] = boost::shared_ptr < waiter_data > (new waiter_data(p));
        }

        boost::shared_ptr < waiter_data > wd = waiters[t];

        wd->wait(boost::bind(&boost::recursive_mutex::scoped_lock::unlock, &w), ts);
      }
      else {
        remove_message(p);
      }

      return (p);
    }

    /**
     * @param[in] t the type of the message
     **/
    template < class M >
    const boost::shared_ptr< M > basic_messenger_impl< M >::await_message(typename M::type_identifier_t t) {
      using namespace boost;

      boost::recursive_mutex::scoped_lock w(waiter_lock);

      boost::shared_ptr < M > p(find_message(t));

      if (p.get() == 0) {
        if (waiters.count(t) == 0) {
          waiters[t] = boost::shared_ptr < waiter_data > (new waiter_data(p));
        }

        boost::shared_ptr < waiter_data > wd = waiters[t];

        wd->wait(boost::bind(&boost::recursive_mutex::scoped_lock::unlock, &w));
      }
      else {
        remove_message(p);
      }

      return (p);
    }
  }
}

#endif


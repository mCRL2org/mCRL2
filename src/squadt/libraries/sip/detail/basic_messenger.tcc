#ifndef BASIC_MESSENGER_TCC
#define BASIC_MESSENGER_TCC

#include <algorithm>
#include <functional>
#include <sstream>
#include <iostream>

#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

#include <sip/detail/basic_messenger.h>
#include <sip/detail/exception.h>

namespace sip {
  namespace messaging {

    template < class M >
    const std::string basic_messenger< M >::tag_open("<message>");

    template < class M >
    const std::string basic_messenger< M >::tag_close("</message>");

    template < class M >
    basic_messenger< M >::~basic_messenger() {
      boost::recursive_mutex::scoped_lock w(waiter_lock);

      // Unblock all waiters
      for (typename waiter_map::const_iterator i = waiters.begin(); i != waiters.end(); ++i) {
        
        (*i).second->wake();
      }
    }

    /**
     * @param d a stream that contains the data to be delived
     * @param o a pointer to the transceiver on which the data was received
     **/
    template < class M >
    void basic_messenger< M >::deliver(std::istream& d, basic_transceiver* o) {
      std::ostringstream s;
 
      s << d.rdbuf() << std::flush;
 
      std::string content = s.str();
 
      deliver(content, o);
    }

    /**
     * @param m the message that is to be sent
     **/
    template < class M >
    inline void basic_messenger< M >::send_message(const message& m) {
      logger->log(1, boost::format("sent     id : %u, type : %u, data : \"%s\"\n") % getpid() % m.get_type() % m.to_xml());

      send(tag_open + m.to_xml() + tag_close);
    }
 
    /** \pre the message queue is not empty */
    template < class M >
    inline boost::shared_ptr< M > basic_messenger< M >::pop_message() {
      message_ptr m = message_queue.front();
 
      message_queue.pop_front();
 
      return (m);
    }
 
    /** \pre the message queue is not empty  */
    template < class M >
    inline M& basic_messenger< M >::peek_message() {
      message_ptr m = message_queue.front();
 
      message_queue.front();
 
      return (*m);
    }
 
    template < class M >
    inline size_t basic_messenger< M >::number_of_messages() {
      return (message_queue.size());
    }

    template < class M >
    inline bool basic_messenger< M >::compare_handlers::operator()(handler_type const& l, handler_type const& r) {
      return (&l < &r);
    }

    /**
     * @param h the handler function that is to be executed
     * @param t the message type on which delivery h is to be executed
     **/
    template < class M >
    inline void basic_messenger< M >::add_handler(const typename M::type_identifier_t t, handler_type h) {
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
    inline void basic_messenger< M >::clear_handlers(const typename M::type_identifier_t t) {
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
    inline void basic_messenger< M >::remove_handler(const typename M::type_identifier_t t, handler_type h) {
      boost::recursive_mutex::scoped_lock w(waiter_lock);

      if (handlers.count(t) != 0) {
        handlers[t].erase(h);
      }
    }
 
    template < class M >
    inline utility::logger* basic_messenger< M >::get_logger() {
      return (logger);
    }

    template < class M >
    inline utility::logger* basic_messenger< M >::get_standard_error_logger() {
      return (&standard_error_logger);
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
    void basic_messenger< M >::deliver(const std::string& data, basic_transceiver* o) {
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

            message_ptr m(new message(new_string, t));

            logger->log(1, boost::format("received id : %u, type : %u, data : \"%s\"\n") % getpid() % t % new_string);

            boost::thread thread(boost::bind(&basic_messenger< M >::service_handlers, this, m, o));
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
    boost::shared_ptr< M > basic_messenger< M >::find_message(typename M::type_identifier_t t) {
      using namespace boost;

      boost::recursive_mutex::scoped_lock w(waiter_lock);

      message_ptr p;

      if (t == M::message_any) {
        if (0 < message_queue.size()) {
          p = message_queue.front();
        }
      }
      else {
        typename message_queue_t::iterator i = std::find_if(message_queue.begin(),
                        message_queue.end(),
                        bind(std::equal_to<typename M::type_identifier_t>(), t,
                                bind(&M::get_type, bind(&message_ptr::get, _1))));

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
    inline void basic_messenger< M >::remove_message(message_ptr& p) {
      using namespace boost;

      message_queue.erase(std::find_if(message_queue.begin(),
                      message_queue.end(),
                      bind(std::equal_to< M* >(),
                              bind(&message_ptr::get, p),
                              bind(&message_ptr::get, _1))));
    }

    /**
     * \attention Meant to be called from a separate thread
     *
     * @param m reference to the pointer to a message to deliver
     * @param o the transceiver that delivered the message
     **/
    template < class M >
    inline void basic_messenger< M >::service_handlers(const message_ptr m, const basic_transceiver* o) {
      typename M::type_identifier_t id = m->get_type();

      boost::recursive_mutex::scoped_lock w(waiter_lock);

      if (handlers.count(id)) {
        BOOST_FOREACH(handler_type h, handlers[id]) {
          h(m, o);
        }
      }
      if (id != M::message_any && handlers.count(M::message_any)) {
        BOOST_FOREACH(handler_type h, handlers[M::message_any]) {
          h(m, o);
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
      if (waiters.count(id) + waiters.count(M::message_any) == 0) {
        /* Put message into queue */
        message_queue.push_back(m);
      }
    }

    /**
     * @param[in] m reference to the pointer to a message to deliver
     **/
    template < class M >
    basic_messenger< M >::waiter_data::waiter_data(boost::shared_ptr < M >& m) {
      pointers.push_back(&m);
    }

    /**
     * @param[in] m reference to the pointer to a message to deliver
     **/
    template < class M >
    void basic_messenger< M >::waiter_data::wake(boost::shared_ptr < M > const& m) {
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
    void basic_messenger< M >::waiter_data::wake() {
      boost::mutex::scoped_lock l(mutex);

      condition.notify_all();
    }

    /**
     * @param[in] h a function, called after lock on mutex is obtained and before notification
     **/
    template < class M >
    void basic_messenger< M >::waiter_data::wait(boost::function < void () > h) {
      boost::mutex::scoped_lock l(mutex);

      h();

      condition.wait(l);
    }

    /**
     * @param[in] m reference to the pointer to a message to deliver
     **/
    template < class M >
    basic_messenger< M >::waiter_data::~waiter_data() {
    }

    /**
     * @param[in] t the type of the message
     **/
    template < class M >
    const boost::shared_ptr< M > basic_messenger< M >::await_message(typename M::type_identifier_t t) {
      using namespace boost;

      boost::recursive_mutex::scoped_lock w(waiter_lock);

      message_ptr p(find_message(t));

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


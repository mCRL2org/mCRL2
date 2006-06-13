#include <algorithm>
#include <functional>
#include <sstream>

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
      boost::mutex::scoped_lock w(waiter_lock);

      // Unblock all waiters
      const message_ptr m(new message());

      for (typename waiter_map::const_iterator i = waiters.begin(); i != waiters.end(); ++i) {
        boost::mutex::scoped_lock l((*i).second->mutex);

        (*i).second->condition.notify_all();
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

    /**
     * @param h the handler function that is to be executed
     * @param t the message type on which delivery h is to be executed
     **/
    template < class M >
    inline void basic_messenger< M >::add_handler(const typename M::type_identifier_t t, handler_type h) {
      handlers[t] = h;
    }

    /**
     * @param t the message type for which to clear the event handler
     **/
    template < class M >
    inline void basic_messenger< M >::clear_handlers(const typename M::type_identifier_t t) {
      handlers.erase(t);
    }
 
    /**
     * @param t the message type for which to clear the event handler
     * @param h the handler to remove
     **/
    template < class M >
    inline void basic_messenger< M >::remove_handler(const typename M::type_identifier_t t, handler_type h) {
      typename handler_map::const_iterator i = std::find_if(handlers.begin(), handlers.end(), boost::bind(&boost::function_equal, h, _1));

      if (i != handlers.end()) {
        handlers.erase(i);
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
            typename M::type_identifier_t t;

            /* End message sequence matched; move message from buffer to queue  */
            std::string new_string;

            new_string.swap(buffer);

            t = M::extract_type(new_string);

            message_ptr m(new message(new_string, t));
std::cerr << "message " << getpid() << " (" << new_string << ")\n";

            if (handlers.find(sip::any) != handlers.end() || handlers.find(t) != handlers.end()) {
              /* Service handler */
              boost::thread thread(boost::bind(&basic_messenger< M >::service_handlers, this, m, o));
            }
            else {
              /* Put message into queue */
              message_queue.push_back(m);
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
              i += n + tag_open.size();
           
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
     *
     * \pre waiter_lock must be in state locked
     **/
    template < class M >
    inline boost::shared_ptr< M > basic_messenger< M >::find_message(typename M::type_identifier_t t) {
      using namespace boost;

      typename message_queue_t::iterator i = std::find_if(message_queue.begin(),
                      message_queue.end(),
                      bind(std::equal_to<typename M::type_identifier_t>(), t,
                              bind(&M::get_type, bind(&message_ptr::get, _1))));

      return ((i != message_queue.end()) ? *i : message_ptr());
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
     * @param h the handler to call
     **/
    template < class M >
    inline void basic_messenger< M >::service_handlers(const message_ptr m, const basic_transceiver* o) {
      typename M::type_identifier_t id = m->get_type();

      typename handler_map::iterator i = handlers.find(sip::any);
      typename handler_map::iterator j = handlers.find(id);

      boost::mutex::scoped_lock w(waiter_lock);

      if (j != handlers.end()) {
        (*j).second(m, o);
      }
      if (id != any && i != handlers.end()) {
        (*i).second(m, o);
      }

      /* Unblock all possible waiters */
      if (id != any && 0 < waiters.count(sip::any)) {
        boost::mutex::scoped_lock l(waiters[id]->mutex);

        if (0 < waiters.count(id)) {
          waiters[id]->condition.notify_all();
        }

        waiters[sip::any]->condition.notify_all();
      }
      else if (0 < waiters.count(id)) {
        boost::mutex::scoped_lock l(waiters[id]->mutex);

        waiters[id]->condition.notify_all();
      }
    }

    /**
     * @param m reference to the pointer to a message to deliver
     * @param o the transceiver that delivered the message
     * @param t reference to the message pointer of the waiter
     **/
    template < class M >
    inline void basic_messenger< M >::deliver_to_waiter(const message_ptr& m, const basic_transceiver* o, message_ptr& t) {
      t = m;
    }

    /**
     * @param m reference to the pointer to a message to deliver
     * @param o the transceiver that delivered the message
     * @param t reference to the message pointer of the waiter
     * @param h the old handler to call
     **/
    template < class M >
    inline void basic_messenger< M >::deliver_to_waiter(const message_ptr& m, const basic_transceiver* o, message_ptr& t, handler_type h) {
      t = m;

      /** Chain call */
      h(m, o);
    }

    /**
     * @param t the type of the message
     **/
    template < class M >
    const boost::shared_ptr< M > basic_messenger< M >::await_message(typename M::type_identifier_t t) {
      using namespace boost;

      boost::mutex::scoped_lock w(waiter_lock);

      message_ptr p(find_message(t));

      if (p.get() == 0) {
        handler_type              chained_handler;
        bool                      stored = 0 < handlers.count(t);

        if (stored) {
          chained_handler = handlers[t];

          add_handler(t, bind(basic_messenger< M >::deliver_to_waiter, _1, _2, ref(p), handlers[t]));
        }
        else {
          add_handler(t, bind(basic_messenger< M >::deliver_to_waiter, _1, _2, ref(p)));
        }
        
        if (waiters.count(t) == 0) {
          typename monitor::ptr m(new monitor);
        
          waiters[t] = m;
        }

        w.unlock();

        boost::mutex::scoped_lock l(waiters[t]->mutex);

        waiters[t]->condition.wait(l);

        w.lock();

        if (stored) {
          // Restore handler
          handlers[t] = chained_handler;
        }
        else {
          // Remove the added handler
          handlers.erase(t);
        }
      }
      else {
        remove_message(p);
      }

      return (p);
    }
  }
}

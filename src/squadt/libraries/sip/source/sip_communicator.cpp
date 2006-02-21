#include <algorithm>
#include <sstream>

#include <sip/detail/basic_messenger.h>
#include <sip/detail/exception.h>

namespace sip {

  namespace communicator {

    void basic_messenger::deliver(std::istream& data) {
      std::ostringstream s;
 
      s << data.rdbuf() << std::flush;
 
      std::string content = s.str();
 
      deliver(content);
    }
 
    /**
     * \pre{A message is of the form <message>...</message>}
     * \pre{Both message::tag_close == message::tag_open start with == '<' and do not further contain this character}
     *
     * \attention{Works under the assumption that message::tag_close.size() < data.size()}
     **/
    void basic_messenger::deliver(std::string& data) {
      std::string::const_iterator i = data.begin();
 
      while (i != data.end()) {
        std::string::const_iterator j = i;
 
        if (message_open) {
          /* The start message tag was matched before */

          if (0 < partially_matched) {
            /* A prefix of the close message tag was matched before */
            j              = std::mismatch(message::tag_close.begin() + partially_matched, message::tag_close.end(), j).first;

            const size_t c = (j - message::tag_close.begin()) - partially_matched;

            if (j == message::tag_close.end()) {
              /* Signal that message is closed */
              message_open = false;

              /* Remove previously matched part from buffer */
              buffer.resize(buffer.size() - c);

              i += message::tag_close.size() - c;
            }

            partially_matched = 0;
          }

          if (message_open) {
            /* Continuing search for the end of the current message; next: try to match close tag */
            size_t n = data.find(message::tag_close, i - data.begin());
           
            if (n != std::string::npos) {
              /* End message sequence matched; signal message close */
              message_open = false;

              j = data.begin() + n;

              /* Append data to buffer */
              buffer.append(i, j);

              /* Skip close tag */
              i = j + message::tag_close.size();
            }
            else {
              const std::string::const_iterator b = data.end();
              const size_t                      s = data.size() - std::min(message::tag_close.size(), data.size());

              /* End message sequence not matched look for partial match in data[(i - tag_close.size())..i] */
              n = data.substr(s).rfind('<');

              if (n != std::string::npos) {
                const std::string::const_iterator k = data.begin() + s + n;
               
                j = std::mismatch(k, b, message::tag_close.begin()).first;
               
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
            message::type_identifier_t t;

            /* End message sequence matched; move message from buffer to queue  */
            std::string new_string;

            new_string.swap(buffer);

            t = message::extract_type(new_string);

            message_ptr m(new message(t));

            m->set_content(new_string);

            handler_map::iterator h = handlers.find(t);

            if (h != handlers.end()) {
              /* Service handler */
              (*h).second(m);
            }
            else {
              /* Put message into queue */
              message_queue.push_back(m);

              /* Unblock waiters, if necessary */
              if (waiters.count(t) != 0) {
                barrier_ptr b = waiters[t];
                
                waiters.erase(t);

                b->wait();
              }
            }
          }
        }
        else {
          if (0 < partially_matched) {
            const std::string::const_iterator k = message::tag_open.begin() + partially_matched;

            /* Part of a start message tag was matched */
            j = std::mismatch(k, message::tag_open.end(), i).first;

            assert (j == message::tag_open.end());

            if (j == message::tag_open.end()) {
              i = data.begin() + message::tag_open.size() - partially_matched;

              message_open = true;
            }

            partially_matched = 0;
          }

          if (!message_open) {
            size_t n = data.find(message::tag_open, i - data.begin());
           
            if (n != std::string::npos) {
              /* Skip message tag */
              i += n + message::tag_open.size();
           
              message_open = true;
            }
            else {
              const std::string::const_iterator b = data.end();
              const size_t                      s = data.size() - std::min(message::tag_open.size(), data.size());

              n = data.substr(s).rfind('<');

              if (n != std::string::npos) {
                const std::string::const_iterator k = data.begin() + s + n;
               
                /* End message sequence not matched look for partial match in data[(i - tag_close.size())..i] */
                j = std::mismatch(k, b, message::tag_open.begin()).first;
               
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
     * \pre no handler or other waiter for this type is registered
     * \attention must not be called from multiple threads for the same type
     **/
    void basic_messenger::await_message(message::type_identifier_t t) {
      assert(waiters.count(t) == 0 && handlers.count(t) == 0);

      barrier_ptr b(new boost::barrier(2));

      waiters[t] = b;

      b->wait();
    }
  }
}

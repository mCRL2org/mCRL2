#include <algorithm>
#include <sstream>

#include <sip/detail/sip_communicator.h>
#include <sip/detail/exception.h>

namespace sip {

  namespace communicator {

    sip_communicator::sip_communicator() : wait_lock(2), message_open(false), using_lock(false), partially_matched(0) {
    }
 
    void sip_communicator::deliver(std::istream& data) {
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
    void sip_communicator::deliver(std::string& data) {
      std::string::const_iterator i = data.begin();
 
      while (i != data.end()) {
        std::string::const_iterator j = i;
 
        if (message_open) {
          /* The start message tag was matched before */

          if (0 < partially_matched) {
            /* A prefix of the close message tag was matched before */
            j                 = std::mismatch(message::tag_close.begin() + partially_matched, message::tag_close.end(), j).first;
            partially_matched = (j - message::tag_close.begin());

            if (j == message::tag_close.end()) {
              /* Signal that message is closed */
              message_open = false;

              /* Remove previously matched part from buffer */
              buffer.resize(buffer.size() - partially_matched);

              i += message::tag_close.size() - partially_matched;

              /* Append */
              buffer.append(i, j);
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
              const std::string::const_iterator k = j;
              const std::string::const_iterator b = data.end();

              /* End message sequence not matched look for partial match in data[(i - tag_close.size())..i] */
              j = data.begin() + data.rfind('<', data.size() -  message::tag_close.size());
              j = std::mismatch(j, b, message::tag_close.begin()).first;
           
              if (j == b) {
                partially_matched = (j - k);

                /* Append */
                buffer.append(i, k);
              }
              else {
                /* Append */
                buffer.append(i, b);
              }

              i = b;
            }
          }

          if (!message_open) {
            /* End message sequence matched; move message from buffer to queue  */
            std::string new_string;

            new_string.swap(buffer);

            message_ptr m(new message(message::extract_type(new_string)));

            m->set_content(new_string);

            message_queue.push_back(m);

            /* Unblock waiters, if necessary */
            if (using_lock) {
              using_lock = false;
          
              wait_lock.wait();
            }
          }
        }
        else {
          if (0 < partially_matched) {
            /* Part of a start message tag was matched */
            j = std::mismatch(message::tag_open.begin() + partially_matched, message::tag_open.end(), i).first;

            assert (j == message::tag_open.end());

            if (j == message::tag_open.end()) {
              i = data.begin() + (j - message::tag_open.begin());

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
              const std::string::const_iterator k = j;
              const std::string::const_iterator b = data.end();

              /* End message sequence not matched look for partial match in data[(i - tag_close.size())..i] */
              j = data.begin() + data.rfind('<', data.size() - message::tag_open.size());
              j = std::mismatch(j, b, message::tag_open.begin()).first;

              if (j == b) {
                partially_matched = (j - k);
              }

              i = b;
            }
          }
        }
      }
    }
 
    /* Wait until the next message arrives (must not be called from multiple threads) */
    void sip_communicator::await_message() {
      assert(!using_lock);
 
      if (message_queue.size() == 0) {
        using_lock = true;
 
        wait_lock.wait();
      }
    }
  }
}

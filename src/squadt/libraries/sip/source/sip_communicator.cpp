#include <algorithm>
#include <sstream>

#include <sip/detail/sip_communicator.h>

namespace sip {

  namespace communicator {
    sip_communicator::sip_communicator() : wait_lock(2), using_buffer(false), using_lock(false)  {
    }
 
    void sip_communicator::deliver(std::istream& data) {
      std::ostringstream s;
 
      s << data.rdbuf() << std::flush;
 
      std::string content = s.str();
 
      deliver(content);
    }
 
    /* A message is of the form <message>...</message> */
    void sip_communicator::deliver(std::string& data) {
      const std::string start_tag("<message>");
      const std::string end_tag("</message>");
 
      std::string::iterator i = data.begin();
 
      while (i != data.end()) {
        std::string::iterator j = i;
 
        if (using_buffer) {
          size_t old_size = buffer.size();
 
          /* Continuing previous message */
          i = std::search(j, data.end(), end_tag.begin(), end_tag.end());
 
          /* Append data to buffer (should probably add a sanity check for buffer size here) */
          buffer.resize(buffer.size() + (i - j));
 
          std::copy(j, i, buffer.begin() + old_size);
 
          if (i != data.end()) {
            /* Finish message in buffer and add to queue */
            message_ptr m(new message(buffer));
       
            buffer.clear();
 
            message_queue.push_back(m);
 
            /* Unblock waiters, if necessary */
            if (using_lock) {
              using_lock = false;
 
              wait_lock.wait();
            }
 
            i           += end_tag.size();
            using_buffer = false;
          }
        }
        else {
          i = std::search(j, data.end(), start_tag.begin(), start_tag.end());
 
          if (i == data.end()) {
            assert(i != j); // Should be no garbage in between messages ...
          }
          else {
            /* Skip message tag */
            i += start_tag.size();
 
            using_buffer = true;
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

#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/asio/deadline_timer.hpp>

#include "extractor.h"

namespace squadt {

  /**
   * @param[in] t reference to the tool object to use for storage
   **/
  extractor::extractor(tool& t) : task_monitor() {
    add_handler(sip::message_reply_tool_capabilities, bind(&extractor::handle_store_tool_capabilities, this, _1, boost::ref(t)));
  }

  void extractor::terminate_after_timeout(const boost::weak_ptr < void > p) {
    static boost::asio::io_service timing_service;

    // Construct a timer without setting an expiry time.
    boost::asio::deadline_timer timer(timing_service);

    // Set an expiry time relative to now.
    timer.expires_from_now(boost::posix_time::milliseconds(1000));

    // Wait for the timer to expire.
    timer.wait();

    boost::shared_ptr < void > g = p.lock();

    if (g.get() != 0) {
      /* Process object still exists; issue process termination */
      terminate_process();
    }
  }

  /**
   * \pre associated_process.get() is not 0
   **/
  void extractor::start() {
    boost::thread timeout_thread(boost::bind(&extractor::terminate_after_timeout, this, impl));

    /* Await connection */
    await_connection();

    if (is_connected()) {
      request_tool_capabilities();

      await_message(sip::message_reply_tool_capabilities, 1);
    }
  }

  /**
   * @param[in] m the message that was just delivered
   * @param[in,out] t the tool object in which to store the result
   **/
  void extractor::handle_store_tool_capabilities(const sip::message_ptr& m, tool& t) {
    xml2pp::text_reader reader(m->to_string().c_str());

    t.capabilities = sip::tool::capabilities::read(reader);

    finish();
  }
}

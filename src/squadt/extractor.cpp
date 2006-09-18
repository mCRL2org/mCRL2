#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include "extractor.h"

namespace squadt {

  /**
   * @param[in] t reference to the tool object to use for storage
   **/
  extractor::extractor(tool& t) : task_monitor() {
    add_handler(sip::message_reply_tool_capabilities, bind(&extractor::handle_store_tool_capabilities, this, _1, boost::ref(t)));
  }

  /**
   * \pre associated_process.get() is not 0
   **/
  void extractor::extract() {

    /* Await connection */
    await_connection(1);

    if (is_connected()) {
      request_tool_capabilities();

      await_message(sip::message_reply_tool_capabilities, 1);
    }

    finish();
  }

  /**
   * @param[in] m the message that was just delivered
   * @param[in,out] t the tool object in which to store the result
   **/
  void extractor::handle_store_tool_capabilities(const sip::message_ptr& m, tool& t) {
    xml2pp::text_reader reader(m->to_string().c_str());

    t.capabilities = sip::tool::capabilities::read(reader);
  }
}

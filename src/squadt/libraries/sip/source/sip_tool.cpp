#include <boost/ref.hpp>

#include <xml2pp/xml_text_reader.h>

#include <sip/sip_tool.h>
#include <sip/detail/message.h>

namespace sip {
  using namespace sip::communicator;

  /* Request details about the amount of space that the controller currently has reserved for this tool */
  void tool_communicator::request_controller_capabilities() {
    send_message(boost::cref(message(message::request_controller_capabilities)));

    /* Await the reply (too crude, message type might not match) */
    await_message();

    xml2pp::text_reader reader(pop_message().to_string());

    reader.read();

    if (current_capabilities != 0) {
      delete current_capabilities;
    }

    current_capabilities = controller_capabilities::from_xml(reader);
  }

  /* Send a specification of the tools capabilities */
  void tool_communicator::reply_tool_capabilities() {
  }

  /* Send a specification of the current configuration (it may change during tool execution) */
  void tool_communicator::send_accept_configuration() {
  }

  /* Send a layout specification for the display space reserved for this tool */
  void tool_communicator::send_display_layout() {
    std::ostringstream data;

//    current_display_layout.to_xml(data);

    message m(message::send_display_layout);

    m.set_content(data.str());

    send_message(m);
  }

  /* Send a signal that the tool is about to terminate */
  void tool_communicator::send_signal_termination() {
    send_message(boost::cref(message(message::send_signal_termination)));
  }

  /* Send a status report to the controller */
  void tool_communicator::send_report(sip::report&) {
  }
}


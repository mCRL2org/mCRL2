#include <boost/ref.hpp>

#include <xml2pp/xml_text_reader.h>

#include <sip/tool.h>
#include <sip/detail/message.h>
#include <sip/detail/basic_messenger.tcc>

namespace sip {
  using namespace sip::messenger;

  tool_communicator::tool_communicator() : current_status(status_initialising), current_capabilities(0) {

    /* Register event handlers for some message types */
    set_handler(boost::bind(&tool_communicator::reply_tool_capabilities, this), sip::request_tool_capabilities);
  }

  tool_communicator::~tool_communicator() {
  }

  /* Request details about the amount of space that the controller currently has reserved for this tool */
  void tool_communicator::request_controller_capabilities() {
    send_message(boost::cref(sip_message(sip::request_controller_capabilities)));

    /* Await the reply */
    await_message(sip::reply_controller_capabilities);

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

    message m(sip::send_display_layout);

    m.set_content(data.str());

    send_message(m);
  }

  /* Send a signal that the tool is about to terminate */
  void tool_communicator::send_signal_termination() {
    send_message(boost::cref(message(sip::send_signal_termination)));
  }

  /* Send a status report to the controller */
  void tool_communicator::send_report(sip::report&) {
  }
}


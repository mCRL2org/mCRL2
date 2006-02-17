#include <boost/ref.hpp>

#include <xml2pp/xml_text_reader.h>

#include <sip/sip_tool.h>

namespace sip {
  using namespace sip::communicator;

  /** Request details about the amount of space that the controller currently has reserved for this tool */
  void tool_communicator::request_controller_capabilities() {
    send_message(boost::cref(message(message::request_controller_capabilities)));

    /* Await the reply (too crude, message type might not match) */
    await_message();

    if (current_capabilities == 0) {
      delete current_capabilities;
    }

    xml2pp::text_reader reader(pop_message().to_string());

    reader.read();

    current_capabilities = controller_capabilities::from_xml(reader);
  }

  /** Request the list of basic input configurations */
  void tool_communicator::reply_input_configurations() {
  }

  /** Send a specification of the current configuration */
  void tool_communicator::send_accept_configuration() {
  }

  /** Send a layout specification for the display space reserved for this tool */
  void tool_communicator::send_display_layout(sip::layout::display_layout) {
  }

  /** Send a layout specification for the display space reserved for this tool */
  void tool_communicator::send_display_data() {
  }

  /** Send a signal that the tool is about to terminate */
  void tool_communicator::send_termination() {
  }

  /** Signal an error to the controller */
  void tool_communicator::send_error(std::string) {
  }

  /** Send a status report to the controller */
  void tool_communicator::send_report(sip::report&) {
  }
}


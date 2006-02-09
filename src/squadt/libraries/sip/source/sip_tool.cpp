#include <sip/sip_tool.h>

namespace sip {
  /** Request details about the amount of space that the controller currently has reserved for this tool */
  void tool_communicator::request_reserved_space() {
  }

  /** Request the list of basic input configurations */
  void tool_communicator::reply_input_configurations() {
  }

  /** Send a specification of the current configuration */
  void tool_communicator::send_accept_configuration() {
  }

  /** Send a layout specification for the display space reserved for this tool */
  void tool_communicator::send_display_layout(sip::layout) {
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


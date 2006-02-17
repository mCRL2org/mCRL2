#include <sstream>

#include <sip/sip_controller.h>
#include <sip/detail/common.h>

namespace sip {
  using namespace sip::communicator;

  /* Reply details about the amount of reserved display space */
  void controller_communicator::reply_controller_capabilities() {
    std::ostringstream data;

    controller_capabilities capabilities(current_version);

    capabilities.set_display_dimensions(50,50,0);
    capabilities.to_xml(data);

    message m(message::reply_controller_capabilities);

    m.set_content(data.str());

    send_message(m);
  }

  /* Request a tool what input configurations it has available */
  void controller_communicator::request_input_configurations() {
  }

  /* Send the selected input configuration */
  void controller_communicator::send_selected_input_configuration() {
  }

  /* Send data gathered by user interaction */
  void controller_communicator::send_interaction_data() {
  }

  /* Request a tool to terminate */
  void controller_communicator::request_termination() {
  }

  void controller_communicator::trigger(message::message_type t) {
    if (t == message::request_controller_capabilities) {
      reply_controller_capabilities();

      pop_message();
    }
  }
}

#include <sstream>

#include <sip/controller.h>
#include <sip/detail/message.h>
#include <sip/detail/common.h>
#include <sip/detail/basic_messenger.tcc>

#include <boost/bind.hpp>

namespace sip {
  using namespace sip::messenger;

  controller_communicator::controller_communicator() : current_status(status_initialising) {
    current_display_dimensions.x = 0;
    current_display_dimensions.y = 0;
    current_display_dimensions.z = 0;

    /* set default handlers for delivery events */
    set_handler(boost::bind(&controller_communicator::reply_controller_capabilities, this), sip::request_controller_capabilities);
    set_handler(boost::bind(&controller_communicator::set_status, this, status_configured), sip::send_accept_configuration);
    set_handler(boost::bind(&controller_communicator::accept_layout_handler, this, _1), sip::send_display_layout);
    set_handler(boost::bind(&controller_communicator::accept_data_handler, this, _1), sip::send_display_data);
  }

  controller_communicator::~controller_communicator() {
  }

  /* Reply details about the amount of reserved display space */
  void controller_communicator::reply_controller_capabilities() {
    std::ostringstream data;

    controller_capabilities capabilities(protocol_version);

    capabilities.set_display_dimensions(current_display_dimensions);
    capabilities.to_xml(data);

    message m(sip::reply_controller_capabilities);

    m.set_content(data.str());

    send_message(m);
  }

  /* Request a tool what input configurations it has available */
  void controller_communicator::request_tool_capabilities() {
    send_message(boost::cref(message(sip::request_tool_capabilities)));
  }

  /* Send the selected input configuration */
  void controller_communicator::send_configuration() {
    std::ostringstream data;

    current_configuration.to_xml(data);

    message m(sip::reply_controller_capabilities);

    m.set_content(data.str());

    send_message(m);
  }

  /* Send data gathered by user interaction */
  void controller_communicator::send_interaction_data() {
  }

  /* Request a tool to terminate */
  void controller_communicator::request_termination() {
    send_message(boost::cref(message(sip::request_termination)));
  }

  void controller_communicator::accept_layout_handler (sip_messenger::message_ptr&) {
  }

  void controller_communicator::accept_data_handler (sip_messenger::message_ptr&) {
  }
}

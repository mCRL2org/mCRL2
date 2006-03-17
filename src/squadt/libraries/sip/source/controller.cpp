#include <cstdlib>
#include <sstream>

#include <sip/controller.h>
#include <sip/detail/message.h>
#include <sip/detail/common.h>
#include <sip/detail/basic_messenger.tcc>

#include <boost/bind.hpp>

namespace sip {
  using namespace sip::messenger;

  controller_communicator::controller_communicator() : current_status(status_initialising) {
    using namespace boost;

    /* set default handlers for delivery events */
    set_handler(bind(&controller_communicator::accept_tool_capabilities, this, _1), sip::reply_tool_capabilities);
    set_handler(bind(&controller_communicator::accept_instance_identifier, this, _1), sip::send_instance_identifier);
    set_handler(bind(&controller_communicator::reply_controller_capabilities, this), sip::request_controller_capabilities);
    set_handler(bind(&controller_communicator::set_status, this, status_configured), sip::send_accept_configuration);
    set_handler(bind(&controller_communicator::accept_layout_handler, this, _1), sip::send_display_layout);
    set_handler(bind(&controller_communicator::accept_data_handler, this, _1), sip::send_display_data);
  }

  controller_communicator::~controller_communicator() {
  }

  /* Reply details about the amount of reserved display space */
  void controller_communicator::reply_controller_capabilities() {
    message m(current_controller_capabilities.write(), sip::reply_controller_capabilities);

    send_message(m);
  }

  /* Request a tool what input configurations it has available */
  void controller_communicator::request_tool_capabilities() {
    message m(sip::request_tool_capabilities);

    send_message(m);
  }

  /* Send the selected input configuration */
  void controller_communicator::send_configuration() {
    message m(current_configuration->write(), sip::send_configuration);

    send_message(m);
  }

  /* Send data gathered by user interaction */
  void controller_communicator::send_interaction_data() {
  }

  /* Request a tool to terminate */
  void controller_communicator::request_termination() {
    message m(sip::request_termination);

    send_message(m);
  }

  void controller_communicator::accept_tool_capabilities(sip_messenger::message_ptr& m) {
    xml2pp::text_reader reader(m->to_string().c_str());

    reader.read();

    current_tool_capabilities = tool_capabilities::read(reader);
  }

  void controller_communicator::accept_layout_handler(sip_messenger::message_ptr&) {
  }

  void controller_communicator::accept_data_handler(sip_messenger::message_ptr&) {
  }
}

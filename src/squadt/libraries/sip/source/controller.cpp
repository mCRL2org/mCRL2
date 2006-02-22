#include <sstream>

#include <sip/controller.h>
#include <sip/detail/message.h>
#include <sip/detail/common.h>
#include <sip/detail/basic_messenger.tcc>

#include <boost/thread/barrier.hpp>
#include <boost/bind.hpp>

namespace sip {
  using namespace sip::messenger;

  controller_communicator::controller_communicator() : current_status(status_initialising) {
    current_display_dimensions.x = 0;
    current_display_dimensions.y = 0;
    current_display_dimensions.z = 0;

    /* set default delivery event handlers */
    set_handler(boost::bind(&controller_communicator::reply_controller_capabilities, this), sip::request_tool_capabilities);
    set_handler(boost::bind(&controller_communicator::set_status, this, status_configured), sip::send_accept_configuration);
  }

  controller_communicator::~controller_communicator() {
  }

  /* Reply details about the amount of reserved display space */
  void controller_communicator::reply_controller_capabilities() {
    std::ostringstream data;

    controller_capabilities capabilities(current_version);

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
}

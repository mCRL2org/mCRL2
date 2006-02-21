#include <sstream>

#include <sip/controller.h>
#include <sip/detail/message.h>
#include <sip/detail/common.h>
#include <sip/detail/basic_messenger.tcc>

#include <boost/thread/barrier.hpp>
#include <boost/bind.hpp>

namespace sip {
  using namespace sip::messenger;

  /* Reply details about the amount of reserved display space */
  void controller_communicator::reply_controller_capabilities() {
    std::ostringstream data;

    controller_capabilities capabilities(current_version);

    capabilities.set_display_dimensions(current_display_dimensions);
    capabilities.to_xml(data);

    message m(message::reply_controller_capabilities);

    m.set_content(data.str());

    send_message(m);
  }

  /* Request a tool what input configurations it has available */
  void controller_communicator::request_tool_capabilities() {
    send_message(boost::cref(message(message::request_tool_capabilities)));
  }

  /* Send the selected input configuration */
  void controller_communicator::send_configuration() {
    std::ostringstream data;

    current_configuration.to_xml(data);

    message m(message::reply_controller_capabilities);

    m.set_content(data.str());

    send_message(m);
  }

  /* Send data gathered by user interaction */
  void controller_communicator::send_interaction_data() {
  }

  /* Request a tool to terminate */
  void controller_communicator::request_termination() {
    send_message(boost::cref(message(message::request_termination)));
  }

  /** \attention{Works, based on assumption that only one delivery is active for this object at the same time} */
  void controller_communicator::deliver(std::istream& s) {
    communicator::sip_communicator::deliver(s);

    basic_messenger::message_queue::iterator m = _message_queue.begin();

    while (m != _message_queue.end()) {
      switch ((*m)->get_type()) {
        case message::request_controller_capabilities:
          reply_controller_capabilities();

          pop_message();
          break;
        case message::send_accept_configuration:
          current_status = status_configured;

          pop_message();
          break;
        case message::send_display_layout:
          pop_message();
          break;
        case message::send_display_data:
          pop_message();
          break;
        default:
          break;
      }

      ++m;
    }

  }
}

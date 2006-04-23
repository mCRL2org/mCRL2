#include <cstdlib>
#include <sstream>

#include <sip/controller.h>
#include <sip/detail/message.h>
#include <sip/detail/common.h>
#include <sip/detail/basic_messenger.tcc>

#include <boost/bind.hpp>

namespace sip {
  namespace controller {

    using namespace sip::messaging;

    controller::capabilities communicator::current_controller_capabilities;
 
    communicator::communicator() : current_status(status_initialising) {
      using namespace boost;
 
      /* set default handlers for delivery events */
      set_handler(bind(&communicator::reply_controller_capabilities, this), sip::request_controller_capabilities);
      set_handler(bind(&communicator::set_status, this, status_configured), sip::send_accept_configuration);
      set_handler(bind(&communicator::accept_layout_handler, this, _1), sip::send_display_layout);
      set_handler(bind(&communicator::accept_data_handler, this, _1), sip::send_display_data);
    }
 
    communicator::~communicator() {
    }
 
    /* Reply details about the amount of reserved display space */
    void communicator::reply_controller_capabilities() {
      message m(current_controller_capabilities.write(), sip::reply_controller_capabilities);
 
      send_message(m);
    }
 
    /* Request a tool what input configurations it has available */
    void communicator::request_tool_capabilities() {
      message m(sip::request_tool_capabilities);
 
      send_message(m);
    }
 
    /* Send the selected input configuration */
    void communicator::send_configuration() {
      message m(current_configuration->write(), sip::send_configuration);
 
      send_message(m);
    }
 
    /* Send data gathered by user interaction */
    void communicator::send_interaction_data() {
    }
 
    /* Request a tool to terminate */
    void communicator::request_termination() {
      message m(sip::request_termination);
 
      send_message(m);
    }
 
    void communicator::send_start_signal() {
      message m(sip::send_signal_start);
 
      send_message(m);
    }
 
    void communicator::accept_layout_handler(const messenger::message_ptr&) {
    }
 
    void communicator::accept_data_handler(const messenger::message_ptr&) {
    }
  }
}

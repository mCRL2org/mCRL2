#include <cstdlib>
#include <sstream>

#include <boost/bind.hpp>

#include <sip/controller.h>
#include <sip/detail/message.h>
#include <sip/detail/common.h>
#include <sip/detail/basic_messenger.tcc>

namespace sip {
  namespace controller {

    using namespace sip::messaging;

    controller::capabilities communicator::current_controller_capabilities;
 
    /**
     * @param[in] m a reference to the message
     **/
    void communicator::store_configuration(const messenger::message_ptr& m) {
      current_configuration = sip::configuration::read(m->to_string());

      set_status(status_configured);
    }
 
    /**
     * @param[in] c the input combination on which to base the new configuration
     **/
    configuration::sptr communicator::new_configuration(sip::tool::capabilities::input_combination const& c) {
      sip::configuration::sptr nc(new sip::configuration(c.category));

      return (nc);
    }
 
    communicator::communicator() : current_status(status_initialising) {
      using namespace boost;
 
      /* set default handlers for delivery events */
      add_handler(sip::message_request_controller_capabilities, bind(&communicator::reply_controller_capabilities, this));
      add_handler(sip::message_accept_configuration, bind(&communicator::store_configuration, this, _1));
    }

    communicator::~communicator() {
    }
 
    /* Reply details about the amount of reserved display space */
    void communicator::reply_controller_capabilities() {
      message m(current_controller_capabilities.write(), sip::message_reply_controller_capabilities);
 
      send_message(m);
    }
 
    /* Request a tool what input configurations it has available */
    void communicator::request_tool_capabilities() {
      message m(sip::message_request_tool_capabilities);
 
      send_message(m);
    }
 
    /* Send the selected input configuration */
    void communicator::send_configuration() {
      message m(current_configuration->write(), sip::message_offer_configuration);
 
      send_message(m);
    }
 
    /* Request a tool to terminate */
    void communicator::request_termination() {
      message m(sip::message_request_termination);
 
      send_message(m);
    }
 
    void communicator::send_start_signal() {
      message m(sip::message_signal_start);
 
      send_message(m);
    }

    /**
     * @param h the function that is called when a new layout for the display has been received
     **/
    void communicator::activate_display_layout_handler(display_layout_handler_function h) {
      /* Remove any previous handlers */
      clear_handlers(sip::message_display_layout);

      add_handler(sip::message_display_layout, boost::bind(&communicator::display_layout_handler, this, _1, h));

      current_layout_handler = h;
    }

    /**
     * @param d pointer to a tool display
     * @param h the function that is called when a new layout for the display has been received
     *
     * \pre d.get() != 0
     **/
    void communicator::activate_display_data_handler(sip::layout::tool_display::sptr d, display_data_handler_function h) {
      /* Remove any previous handlers */
      clear_handlers(sip::message_display_data);

      add_handler(sip::message_display_data, boost::bind(&communicator::display_data_handler, this, _1, d, h));

      current_data_handler = h;
    }

    /**
     * @param m pointer to the message
     * @param h the function that is called when a new layout for the display has been received
     **/
    void communicator::display_layout_handler(const messenger::message_ptr& m, display_layout_handler_function h) {
      xml2pp::text_reader reader(m->to_string().c_str());

      sip::layout::tool_display::sptr d = layout::tool_display::read(reader);

      h(d);
    }

    /**
     * @param m pointer to the message
     * @param h the function that is called when data for the display has been received
     * @param d a shared pointer to a tool display
     **/
    void communicator::display_data_handler(const messenger::message_ptr& m, sip::layout::tool_display::sptr d, display_data_handler_function h) {
      std::vector < sip::layout::element const* > elements;

      xml2pp::text_reader reader(m->to_string().c_str());

      d->update(reader, elements);

      h(elements);
    }
  }
}

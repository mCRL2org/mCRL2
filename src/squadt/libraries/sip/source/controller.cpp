#include <cstdlib>
#include <sstream>

#include <boost/bind.hpp>

#include <sip/detail/controller.tcc>

namespace sip {
  namespace controller {

    controller::capabilities communicator::current_controller_capabilities;
 
    communicator::communicator(communicator_impl* c) : impl(c) {
    }

    communicator::communicator() : impl(new communicator_impl) {
    }

    /**
     * @param[in] c the current configuration
     **/
    void communicator::set_configuration(boost::shared_ptr < configuration > c) {
      impl->current_configuration = c;
    }
 
    /** \attention use get_configuration().swap() to set the configuration */
    configuration::sptr communicator::get_configuration() const {
      return (impl->current_configuration);
    }
 
    /**
     * @param[in] c the input combination on which to base the new configuration
     **/
    configuration::sptr communicator::new_configuration(sip::tool::capabilities::input_combination const& c) {
      sip::configuration::sptr nc(new sip::configuration(c.category));

      return (nc);
    }
 
    /* Request a tool what input configurations it has available */
    void communicator::request_tool_capabilities() {
      message m(sip::message_request_tool_capabilities);

      impl->send_message(m);
    }
 
    /* Send the selected input configuration */
    void communicator::send_configuration() {
      sip::message m(impl->current_configuration->write(), sip::message_offer_configuration);

      impl->send_message(m);
    }
 
    /* Request a tool to terminate */
    void communicator::request_termination() {
      sip::message m(sip::message_request_termination);

      impl->send_message(m);
    }
 
    void communicator::send_start_signal() {
      sip::message m(sip::message_signal_start);

      impl->send_message(m);
    }

    /**
     * @param h the function that is called when a new layout for the display has been received
     **/
    void communicator::activate_display_layout_handler(display_layout_handler_function h) {
      impl->activate_display_layout_handler(h);
    }

    /**
     * @param d pointer to a tool display
     * @param h the function that is called when a new layout for the display has been received
     *
     * \pre d.get() != 0
     **/
    void communicator::activate_display_data_handler(sip::layout::tool_display::sptr d, display_data_handler_function h) {
      impl->activate_display_data_handler(d, h);
    }

    /**
     * @param h the function that is called when a new layout for the display has been received
     **/
    void communicator::activate_status_message_handler(status_message_handler_function h) {
      impl->activate_status_message_handler(h);
    }
    /**
     * @param[in] e a sip layout element of which the data is to be sent
     **/
    void communicator::send_display_data(sip::layout::element const* e) {
      message m(e->get_state(), sip::message_display_data);

      impl->send_message(m);
    }

    /**
     * @param[in] t the type of the message
     **/
    const sip::message_ptr communicator::await_message(sip::message::type_identifier_t t) {
      return (impl->await_message(t));
    }

    /**
     * @param h the handler function that is to be executed
     * @param t the message type on which delivery h is to be executed
     **/
    void communicator::add_handler(const sip::message::type_identifier_t t, sip::message_handler_type h) {
      impl->add_handler(t, h);
    }

    /**
     * @param t the message type for which to clear the event handler
     * @param h the handler to remove
     **/
    void communicator::remove_handler(const sip::message::type_identifier_t t, sip::message_handler_type h) {
      impl->remove_handler(t, h);
    }

    utility::logger* communicator::get_logger() const {
      return (impl->get_logger());
    }

    utility::logger* communicator::get_standard_error_logger() {
      return (sip::messenger::get_standard_error_logger());
    }
  }
}

#include <cstdlib>
#include <sstream>

#include <boost/bind.hpp>

#include <sip/detail/controller.tcc>

namespace sip {
  namespace layout {
    /**
     * \param[in] t the tool communicator object to use
     * \param[in] e the layout element of which to send the state
     **/
    void element::update(tool::communicator* t, layout::element const* e) {
    }
  }
  namespace controller {

    controller::capabilities communicator::m_controller_capabilities;
 
    communicator::communicator(communicator_impl* c) : sip::messenger(c) {
    }

    communicator::communicator() : sip::messenger(new communicator_impl) {
    }

    /**
     * \param[in] c the current configuration
     **/
    void communicator::set_configuration(boost::shared_ptr < sip::configuration > c) {
      boost::dynamic_pointer_cast < communicator_impl > (impl)->m_configuration = c;
    }
 
    /** \attention use get_configuration().swap() to set the configuration */
    boost::shared_ptr < configuration > communicator::get_configuration() const {
      return (boost::dynamic_pointer_cast < communicator_impl > (impl)->m_configuration);
    }
 
    /**
     * \param[in] c the input combination on which to base the new configuration
     **/
    boost::shared_ptr < configuration > communicator::new_configuration(sip::tool::capabilities::input_combination const& c) {
      return (communicator_impl::new_configuration(c));
    }

    /* Request a tool what input configurations it has available */
    void communicator::request_tool_capabilities() {
      message m(sip::message_request_tool_capabilities);

      impl->send_message(m);
    }
 
    /* Send the selected input configuration */
    void communicator::send_configuration() {
      sip::message m(visitors::store(
                *boost::dynamic_pointer_cast < communicator_impl > (impl)->m_configuration),
                                                                sip::message_offer_configuration);

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
     * \param h the function that is called when a new layout for the display has been received
     **/
    void communicator::activate_display_layout_handler(display_layout_handler_function h) {
      boost::dynamic_pointer_cast < communicator_impl > (impl)->activate_display_layout_handler(h);
    }

    /**
     * \param d pointer to a tool display
     * \param h the function that is called when a new layout for the display has been received
     *
     * \pre d.get() != 0
     **/
    void communicator::activate_display_update_handler(sip::layout::tool_display::sptr d, display_update_handler_function h) {
      boost::dynamic_pointer_cast < communicator_impl > (impl)->activate_display_update_handler(d, h);
    }

    /**
     * \param h the function that is called when a new layout for the display has been received
     **/
    void communicator::activate_status_message_handler(status_message_handler_function h) {
      boost::dynamic_pointer_cast < communicator_impl > (impl)->activate_status_message_handler(h);
    }
    /**
     * \param[in] e a sip layout element of which the data is to be sent
     **/
    void communicator::send_display_update(sip::layout::element const* e) {
      message m(e->get_state(), sip::message_display_update);

      impl->send_message(m);
    }
  }
}

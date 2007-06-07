#include <cstdlib>
#include <sstream>

#include <boost/bind.hpp>

#include <sip/detail/controller.tcc>
#include <sip/detail/event_handlers.h>

namespace sip {
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
      boost::static_pointer_cast < communicator_impl > (impl)->m_configuration = c;
    }
 
    /** \attention use get_configuration().swap() to set the configuration */
    boost::shared_ptr < configuration > communicator::get_configuration() const {
      return (boost::static_pointer_cast < communicator_impl > (impl)->m_configuration);
    }
 
    /**
     * \param[in] c the input combination on which to base the new configuration
     **/
    boost::shared_ptr < configuration > communicator::new_configuration(sip::tool::capabilities::input_combination const& c) {
      return (communicator_impl::new_configuration(c));
    }

    /* Request a tool what input configurations it has available */
    void communicator::request_tool_capabilities() {
      impl->send_message(sip::message_request_tool_capabilities);
    }
 
    /* Send the selected input configuration */
    void communicator::send_configuration(boost::shared_ptr < sip::configuration > const& c) {
      impl->send_message(sip::message(visitors::store(*c), sip::message_offer_configuration));
    }
 
    /* Request a tool to terminate */
    void communicator::request_termination() {
      impl->send_message(sip::message_request_termination);
    }
 
    void communicator::send_start_signal() {
      impl->send_message(sip::message_signal_start);
    }

    void communicator::deactivate_display_layout_handler() {
      boost::static_pointer_cast < communicator_impl > (impl)->deactivate_display_layout_handler();
    }

    /**
     * \param h the function that is called when a new layout for the display has been received
     **/
    void communicator::activate_display_layout_handler(display_layout_handler_function h) {
      boost::static_pointer_cast < communicator_impl > (impl)->
        activate_display_layout_handler(boost::static_pointer_cast < communicator_impl > (impl), h);
    }

    void communicator::deactivate_display_update_handler() {
      boost::static_pointer_cast < communicator_impl > (impl)->deactivate_display_update_handler();
    }

    /**
     * \param d pointer to a tool display
     * \param h the function that is called when a new layout for the display has been received
     *
     * \pre d.get() != 0
     **/
    void communicator::activate_display_update_handler(sip::layout::tool_display::sptr d, display_update_handler_function h) {
      boost::static_pointer_cast < communicator_impl > (impl)->
        activate_display_update_handler(boost::static_pointer_cast < communicator_impl > (impl), d, h);
    }

    void communicator::deactivate_status_message_handler() {
      boost::static_pointer_cast < communicator_impl > (impl)->deactivate_status_message_handler();
    }

    /**
     * \param h the function that is called when a new layout for the display has been received
     **/
    void communicator::activate_status_message_handler(status_message_handler_function h) {
      boost::static_pointer_cast < communicator_impl > (impl)->
        activate_status_message_handler(boost::static_pointer_cast < communicator_impl > (impl), h);
    }

    /**
     * \param[in] e a sip layout element of which the data is to be sent
     * \param[in] display the associated sip::display element
     **/
    void communicator::send_display_update(sip::layout::element const& e, boost::shared_ptr < sip::display const >& display) {
      std::string        c;

      {
        sip::store_visitor v(c);

        v.visit(e, display->find(&e));
      }

      impl->send_message(sip::message(c, sip::message_display_update));
    }

    /**
     * \param[in] impl weak pointer to this object (for life check)
     * \param[in] m pointer to the message
     * \param[in] h the function that is called when a new layout for the display has been received
     * \pre impl.get() == this
     **/
    void communicator_impl::display_layout_handler(boost::weak_ptr < communicator_impl > impl, messenger::message_ptr const& m, display_layout_handler_function h) {
      struct trampoline { 
        inline static void send_display_data(boost::weak_ptr < communicator_impl > impl, void const* e, boost::shared_ptr< sip::display const > display) { 
          boost::shared_ptr < communicator_impl > g(impl.lock());

          if (g.get() != 0) {
            std::string c; 

            { 
              sip::store_visitor v(c); 
	  
              v.visit(*reinterpret_cast < sip::layout::element const* > (e), display->find(reinterpret_cast < sip::layout::element const* > (e))); 
            } 

            g->send_message(sip::message(c, sip::message_display_update)); 
          }
        } 
      }; 

      boost::shared_ptr < communicator_impl > g(impl.lock());

      if (g.get() != 0) {
        sip::layout::tool_display::sptr d(new layout::tool_display);

        visitors::restore(*d, m->to_string());

        if (d->get_manager()) { 
          d->get_manager()->get_event_handler()->connect(boost::bind(&trampoline::send_display_data, impl, _1, d)); 
        } 

        h(d);
      }
    }
  }
}

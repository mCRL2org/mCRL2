//  Copyright 2007 Jeroen van der Wulp. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file source/controller/communicator.cpp

#include <cstdlib>
#include <sstream>

#include <boost/bind.hpp>

#include <tipi/detail/controller.tcc>
#include "tipi/detail/event_handlers.hpp"

namespace tipi {
  namespace controller {

    controller::capabilities communicator::m_controller_capabilities;
 
    communicator::communicator() :
        tipi::messenger(boost::shared_ptr < tipi::messaging::basic_messenger_impl< tipi::message > > (new communicator_impl)) {
    }

    /**
     * \param[in] c an implementation object
     **/
    communicator::communicator(boost::shared_ptr < communicator_impl > const& c) :
        tipi::messenger(boost::static_pointer_cast< tipi::messaging::basic_messenger_impl < tipi::message > > (c)) {
    }

    /**
     * \param[in] c the current configuration
     **/
    void communicator::set_configuration(boost::shared_ptr < tipi::configuration > c) {
      boost::static_pointer_cast < communicator_impl > (impl)->m_configuration = c;
    }
 
    /** \attention use get_configuration().swap() to set the configuration */
    boost::shared_ptr < configuration > communicator::get_configuration() const {
      return (boost::static_pointer_cast < communicator_impl > (impl)->m_configuration);
    }
 
    /**
     * \param[in] c the input combination on which to base the new configuration
     **/
    boost::shared_ptr < configuration > communicator::new_configuration(tipi::tool::capabilities::input_combination const& c) {
      return (communicator_impl::new_configuration(c));
    }

    /* Request a tool what input configurations it has available */
    void communicator::request_tool_capabilities() {
      boost::static_pointer_cast < communicator_impl > (impl)->send_message(tipi::message_capabilities);
    }
 
    /* Send the selected input configuration */
    void communicator::send_configuration(boost::shared_ptr < tipi::configuration > const& c) {
      boost::static_pointer_cast < communicator_impl > (impl)->send_message(tipi::message(visitors::store(*c), tipi::message_configuration));
    }
 
    /* Request a tool to terminate */
    void communicator::request_termination() {
      boost::static_pointer_cast < communicator_impl > (impl)->send_message(tipi::message_termination);
    }
 
    void communicator::send_start_signal() {
      boost::static_pointer_cast < communicator_impl > (impl)->send_message(tipi::message_task_start);
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
    void communicator::activate_display_update_handler(tipi::layout::tool_display::sptr d, display_update_handler_function h) {
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
     * \param[in] e a tipi layout element of which the data is to be sent
     * \param[in] display the associated tipi::display element
     **/
    void communicator::send_display_update(tipi::layout::element const& e, boost::shared_ptr < tipi::display const >& display) {
      std::string        c;

      try {
        tipi::store_visitor v(c);

        v.visit(e, display->find(&e));
      }
      catch (bool b) {
        // find failed for some reason
      }

      boost::static_pointer_cast < communicator_impl > (impl)->send_message(tipi::message(c, tipi::message_display_data));
    }

    /**
     * \param[in] impl weak pointer to this object (for life check)
     * \param[in] m pointer to the message
     * \param[in] h the function that is called when a new layout for the display has been received
     * \pre impl.get() == this
     **/
    void communicator_impl::display_layout_handler(boost::weak_ptr < communicator_impl > impl, messenger::message_ptr const& m, display_layout_handler_function h) {
      struct trampoline { 
        inline static void send_display_data(boost::weak_ptr < communicator_impl > impl, void const* e, boost::shared_ptr< tipi::display const > display) { 
          boost::shared_ptr < communicator_impl > g(impl.lock());

          if (g.get() != 0) {
            std::string c; 

            try { 
              tipi::store_visitor v(c); 

              v.visit(*reinterpret_cast < tipi::layout::element const* > (e),
                display->find(reinterpret_cast < tipi::layout::element const* > (e))); 
            } 
            catch (bool b) {
              // find failed for some reason
            }

            g->send_message(tipi::message(c, tipi::message_display_data)); 
          }
        } 
      }; 

      boost::shared_ptr < communicator_impl > g(impl.lock());

      if (g.get() != 0) {
        tipi::layout::tool_display::sptr d(new layout::tool_display);
 
        // Make sure the global event handler (the default event handler) does not have a global event is empty
        tipi::layout::element::global_event_handler.remove();

        try {
          visitors::restore(*d, m->to_string());
      
          if (d->get_manager()) {
            d->get_manager()->get_event_handler()->add(boost::bind(&trampoline::send_display_data, impl, _1, d)); 
      
            h(d);
          }
        }
        catch (std::runtime_error& e) {
          logger->log(1, "Failure with interpretation of message: `" + std::string(e.what()) + "'\n");
        }
      }
    }
  }
}

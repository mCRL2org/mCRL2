// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING).
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file controller/communicator.cpp

#include <exception>
#include <cstdlib>
#include <sstream>

#include <boost/bind.hpp>

#include <tipi/detail/controller.ipp>
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
    communicator::communicator(boost::shared_ptr < tipi::controller::communicator_impl > const& c) :
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
    boost::shared_ptr < configuration > communicator::new_configuration(tipi::tool::capabilities::input_configuration const& c) {
      return (communicator_impl::new_configuration(c));
    }

    /* Request a tool what input configurations it has available */
    void communicator::request_tool_capabilities() {
      boost::static_pointer_cast < communicator_impl > (impl)->send_message(tipi::message_capabilities);
    }
 
    /**
     * \pre c.get != 0
     * Send the selected input configuration
     **/
    void communicator::send_configuration(boost::shared_ptr < tipi::configuration > const& c) {
        boost::static_pointer_cast < communicator_impl > (impl)->send_message(tipi::message(visitors::store(*c), tipi::message_configuration));
    }
 
    /* Request a tool to terminate */
    void communicator::request_termination() {
      boost::static_pointer_cast < communicator_impl > (impl)->send_message(tipi::message_termination);
    }
 
    void communicator::send_start_signal() {
      boost::static_pointer_cast < communicator_impl > (impl)->send_message(tipi::message_task);
    }

    /**
     * \param hi the function that is called when a new layout for the display has been received
     * \param hu the function that is called when an update for the current layout has been received
     **/
    void communicator::activate_display_layout_handling(display_layout_handler_function const& hi, display_update_handler_function const& hu) {
      boost::static_pointer_cast < communicator_impl > (impl)->
        activate_display_layout_handling(boost::static_pointer_cast < communicator_impl > (impl), hi, hu);
    }

    void communicator::deactivate_display_layout_handling() {
      boost::static_pointer_cast < communicator_impl > (impl)->deactivate_display_layout_handling();
    }

    void communicator::deactivate_status_message_handler() {
      boost::static_pointer_cast < communicator_impl > (impl)->deactivate_status_message_handler();
    }

    /**
     * \param h the function that is called when a new layout for the display has been received
     **/
    void communicator::activate_status_message_handler(status_message_handler_function& h) {
      boost::static_pointer_cast < communicator_impl > (impl)->
        activate_status_message_handler(boost::static_pointer_cast < communicator_impl > (impl), h);
    }

    /// \cond INTERNAL_DOCS
    communicator_impl::communicator_impl() {
      using namespace boost;

      struct trampoline {
        static void capabilities(boost::shared_ptr< const tipi::message >& m, communicator_impl& impl) {
          if (m->is_empty()) {
            message m(visitors::store(communicator::m_controller_capabilities), tipi::message_capabilities);
        
            impl.send_message(m);
          }
        }
        static void configuration(boost::shared_ptr< const tipi::message >& m, communicator_impl& impl) {
          impl.m_configuration.reset(new tipi::configuration);

          visitors::restore(*impl.m_configuration, m->to_string());
        }
      };

      /* set default handlers for delivery events */
      add_handler(tipi::message_capabilities, bind(&trampoline::capabilities, _1, boost::ref(*this)));
      add_handler(tipi::message_configuration, bind(&trampoline::configuration, _1, boost::ref(*this)));
    }
    
    /**
     * \param[in] impl a weak pointer to this object (for lifetime check)
     * \param h the function that is called when a new layout for the display has been received
     *
     * \note deactivates event handling if h.empty()
     **/
    void communicator_impl::activate_status_message_handler(boost::weak_ptr < communicator_impl > impl, status_message_handler_function& h) {
      struct trampoline {
        static void status_message(boost::shared_ptr< const tipi::message >& m, status_message_handler_function h) {
          boost::shared_ptr < tipi::report > r(new tipi::report);

          tipi::visitors::restore(*r, m->to_string());

          h(r);
        }
      };

      boost::shared_ptr < communicator_impl > g(impl.lock());

      if (g.get() != 0) {
        /* Remove any previous handlers */
        clear_handlers(tipi::message_report);

        if (!h.empty()) {
          add_handler(tipi::message_report, boost::bind(&trampoline::status_message, _1, h));
        }
      } 
    }

    /**
     * \param[in] impl a weak pointer to this object (for lifetime check)
     * \param[in] h the function that is called when a new layout for the display has been received
     *
     * \note deactivates event handling if h.empty()
     **/
    void communicator_impl::activate_display_layout_handling(boost::weak_ptr < communicator_impl > impl, display_layout_handler_function const& hi, display_update_handler_function const& hu) {
      struct trampoline {
        static void update(boost::shared_ptr< const tipi::message >& m, boost::weak_ptr < tipi::layout::tool_display > d, display_update_handler_function h) {
          boost::shared_ptr < tipi::layout::tool_display > g(d.lock());

          if (g) {
            std::vector < tipi::layout::element const* > elements;
     
            g->update(m->to_string(), elements);
     
            h(elements);
          }
        }

        static void send_display_data(boost::weak_ptr < communicator_impl > impl, void const* e, boost::shared_ptr< tipi::display const > display) { 
          boost::shared_ptr < communicator_impl > g(impl.lock());
      
          if (g.get() != 0) {
            std::string c; 
      
            try {
              tipi::store_visitor v(c); 
      
              if (dynamic_cast< tipi::layout::element const* > (reinterpret_cast < tipi::layout::element const* > (e))) { // safe to do reinterpret cast
                v.visit(*reinterpret_cast < tipi::layout::element const* > (e),
                  display->find(reinterpret_cast < tipi::layout::element const* > (e))); 
              }
            } 
            catch (bool) {
              // find failed for some reason
            }
      
            g->send_message(tipi::message(c, tipi::message_display_data)); 
          }
        } 

        static void instantiate(boost::shared_ptr< const tipi::message >& m, boost::weak_ptr < communicator_impl > impl, display_layout_handler_function h1, display_update_handler_function h2) {
          boost::shared_ptr < communicator_impl > g(impl.lock());

          if (g) {
            tipi::layout::tool_display::sptr d(new layout::tool_display);
          
            // Make sure the global event handler (the default event handler does not have a global event) is empty
            d->remove();
          
            try {
              visitors::restore(*d, m->to_string());
            
              d->add(boost::bind(&trampoline::send_display_data, g, _1, d)); 
           
              // Register handler function
              g->clear_handlers(tipi::message_display_data);
           
              g->add_handler(tipi::message_display_data, boost::bind(&trampoline::update, _1, d, h2));
          
              h1(d);
            }
            catch (std::runtime_error& e) {
              g->logger->log(1, "Failure with interpretation of message: `" + std::string(e.what()) + "'\n");
            }
          }
        }
      };

      boost::shared_ptr < communicator_impl > g(impl.lock());

      if (g.get() != 0) {
        /* Remove any previous handlers */
        clear_handlers(tipi::message_display_layout);

        add_handler(tipi::message_display_layout, boost::bind(&trampoline::instantiate, _1, g, hi, hu));
      }
    }
    /// \endcond
  }
}

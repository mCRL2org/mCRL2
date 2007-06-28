//  Copyright 2007 Jeroen van der Wulp. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file include/tipi/detail/controller.tcc

#ifndef TIPI_CONTROLLER_TCC_
#define TIPI_CONTROLLER_TCC_

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <tipi/controller.h>
#include <tipi/configuration.h>
#include <tipi/controller/capabilities.h>
#include <tipi/detail/basic_messenger.tcc>
#include <tipi/common.h>
#include <tipi/visitors.h>

namespace tipi {
  namespace controller {

    class communicator_impl : public tipi::messaging::basic_messenger_impl< tipi:: message > {
      friend class communicator;
      
      private:

        /** \brief call-back function type for layout changes of the display */
        typedef communicator::display_layout_handler_function display_layout_handler_function;

        /** \brief call-back function type for updates to the display */
        typedef communicator::display_update_handler_function display_update_handler_function;

        /** \brief call-back function type for diagnostic messages */
        typedef communicator::status_message_handler_function status_message_handler_function;

      private:

        /** \brief Handler function to replace the current display layout with a new one */
        void display_layout_handler(boost::weak_ptr < communicator_impl >, messenger::message_ptr const&, display_layout_handler_function);
 
        /** \brief Handler function to replace the current display layout with a new one */
        void display_update_handler(messenger::message_ptr const&, tipi::layout::tool_display::sptr, display_update_handler_function);

        /** \brief Handler function to replace the current display layout with a new one */
        void status_message_handler(messenger::message_ptr const&, status_message_handler_function);

        /** \brief Send details about the amount of space that the controller currently has reserved for this tool */
        void request_controller_capabilities_handler();
 
        /** \brief Event handler for storing configurations */
        void tool_configuration_handler(messenger::message_ptr const& m);

        /** \brief Creates a new configuration object */
        static boost::shared_ptr < configuration > new_configuration(tipi::tool::capabilities::input_combination const& c);

      private:

        /** \brief The current configuration of a tool (may be limited to a main input configuration) */
        boost::shared_ptr < configuration >  m_configuration;

      public:

        /** \brief Default constructor */
        communicator_impl();

        /** \brief Clears handlers for display change messages */
        void deactivate_display_layout_handler();

        /** \brief Sets a handler for layout messages using a handler function */
        void activate_display_layout_handler(boost::weak_ptr < communicator_impl >, display_layout_handler_function);

        /** \brief Clears handlers for display update messages */
        void deactivate_display_update_handler();

        /** \brief Sets a handler for layout messages using a handler function */
        void activate_display_update_handler(boost::weak_ptr < communicator_impl >, tipi::layout::tool_display::sptr, display_update_handler_function);

        /** \brief Clears handlers for status messages */
        void deactivate_status_message_handler();

        /** \brief Sets a handler for layout messages using a handler function */
        void activate_status_message_handler(boost::weak_ptr < communicator_impl >, status_message_handler_function);
    };

    inline communicator_impl::communicator_impl() {
      using namespace boost;

      /* set default handlers for delivery events */
      add_handler(tipi::message_controller_capabilities, bind(&communicator_impl::request_controller_capabilities_handler, this));
      add_handler(tipi::message_configuration, bind(&communicator_impl::tool_configuration_handler, this, _1));
    }
    
    inline void communicator_impl::deactivate_display_layout_handler() {
      clear_handlers(tipi::message_display_layout);
    }

    /**
     * \param[in] impl a weak pointer to this object (for lifetime check)
     * \param[in] h the function that is called when a new layout for the display has been received
     *
     * \note deactivates event handling if h.empty()
     **/
    inline void communicator_impl::activate_display_layout_handler(boost::weak_ptr < communicator_impl > impl, display_layout_handler_function h) {
      boost::shared_ptr < communicator_impl > g(impl.lock());

      if (g.get() != 0) {
        /* Remove any previous handlers */
        clear_handlers(tipi::message_display_layout);

        add_handler(tipi::message_display_layout, boost::bind(&communicator_impl::display_layout_handler, this, g, _1, h));
      }
    }

    inline void communicator_impl::deactivate_display_update_handler() {
      clear_handlers(tipi::message_display_update);
    }

    /**
     * \param[in] impl a weak pointer to this object (for lifetime check)
     * \param[in,out] d pointer to a tool display
     * \param[in] h the function that is called when a new layout for the display has been received
     *
     * \pre d.get() != 0
     * \note deactivates event handling if h.empty()
     **/
    inline void communicator_impl::activate_display_update_handler(boost::weak_ptr < communicator_impl > impl, tipi::layout::tool_display::sptr d, display_update_handler_function h) {
      boost::shared_ptr < communicator_impl > g(impl.lock());

      if (g.get() != 0) {
        /* Remove any previous handlers */
        clear_handlers(tipi::message_display_update);

        if (!h.empty()) {
          add_handler(tipi::message_display_update, boost::bind(&communicator_impl::display_update_handler, this, _1, d, h));
        }
      }
    }

    inline void communicator_impl::deactivate_status_message_handler() {
      clear_handlers(tipi::message_report);
    }

    /**
     * \param[in] impl a weak pointer to this object (for lifetime check)
     * \param h the function that is called when a new layout for the display has been received
     *
     * \note deactivates event handling if h.empty()
     **/
    inline void communicator_impl::activate_status_message_handler(boost::weak_ptr < communicator_impl > impl, status_message_handler_function h) {
      boost::shared_ptr < communicator_impl > g(impl.lock());

      if (g.get() != 0) {
        /* Remove any previous handlers */
        clear_handlers(tipi::message_report);

        if (!h.empty()) {
          add_handler(tipi::message_report, boost::bind(&communicator_impl::status_message_handler, this, _1, h));
        }
      } 
    }

    /**
     * \param m pointer to the message
     * \param h the function that is called when data for the display has been received
     * \param d a shared pointer to a tool display
     **/
    inline void communicator_impl::display_update_handler(const messenger::message_ptr& m, tipi::layout::tool_display::sptr d, display_update_handler_function h) {
      if (d.get() != 0) {
        std::vector < tipi::layout::element const* > elements;

        d->update(m->to_string(), elements);

        h(elements);
      }
    }

    /**
     * \param m pointer to the message
     * \param h the function that is called when a new report has been received
     **/
    inline void communicator_impl::status_message_handler(const messenger::message_ptr& m, status_message_handler_function h) {
      boost::shared_ptr < tipi::report > r(new tipi::report);

      tipi::visitors::restore(*r, m->to_string());

      h(r);
    }

    /* Reply details about the amount of reserved display space */
    inline void communicator_impl::request_controller_capabilities_handler() {
      message m(visitors::store(communicator::m_controller_capabilities), tipi::message_controller_capabilities);

      send_message(m);
    }
 
    /**
     * \param[in] m a reference to the message
     **/
    inline void communicator_impl::tool_configuration_handler(const messenger::message_ptr& m) {
      m_configuration.reset(new configuration);

      visitors::restore(*m_configuration, m->to_string());
    }

    /**
     * \param[in] c the input combination on which to base the new configuration
     **/
    inline boost::shared_ptr < configuration > communicator_impl::new_configuration(tipi::tool::capabilities::input_combination const& c) {
      boost::shared_ptr < configuration > nc(new tipi::configuration(c.m_category));

      return (nc);
    }
  }
}
 

#endif

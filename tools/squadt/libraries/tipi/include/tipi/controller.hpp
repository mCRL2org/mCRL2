//  Copyright 2007 Jeroen van der Wulp. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file include/tipi/controller.h

#ifndef TIPI_CONTROLLER_H
#define TIPI_CONTROLLER_H

#include <vector>

#include "tipi/utility/logger.hpp"

#include "tipi/common.hpp"
#include "tipi/tool/capabilities.hpp"
#include "tipi/controller/capabilities.hpp"
#include "tipi/report.hpp"
#include "tipi/display.hpp"
#include "tipi/detail/layout_elements.hpp"
#include "tipi/detail/basic_messenger.hpp"

/* Interface classes for the tool side of the Squadt Interaction Protocol */
namespace tipi {
  namespace controller {

    class communicator_impl;

    /** \brief Specifies controller capabilities */
    class capabilities;

    /**
     * \class communicator controller.h
     * \brief The main interface to the protocol (controller-side)
     **/
    class communicator : public messaging::basic_messenger< tipi::message > {
      friend class communicator_impl;

      protected:
 
        /** \brief The capabilities object of the controller as it is send, when requested */
        static controller::capabilities         m_controller_capabilities;

      public:

        /** \brief Function type that for communicating display layouts */
        typedef boost::function < void (tipi::layout::tool_display::sptr) >                     display_layout_handler_function;

        /** \brief Function type that for communicating display layouts */
        typedef boost::function < void (std::vector < tipi::layout::element const* > const&) >  display_update_handler_function;

        /** \brief Function type that for communicating display layouts */
        typedef boost::function < void (tipi::report::sptr) >                                   status_message_handler_function;

      protected:
 
        /** \brief Alternate constructor */
        communicator(communicator_impl*);

      public:

        /** \brief Default constructor */
        communicator();

        /** \brief Get the controller_capabilities object that is send to tools */
        const controller::capabilities& get_controller_capabilities();
 
        /** \brief Request the list of basic input configurations */
        void request_tool_capabilities();

        /** \brief Send a specification of a (perhaps partial) configuration */
        void send_configuration(boost::shared_ptr < tipi::configuration > const&);
 
        /** \brief Send data to update the state of the last communicated display layout */
        void send_display_update(tipi::layout::element const&, boost::shared_ptr < tipi::display const >&);
 
        /** \brief Sends a message to a tool that it may start processing */
        void send_start_signal();
 
        /** \brief Request the tool to terminate itself */
        void request_termination();
 
        /** \brief Set the current (perhaps partial) configuration */
        void set_configuration(boost::shared_ptr < tipi::configuration >);

        /** \brief Creates a new configuration object based on a given input_combination */
        static boost::shared_ptr < configuration > new_configuration(tipi::tool::capabilities::input_combination const&);
 
        /** \brief Get the current (perhaps partial) configuration */
        boost::shared_ptr < configuration > get_configuration() const;

        /** \brief Clears handlers for display change messages */
        void deactivate_display_layout_handler();

        /** \brief Sets a handler for layout messages using a handler function */
        void activate_display_layout_handler(display_layout_handler_function);

        /** \brief Clears handlers for display update messages */
        void deactivate_display_update_handler();

        /** \brief Sets a handler for layout messages using a handler function */
        void activate_display_update_handler(tipi::layout::tool_display::sptr, display_update_handler_function);

        /** \brief Clears handlers for status messages */
        void deactivate_status_message_handler();

        /** \brief Sets a handler for layout messages using a handler function */
        void activate_status_message_handler(status_message_handler_function);
    };
 
    inline const controller::capabilities& communicator::get_controller_capabilities() {
      return (m_controller_capabilities);
    }
  }
}

#endif


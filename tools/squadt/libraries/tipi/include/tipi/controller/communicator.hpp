// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tipi/controller/communicator.hpp
/// \brief Main interface for the controller (protocol communication partner)

#ifndef TIPI_CONTROLLER_H
#define TIPI_CONTROLLER_H

#include <vector>

#include "tipi/report.hpp"
#include "tipi/tool_display.hpp"
#include "tipi/layout_elements.hpp"
#include "tipi/detail/basic_messenger.hpp"
#include "tipi/controller/capabilities.hpp"
#include "tipi/tool/capabilities.hpp"

/* Interface classes for the tool side of the Squadt Interaction Protocol */
namespace tipi {
  namespace controller {

    /// \cond INTERNAL_DOCS
    class communicator_impl;
    /// \endcond

    /** \brief Specifies controller capabilities */
    class capabilities;

    /**
     * \class communicator
     * \brief The main interface to the protocol (controller-side)
     **/
    class communicator : public messaging::basic_messenger< tipi::message > {
    /// \cond INTERNAL_DOCS
      friend class communicator_impl;
    /// \endcond

      private:

        /** \brief The capabilities object of the controller as it is send, when requested */
        static controller::capabilities         m_controller_capabilities;

      public:

        /** \brief Function type that for communicating display layouts */
        typedef boost::function < void (boost::shared_ptr< tipi::tool_display >) >              display_layout_handler_function;

        /** \brief Function type that for communicating display layouts */
        typedef boost::function < void (std::vector < tipi::layout::element const* > const&) >  display_update_handler_function;

        /** \brief Function type that for communicating display layouts */
        typedef boost::function < void (boost::shared_ptr< tipi::report >) >                    status_message_handler_function;

      protected:

        /** \brief Constructor for derived implementation objects */
        communicator(boost::shared_ptr < tipi::controller::communicator_impl > const&);

      public:

        /** \brief Default constructor */
        communicator();

        /** \brief Get the controller_capabilities object that is send to tools */
        const controller::capabilities& get_controller_capabilities();

        /** \brief Request the list of basic input configurations */
        void request_tool_capabilities();

        /** \brief Send a specification of a (perhaps partial) configuration */
        void send_configuration(boost::shared_ptr < tipi::configuration > const&);

        /** \brief Sends a message to a tool that it may start processing */
        void send_start_signal();

        /** \brief Request the tool to terminate itself */
        void request_termination();

        /** \brief Set the current (perhaps partial) configuration */
        void set_configuration(boost::shared_ptr < tipi::configuration >);

        /** \brief Creates a new configuration object based on a given input_configuration */
        static boost::shared_ptr < configuration > new_configuration(tipi::tool::capabilities::input_configuration const&);

        /** \brief Get the current (perhaps partial) configuration */
        boost::shared_ptr < configuration > get_configuration() const;

        /** \brief Clears handlers for display change messages */
        void deactivate_display_layout_handling();

        /** \brief Sets a handler for layout messages using a handler function */
        void activate_display_layout_handling(display_layout_handler_function const&, display_update_handler_function const&);

        /** \brief Clears handlers for status messages */
        void deactivate_status_message_handler();

        /** \brief Sets a handler for layout messages using a handler function */
        void activate_status_message_handler(status_message_handler_function&);
    };

    inline const controller::capabilities& communicator::get_controller_capabilities() {
      return (m_controller_capabilities);
    }
  }
}

#endif


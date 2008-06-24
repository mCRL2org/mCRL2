// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tipi/tool.hpp
/// \brief Main interface for a tool (protocol communication partner)

#ifndef TIPI_TOOL_H
#define TIPI_TOOL_H

#include "tipi/tool_display.hpp"
#include "tipi/layout_elements.hpp"
#include "tipi/detail/utility/logger.hpp"
#include "tipi/report.hpp"
#include "tipi/tool/capabilities.hpp"
#include "tipi/tool/category.hpp"

/* Interface classes for the tool side */
namespace tipi {
  namespace controller {
    /** \brief Specifies capabilities */
    class capabilities;
  }

  namespace tool {

    /// \cond INTERNAL_DOCS
    class communicator_impl;
    /// \endcond

    /** \brief Specifies capabilities */
    class capabilities;

    /** \brief The main interface to the protocol implementation (tool-side) */
    class communicator : public tipi::messenger {
      friend class layout::element;

    /// \cond INTERNAL_DOCS
      friend class communicator_impl;
    /// \endcond

      private:

        /** \brief Send data to update the state of the last communicated display layout */
        void send_display_data(layout::element const*);
 
        /** \brief Set the current tool configuration object */
        inline void set_configuration(boost::shared_ptr < configuration >);

      protected:

        /** \brief Constructor for derived implementation object */
        communicator(boost::shared_ptr < tipi::tool::communicator_impl > const&);
 
      public:
 
        /** \brief Default constructor */
        communicator();
 
        /** \brief Activate the communicator via connection arguments from the command line */
        bool activate(int&, char** const);

        /** \brief Activate the communicator via connection arguments from a vector of arguments */
        bool activate(char*&);

        /** \brief Activate the communicator via connection arguments from a vector of arguments */
        bool activate(std::vector < std::string> const&);
 
        /** \brief Request details about the amount of space that the controller currently has reserved for this tool */
        void request_controller_capabilities();
 
        /** \brief Signal that the current configuration is complete enough for the tool to start processing */
        void send_configuration();

        /** \brief Signal that the current configuration is complete enough for the tool to start processing */
        void send_configuration(tipi::configuration&);
 
        /** \brief Send a layout specification for the display space reserved for this tool */
        void send_display_layout(tool_display&);
 
        /** \brief Sends the empty layout specification for the display space */
        void send_clear_display();
 
        /** \brief Send a signal that the tool has finished its last operation */
        void send_task_done(bool);
 
        /** \brief Send a signal that the tool is about to terminate */
        void send_signal_termination();
 
        /** \brief Send a status report to the controller */
        void send_report(tipi::report const&) const;

        /** \brief Sends an error report to the controller */
        void send_status_report(tipi::report::type, std::string const&) const;
 
        /** \brief Get the tool capabilities object that will be sent when a request is received */
        tool::capabilities& get_capabilities();
 
        /** \brief Get the current tool configuration object be sent when a request is received */
        configuration& get_configuration();
 
        /** \brief Get the last communicated set of controller capabilities */
        const boost::shared_ptr < controller::capabilities > get_controller_capabilities() const;

        /** \brief Waits until a configuration is delivered and returns a pointer to it */
        void await_configuration() const;
    };
  }
}

#endif

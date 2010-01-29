// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file include/tipi/detail/tool.ipp
#ifndef __TIPI_TOOL_IPP__
#define __TIPI_TOOL_IPP__

#include "tipi/detail/basic_messenger.ipp"
#include "tipi/detail/command_line_interface.hpp"
#include "tipi/detail/message.hpp"
#include "tipi/detail/visitors.hpp"
#include "tipi/tool/communicator.hpp"
#include "tipi/tool/capabilities.hpp"
#include "tipi/layout_base.hpp"
#include "tipi/common.hpp"

namespace tipi {
  namespace tool {

    /// \cond INTERNAL_DOCS
    class communicator_impl : public tipi::messaging::basic_messenger_impl< tipi::message > {
      friend class communicator;
      friend class messaging::scheme;

      private:

        /** \brief The last received set of controller capabilities */
        boost::shared_ptr < controller::capabilities > current_controller_capabilities;

        /** \brief The object that described the capabilities of the current tool */
        tool::capabilities                             current_tool_capabilities;

        /** \brief This object reflects the current configuration */
        boost::shared_ptr < configuration >            current_configuration;

        /** \brief This object reflects the current configuration */
        boost::shared_ptr < display >                  current_display;

        /** \brief Unique identifier for the running tool, obtained via the command line */
        long                                           instance_identifier;

      private:

        /** \brief Send details about the controllers capabilities */
        void handle_capabilities_request(boost::shared_ptr< const tipi::message >&);

        /** \brief Extract a configuration from an offer_configuration message */
        void receive_configuration_handler(boost::shared_ptr< const tipi::message >& m);

        /** \brief Wakes waiters if no connections remain */
        void on_disconnect(messaging::basic_messenger_impl< tipi::message >::end_point o) {
          if (number_of_connections() == 0) {
            disconnect();

            // unblock all threads waiting for incoming data
            if (current_display) {
              current_display->impl->shutdown();
            }
          }
        }

      public:

        /** \brief Constructor */
        communicator_impl();

        /** \brief Constructor that takes controller connection arguments from the command line */
        bool activate(tipi::tool::communicator*, int&, char** const);

        /** \brief Constructor that takes controller connection arguments from the command line */
        bool activate(tipi::tool::communicator*, char*&);

        /** \brief Attempts to build a connection using a scheme object */
        bool activate(tipi::tool::communicator*, boost::shared_ptr< messaging::scheme > const&, long const&);

        /** \brief Send a layout specification for the display space reserved for this tool */
        void send_display_layout(boost::shared_ptr< communicator_impl >, tool_display&);

        /** \brief Sends the empty layout specification for the display space */
        void send_clear_display();
    };

    /**
     * \attention please use connect() to manually establish a connection with a controller
     **/
    inline communicator_impl::communicator_impl() : current_tool_capabilities() {

      /* Register event handlers for some message types */
      add_handler(tipi::message_capabilities, boost::bind(&communicator_impl::handle_capabilities_request, this, _1));
      add_handler(tipi::message_configuration, boost::bind(&communicator_impl::receive_configuration_handler, this, _1));
    }

    /**
     * \param[in,out] argv a pointer to the list of command line arguments
     **/
    inline bool communicator_impl::activate(tipi::tool::communicator* c, char*& argv) {
      command_line_interface::argument_extractor e(argv);

      return activate(c, e.get_scheme(), e.get_identifier());
    }

    /**
     * \param[in,out] argc the number of command line arguments
     * \param[in,out] argv a pointer to the list of command line arguments
     **/
    inline bool communicator_impl::activate(tipi::tool::communicator* c, int& argc, char** const argv) {
      command_line_interface::argument_extractor e(argc, argv);

      return activate(c, e.get_scheme(), e.get_identifier());
    }

    /**
     * \param[in] s a scheme object that represents the method of connecting
     * \param[in] id an identifier used in the connection
     **/
    inline bool communicator_impl::activate(tipi::tool::communicator* c, boost::shared_ptr< messaging::scheme > const& s, long const& id) {
      if (s.get() != 0) {
        s->connect(*c);

        instance_identifier = id;

        /* Identify the tool instance to the controller */
        tipi::message m(boost::str(boost::format("%u") % id), tipi::message_identification);

        send_message(m);
      }

      return (s.get() != 0);
    }

    inline void communicator_impl::send_clear_display() {
      tool_display display;

      clear_handlers(tipi::message_display_data);

      try {
        tipi::message m(tipi::visitors::store(display), tipi::message_display_layout);

        send_message(m);
      }
      catch (...) {
      }
    }

    /* Send a specification of the tools capabilities */
    inline void communicator_impl::handle_capabilities_request(boost::shared_ptr< const tipi::message >& m) {
      if (m->is_empty()) {
        try {
          tipi::message m(tipi::visitors::store(current_tool_capabilities), tipi::message_capabilities);

          send_message(m);
        }
        catch (...) {
        }
      }
    }

    /**
     * \param[in] m shared pointer reference to an offer_configuration message
     **/
    inline void communicator_impl::receive_configuration_handler(boost::shared_ptr< const tipi::message >& m) {
      if(!(m->get_type() == tipi::message_configuration)){
        mcrl2::runtime_error( "Expected configuration message type ");
      }

      try {
        boost::shared_ptr < configuration > c(new configuration);

        tipi::visitors::restore(*c, m->to_string());

        current_configuration = c;
      }
      catch (...) {
      }
    }
    /// \endcond
  }
}
#endif

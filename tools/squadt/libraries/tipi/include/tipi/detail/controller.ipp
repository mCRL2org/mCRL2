// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file include/tipi/detail/controller.ipp

#ifndef TIPI_CONTROLLER_IPP_
#define TIPI_CONTROLLER_IPP_

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include "tipi/detail/basic_messenger.ipp"
#include "tipi/detail/visitors.hpp"
#include "tipi/controller/communicator.hpp"
#include "tipi/controller/capabilities.hpp"
#include "tipi/configuration.hpp"
#include "tipi/common.hpp"

namespace tipi {
  namespace controller {

    /// \cond INTERNAL_DOCS
    class communicator_impl : public tipi::messaging::basic_messenger_impl< tipi::message > {
      friend class communicator;

      private:

        /** \brief call-back function type for layout changes of the display */
        typedef communicator::display_layout_handler_function display_layout_handler_function;

        /** \brief call-back function type for updates to the display */
        typedef communicator::display_update_handler_function display_update_handler_function;

        /** \brief call-back function type for diagnostic messages */
        typedef communicator::status_message_handler_function status_message_handler_function;

      private:

        /** \brief Creates a new configuration object */
        static boost::shared_ptr < configuration > new_configuration(tipi::tool::capabilities::input_configuration const& c);

      private:

        /** \brief The current configuration of a tool (may be limited to a main input configuration) */
        boost::shared_ptr < configuration >  m_configuration;

        /** \brief Wakes waiters if no connections remain */
        void on_disconnect(messaging::basic_messenger_impl< tipi::message >::end_point) {
        }

      public:

        /** \brief Default constructor */
        communicator_impl();

        /** \brief Clears handlers for display change messages */
        void deactivate_display_layout_handling();

        /** \brief Sets a handler for layout messages using a handler function */
        void activate_display_layout_handling(boost::weak_ptr < communicator_impl >, display_layout_handler_function const&, display_update_handler_function const&);

        /** \brief Clears handlers for status messages */
        void deactivate_status_message_handler();

        /** \brief Sets a handler for layout messages using a handler function */
        void activate_status_message_handler(boost::weak_ptr < communicator_impl >, status_message_handler_function&);
    };

    inline void communicator_impl::deactivate_display_layout_handling() {
      clear_handlers(tipi::message_display_layout);
      clear_handlers(tipi::message_display_data);
    }

    inline void communicator_impl::deactivate_status_message_handler() {
      clear_handlers(tipi::message_report);
    }

    /**
     * \param[in] c the input combination on which to base the new configuration
     **/
    inline boost::shared_ptr < configuration > communicator_impl::new_configuration(tipi::tool::capabilities::input_configuration const& c) {
      boost::shared_ptr < configuration > nc(new tipi::configuration(c.get_category()));

      return (nc);
    }
    /// \endcond
  }
}

#endif

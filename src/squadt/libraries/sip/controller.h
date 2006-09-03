#ifndef SIP_CONTROLLER_H
#define SIP_CONTROLLER_H

#include <vector>

#include <sip/detail/common.h>
#include <sip/detail/report.h>
#include <sip/detail/controller_capabilities.h>
#include <sip/detail/tool_capabilities.h>
#include <sip/detail/layout_tool_display.h>
#include <sip/detail/layout_elements.h>
#include <utility/logger.h>

/* Interface classes for the tool side of the Squadt Interaction Protocol */
namespace sip {
  namespace controller {

    class communicator_impl;

    /**
     * \class communicator controller.h
     * \brief The main interface to the protocol (controller-side)
     **/
    class communicator {
      friend class communicator_impl;

      protected:
 
        /** \brief Implementation object, that contains the real functionality */
        boost::shared_ptr < communicator_impl > impl;
 
        /** \brief The capabilities object of the controller as it is send, when requested */
        static controller::capabilities         current_controller_capabilities;

      public:

        /** \brief Function type that for communicating display layouts */
        typedef boost::function < void (sip::layout::tool_display::sptr) >                     display_layout_handler_function;

        /** \brief Function type that for communicating display layouts */
        typedef boost::function < void (std::vector < sip::layout::element const* > const&) >  display_data_handler_function;

        /** \brief Function type that for communicating display layouts */
        typedef boost::function < void (sip::report::sptr) >                                   status_message_handler_function;

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
        void send_configuration();
 
        /** \brief Send data to update the state of the last communicated display layout */
        void send_display_data(sip::layout::element const*);
 
        /** \brief Sends a message to a tool that it may start processing */
        void send_start_signal();
 
        /** \brief Request the tool to terminate itself */
        void request_termination();
 
        /** \brief Set the current (perhaps partial) configuration */
        void set_configuration(boost::shared_ptr < sip::configuration >);

        /** \brief Creates a new configuration object based on a given input_combination */
        static configuration::sptr new_configuration(sip::tool::capabilities::input_combination const&);
 
        /** \brief Get the current (perhaps partial) configuration */
        configuration::sptr get_configuration() const;

        /** \brief Sets a handler for layout messages using a handler function */
        void activate_display_layout_handler(display_layout_handler_function);

        /** \brief Sets a handler for layout messages using a handler function */
        void activate_display_data_handler(sip::layout::tool_display::sptr, display_data_handler_function);

        /** \brief Sets a handler for layout messages using a handler function */
        void activate_status_message_handler(status_message_handler_function);

        /** \brief Waits for the first message with a specific type to arrive */
        const sip::message_ptr await_message(sip::message::type_identifier_t);

        /** \brief Set the handler for a type */
        void add_handler(const sip::message::type_identifier_t, sip::message_handler_type);

        /** \brief Remove a specific handlers for a message type */
        void remove_handler(const sip::message::type_identifier_t, sip::message_handler_type);

        /** \brief Get the logger instance used for this communicator */
        utility::logger* get_logger() const;

        /** \brief Gets the associated logger object */
        static utility::logger* get_standard_error_logger();
    };
 
    inline const controller::capabilities& communicator::get_controller_capabilities() {
      return (current_controller_capabilities);
    }
  }
}

#endif


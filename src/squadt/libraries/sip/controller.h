#ifndef SIP_CONTROLLER_H
#define SIP_CONTROLLER_H

#include <vector>

#include <sip/detail/common.h>
#include <sip/detail/report.h>
#include <sip/detail/controller_capabilities.h>
#include <sip/detail/tool_capabilities.h>
#include <sip/detail/layout_tool_display.h>
#include <sip/detail/layout_elements.h>

/* Interface classes for the tool side of the Squadt Interaction Protocol */
namespace sip {
  namespace controller {

    /**
     * \class communicator controller.h
     * \brief The main interface to the protocol (controller-side)
     **/
    class communicator : public sip::messenger {

      protected:
        /** \brief Type for keeping protocol phase status */
        enum status {
          status_initialising, ///< \brief No connection with tool yet
          status_clean,        ///< \brief Connection with tool: Phase 0
          status_configured,   ///< \brief Tool has accepted a configuration: Phase 1
          status_started,      ///< \brief Tool is running: Phase 2
          status_reported,     ///< \brief Tool is finished and has send a report: Phase 3
          status_error         ///< \brief An error occurred
        };
 
        /** \brief The current protocol status */
        status                      current_status;
 
        /** \brief The current configuration of a tool (may be limited to a main input configuration) */
        configuration::sptr         current_configuration;
 
      protected:

        /** \brief The capabilities object of the controller as it is send, when requested */
        static controller::capabilities current_controller_capabilities;

      public:

        /** \brief Function type that for communicating display layouts */
        typedef boost::function < void (sip::layout::tool_display::sptr) >                     display_layout_handler_function;

        /** \brief Function type that for communicating display layouts */
        typedef boost::function < void (std::vector < sip::layout::element const* > const&) >  display_data_handler_function;

        /** \brief Function type that for communicating display layouts */
        typedef boost::function < void (sip::report::sptr) >                                   status_message_handler_function;

      protected:

        /** \brief convenience function for use with event handlers */
        inline void set_status(status);

      private:
 
        /** \brief Handler function to replace the current display layout with a new one */
        void display_layout_handler(const messenger::message_ptr&, display_layout_handler_function);
 
        /** \brief Handler function to replace the current display layout with a new one */
        void display_data_handler(const messenger::message_ptr&, sip::layout::tool_display::sptr, display_data_handler_function);

        /** \brief Handler function to replace the current display layout with a new one */
        void status_message_handler(const messenger::message_ptr&, status_message_handler_function);

        /** \brief The current handler for layout change events */
        display_layout_handler_function current_layout_handler;

        /** \brief The current handler for layout state change events */
        display_data_handler_function   current_data_handler;
 
        /** \brief Event handler for storing configurations */
        void store_configuration(const messenger::message_ptr& m);

      public:

        /** \brief Default constructor */
        communicator();
 
        /** \brief Default destructor */
        ~communicator();
 
        /** \brief Send details about the amount of space that the controller currently has reserved for this tool */
        void reply_controller_capabilities();
 
        /** \brief Request the list of basic input configurations */
        void request_tool_capabilities();

        /** \brief Creates a new configuration object based on a given input_combination */
        static configuration::sptr new_configuration(sip::tool::capabilities::input_combination const&);
 
        /** \brief Send a specification of a (perhaps partial) configuration */
        void send_configuration();
 
        /** \brief Send data to update the state of the last communicated display layout */
        template < typename E >
        void send_display_data(E const*);
 
        /** \brief Sends a message to a tool that it may start processing */
        void send_start_signal();
 
        /** \brief Request the tool to terminate itself */
        void request_termination();
 
        /** \brief Get the controller_capabilities object that is send to tools */
        const controller::capabilities& get_controller_capabilities();
 
        /** \brief Set the current (perhaps partial) configuration */
        template < typename T >
        void set_configuration(T);

        /** \brief Get the current (perhaps partial) configuration */
        configuration::sptr get_configuration() const;

        /** \brief Sets a handler for layout messages using a handler function */
        void activate_display_layout_handler(display_layout_handler_function);

        /** \brief Sets a handler for layout messages using a handler function */
        void activate_display_data_handler(sip::layout::tool_display::sptr, display_data_handler_function);

        /** \brief Sets a handler for layout messages using a handler function */
        void activate_status_message_handler(status_message_handler_function);
    };
 
    inline const controller::capabilities& communicator::get_controller_capabilities() {
      return (current_controller_capabilities);
    }
 
    /**
     * @param[in] c the current configuration
     **/
    template < typename T >
    inline void communicator::set_configuration(T c) {
      current_configuration = c;
    }
 
    /** \attention use get_configuration().swap() to set the configuration */
    inline configuration::sptr communicator::get_configuration() const {
      return (current_configuration);
    }
 
    inline void communicator::set_status(status s) {
      current_status = s;
    }

    /**
     * @param[in] e a sip layout element of which the data is to be sent
     **/
    template < typename E >
    void communicator::send_display_data(E const* e) {
      message m(e->get_state(), sip::message_display_data);

      send_message(m);
    }
  }
}

#include <sip/detail/basic_messenger.tcc>

#endif


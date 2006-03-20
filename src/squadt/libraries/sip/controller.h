#ifndef SIP_CONTROLLER_H
#define SIP_CONTROLLER_H

#include <sip/detail/common.h>

/* Interface classes for the tool side of the Squadt Interaction Protocol */
namespace sip {
  namespace controller {

    /** \brief The main interface to the protocol (controller-side) */
    class communicator : public sip::messenger {
      private:
        /** Type for keeping protocol phase status */
        typedef enum {
          status_initialising, ///< \brief No connection with tool yet
          status_clean,        ///< \brief Connection with tool: Phase 0
          status_configured,   ///< \brief Tool has accepted a configuration: Phase 1
          status_started,      ///< \brief Tool is running: Phase 2
          status_reported,     ///< \brief Tool is finished and has send a report: Phase 3
          status_error         ///< \brief An error occurred
        } status;
 
        /** \brief The current protocol status */
        status                      current_status;
 
        /** \brief The currently active display layout */
        sip::layout::display_layout current_layout;
 
        /** \brief The capabilities object of the controller as it is send, when requested */
        controller::capabilities    current_controller_capabilities;
 
        /** \brief The last received capabilities object of the tool */
        tool::capabilities::ptr     current_tool_capabilities;
 
        /** \brief The current configuration of a tool (may be limited to a main input configuration) */
        configuration::ptr          current_configuration;
 
        /** \brief Unique identifier assigned to the peer */
        long instance_identifier;
 
        /** \brief Convenience function for use with event handlers */
        inline void set_status(status);
 
        /** \brief Handler function that sets the identifier for this instance */
        void accept_instance_identifier(messenger::message_ptr&);
 
        /** \brief Handler function that replaces the current tool capabilities object with the one from a message */
        void accept_tool_capabilities(messenger::message_ptr&);
 
        /** \brief Handler function to replace the current display layout with a new one */
        void accept_layout_handler(messenger::message_ptr&);
 
        /** \brief Handler function to map data to the display */
        void accept_data_handler(messenger::message_ptr&);
 
      public:
        /** \brief Default constructor */
        communicator();
 
        /** \brief Default destructor */
        ~communicator();
 
        /** \brief Send details about the amount of space that the controller currently has reserved for this tool */
        void reply_controller_capabilities();
 
        /** \brief Request the list of basic input configurations */
        void request_tool_capabilities();
 
        /** \brief Send a specification of a (perhaps partial) configuration */
        void send_configuration();
 
        /** \brief Send a layout specification for the display space reserved for this tool */
        void send_interaction_data();
 
        /** \brief Send a layout specification for the display space reserved for this tool */
        void send_start_signal();
 
        /** \brief Request the tool to terminate itself */
        void request_termination();
 
        /** \brief Get the controller_capabilities object that is send to tools */
        controller::capabilities& get_controller_capabilities();
 
        /** \brief Get the most recently received tool_capabilities object of the connected tool */
        tool::capabilities::ptr get_tool_capabilities();
 
        /** \brief Set the current configuration (only effective before status_configured) */
        void set_configuration(const configuration&);
 
        /** \brief Get the current (perhaps partial) configuration */
        configuration::ptr get_configuration() const;
    };
 
    inline controller::capabilities& communicator::get_controller_capabilities() {
      return (current_controller_capabilities);
    }
 
    inline tool::capabilities::ptr communicator::get_tool_capabilities() {
      return (current_tool_capabilities);
    }
 
    /** \attention use get_configuration().swap() to set the configuration */
    inline configuration::ptr communicator::get_configuration() const {
      return (current_configuration);
    }
 
    inline void communicator::set_status(status s) {
      current_status = s;
    }
 
    inline void communicator::accept_instance_identifier(messenger::message_ptr& m) {
      instance_identifier = atol(m->to_string().c_str());
 
      current_status = status_clean;
    }
  }
}

#endif


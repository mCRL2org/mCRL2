#ifndef SIP_CONTROLLER_H
#define SIP_CONTROLLER_H

#include <string>
#include <deque>

#include <sip/detail/basic_messenger.h>
#include <sip/detail/configuration.h>
#include <sip/detail/controller_capabilities.h>
#include <sip/detail/common.h>

/* Interface classes for the tool side of the Squadt Interaction Protocol */
namespace sip {

  class controller_communicator : public sip_messenger {
    public:
      typedef controller_capabilities::version            version;

      typedef controller_capabilities::display_dimensions display_dimensions;

    private:
      /** Charactarises the current state */
      typedef enum {
         status_initialising /** \brief No connection with tool yet */
        ,status_clean        /** \brief Connection with tool: Phase 0 */
        ,status_configured   /** \brief Tool has accepted a configuration: Phase 1 */
        ,status_started      /** \brief Tool is running: Phase 2 */
        ,status_reported     /** \brief Tool is finished and has send a report: Phase 3 */
        ,status_error        /** \brief An error occurred */
      } status;

      static const version        current_version;

      /** The current protocol status */
      status                      current_status;

      /** The currently active display layout */
      sip::layout::display_layout current_layout;

      /** The dimensions of the currently reserved display for the connected tool */
      display_dimensions          current_display_dimensions;

      /** The current configuration of a tool (may be limited to a main input configuration) */
      configuration               current_configuration;

      /** Handler function to replace the current display layout with a new one */
      void (*accept_layout_handler)();

      /** Handler function to map data to the display */
      void (*accept_data_handler)();

      /** Triggers event handlers for incoming messages */
      void deliver(std::istream&);

    public:
      inline controller_communicator();

      /** Send details about the amount of space that the controller currently has reserved for this tool */
      void reply_controller_capabilities();

      /** Request the list of basic input configurations */
      void request_tool_capabilities();

      /** Send a specification of a (perhaps partial) configuration */
      void send_configuration();

      /** Send a layout specification for the display space reserved for this tool */
      void send_interaction_data();

      /** Send a layout specification for the display space reserved for this tool */
      void send_start_signal();

      /** Request the tool to terminate itself */
      void request_termination();

      /** Set the amount of display space that is reserved for this tool */
      void set_display_dimensions(unsigned short x, unsigned short y, unsigned short z);

      /** Get a controller_capabilities object that is send to tools */
      controller_capabilities get_capabilities();

      /** Set the current configuration (only effective before status_configured) */
      void set_configuration(const configuration&);

      /** Get the current (perhaps partial) configuration */
      configuration get_configuration() const;

      /** Returns the used protocol version */
      inline controller_communicator::version get_version() const; 
  };

  /** Protocol version {major,minor} */
  const controller_communicator::version controller_communicator::current_version = {1,0};

  inline controller_communicator::controller_communicator() : current_status(status_initialising) {
    current_display_dimensions.x = 0;
    current_display_dimensions.y = 0;
    current_display_dimensions.z = 0;
  }

  inline void controller_communicator::set_display_dimensions(unsigned short x, unsigned short y, unsigned short z) {
    current_display_dimensions.x = x;
    current_display_dimensions.y = y;
    current_display_dimensions.z = z;
  }

  inline controller_capabilities controller_communicator::get_capabilities() {
    controller_capabilities c = controller_capabilities(current_version);

    c.set_display_dimensions(current_display_dimensions);

    return (c);
  }

  inline void controller_communicator::set_configuration(const configuration& c) {
    if (status_configured <= current_status) {
      current_configuration = c;
    }
  }

  inline configuration controller_communicator::get_configuration() const {
    return (current_configuration);
  }

  inline controller_communicator::version controller_communicator::get_version() const {
    return (current_version);
  }
}

#endif


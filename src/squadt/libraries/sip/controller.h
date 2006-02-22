#ifndef SIP_CONTROLLER_H
#define SIP_CONTROLLER_H

#include <sip/detail/common.h>

/* Interface classes for the tool side of the Squadt Interaction Protocol */
namespace sip {

  class controller_communicator : public sip_messenger {
    public:
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

      /** Convenience function for use with event handlers */
      inline void set_status(status);

      /** Handler function to replace the current display layout with a new one */
      void accept_layout_handler(sip_messenger::message_ptr&);

      /** Handler function to map data to the display */
      void accept_data_handler(sip_messenger::message_ptr&);

    public:
      /** Default constructor */
      controller_communicator();

      /** Default destructor */
      ~controller_communicator();

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
  };

  inline void controller_communicator::set_display_dimensions(unsigned short x, unsigned short y, unsigned short z) {
    current_display_dimensions.x = x;
    current_display_dimensions.y = y;
    current_display_dimensions.z = z;
  }

  inline controller_capabilities controller_communicator::get_capabilities() {
    controller_capabilities c = controller_capabilities(protocol_version);

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

  inline void controller_communicator::set_status(status s) {
    current_status = s;
  }
}

#endif


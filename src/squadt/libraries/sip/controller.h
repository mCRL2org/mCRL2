#ifndef SIP_CONTROLLER_H
#define SIP_CONTROLLER_H

#include <sip/detail/common.h>

/* Interface classes for the tool side of the Squadt Interaction Protocol */
namespace sip {

  /** \brief The main interface to the protocol (controller-side) */
  class controller_communicator : public sip_messenger {
    public:
      typedef controller_capabilities::display_dimensions display_dimensions;

    private:
      /** Type for keeping protocol phase status */
      typedef enum {
         status_initialising /** \brief No connection with tool yet */
        ,status_clean        /** \brief Connection with tool: Phase 0 */
        ,status_configured   /** \brief Tool has accepted a configuration: Phase 1 */
        ,status_started      /** \brief Tool is running: Phase 2 */
        ,status_reported     /** \brief Tool is finished and has send a report: Phase 3 */
        ,status_error        /** \brief An error occurred */
      } status;

      /** \brief The protocol version of the peer, if known otherwise assumed 1.0 */
      static const version        peer_version;

      /** \brief The current protocol status */
      status                      current_status;

      /** \brief The currently active display layout */
      sip::layout::display_layout current_layout;

      /** \brief The dimensions of the currently reserved display for the connected tool */
      display_dimensions          current_display_dimensions;

      /** \brief The current configuration of a tool (may be limited to a main input configuration) */
      configuration               current_configuration;

      /** \brief Unique identifier assigned to the peer */
      long instance_identifier;

      /** \brief Convenience function for use with event handlers */
      inline void set_status(status);

      /** \brief Convenience function for use with event handlers */
      void accept_instance_identifier(message_ptr&);

      /** \brief Handler function to replace the current display layout with a new one */
      void accept_layout_handler(sip_messenger::message_ptr&);

      /** \brief Handler function to map data to the display */
      void accept_data_handler(sip_messenger::message_ptr&);

    public:
      /** \brief Default constructor */
      controller_communicator();

      /** \brief Default destructor */
      ~controller_communicator();

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

      /** \brief Set the amount of display space that is reserved for this tool */
      void set_display_dimensions(unsigned short x, unsigned short y, unsigned short z);

      /** \brief Get a controller_capabilities object that is send to tools */
      controller_capabilities get_capabilities();

      /** \brief Set the current configuration (only effective before status_configured) */
      void set_configuration(const configuration&);

      /** \brief Get the current (perhaps partial) configuration */
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

  inline void controller_communicator::accept_instance_identifier(message_ptr& m) {
    instance_identifier = atol(m->to_string().c_str());

    current_status = status_clean;
  }
}

#endif


#ifndef SIP_TOOL_H
#define SIP_TOOL_H

#include <set>

#include <sip/detail/common.h>

/* Interface classes for the tool side of the Squadt Interaction Protocol */
namespace sip {

  class tool_communicator : public sip_messenger {
    public:
      /** Charactarises the current state */
      typedef enum {
         status_initialising /** \brief No connection with controller yet */
        ,status_clean        /** \brief Connection with controller: Phase 0 */
        ,status_configured   /** \brief Tool has accepted a configuration: Phase 1 */
        ,status_started      /** \brief Tool is running: Phase 2 */
        ,status_reported     /** \brief Tool is finished and has send a report: Phase 3 */
        ,status_error        /** \brief An error occurred */
      } status;

    private:

      /** The current protocol status */
      status                         current_status;

      /** The currently active display layout */
      sip::layout::display_layout    current_display_layout;

      /** The last received set of controller capabilities */
      sip::controller_capabilities*  current_capabilities;

      /** A set of available input configurations for this tool */
      std::set < std::pair < tool_category, storage_format > >       current_input_configurations;

    public:

      /** Default constructor */
      tool_communicator();

      /** Default destructor */
      ~tool_communicator();

      /** Request details about the amount of space that the controller currently has reserved for this tool */
      void request_controller_capabilities();

      /** Request the list of basic input configurations */
      void reply_tool_capabilities();

      /** Signal that the current configuration is complete enough for the tool to start processing */
      void send_accept_configuration();

      /** Send a layout specification for the display space reserved for this tool */
      void send_display_data();

      /** Send a layout specification for the display space reserved for this tool */
      void send_display_layout();

      /** Send a signal that the tool is about to terminate */
      void send_signal_termination();

      /** Send a status report to the controller */
      void send_report(sip::report&);

      /** Add an input configuration that will be send when the controller asks for it */
      inline void add_input_configuration(tool_category, storage_format);

      /** Get the last communicated set of controller capabilities */
      inline const controller_capabilities& get_controller_capabilities() const;

      inline layout::display_layout& get_display_layout();

      /** Returns the current status */
      inline tool_communicator::status get_status() const;
  };

  /** \pre{status is status_initialising} */
  inline void tool_communicator::add_input_configuration(tool_category c, storage_format f) {
    assert(current_status == status_initialising);

    std::pair < tool_category, storage_format > p(c,f);

    current_input_configurations.insert(p);
  }

  inline tool_communicator::status tool_communicator::get_status() const {
    return (current_status);
  }

  /** \pre{status is not status_initialising} */
  inline const controller_capabilities& tool_communicator::get_controller_capabilities() const {
    if (current_capabilities == 0) {
      throw (new exception(exception::controller_capabilities_unknown));
    }

    return (*current_capabilities);
  }

  inline layout::display_layout& tool_communicator::get_display_layout() {
    return (current_display_layout);
  }
}

#endif


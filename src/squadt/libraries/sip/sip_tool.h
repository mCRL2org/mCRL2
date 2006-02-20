#ifndef SIP_TOOL_H
#define SIP_TOOL_H

#include <string>
#include <deque>
#include <set>

#include <sip/detail/common.h>
#include <sip/detail/message.h>
#include <sip/detail/controller_capabilities.h>
#include <sip/detail/report.h>
#include <sip/detail/sip_communicator.h>

/* Interface classes for the tool side of the Squadt Interaction Protocol */
namespace sip {

  class tool_communicator : public communicator::sip_communicator {
    public:
      /** Charactarises the current state */
      typedef enum {
         status_initialising /** \brief{No connection with controller yet} */
        ,status_clean        /** \brief{Connection with controller: Phase 0} */
        ,status_configured   /** \brief{Tool has accepted a configuration: Phase 1} */
        ,status_started      /** \brief{Tool is running: Phase 2} */
        ,status_reported     /** \brief{Tool is finished and has send a report: Phase 3} */
        ,status_error        /** \brief{An error occurred} */
      } status;

    private:
      /** The current protocol status */
      status                         current_status;

      /** The currently active display layout */
      sip::layout::display_layout*   current_layout;

      /** The last received set of controller capabilities */
      sip::controller_capabilities*  current_capabilities;

      /** A set of available input configurations for this tool */
      std::set < std::pair < tool_category, storage_format > >       current_input_configurations;

    public:
      tool_communicator();

      /** Request details about the amount of space that the controller currently has reserved for this tool */
      void request_controller_capabilities();

      /** Request the list of basic input configurations */
      void reply_input_configurations();

      /** Send a specification of the current configuration */
      void send_accept_configuration();

      /** Send a layout specification for the display space reserved for this tool */
      void send_display_layout(sip::layout::display_layout);

      /** Send a layout specification for the display space reserved for this tool */
      void send_display_data();

      /** Send a signal that the tool is about to terminate */
      void send_termination();

      /** Signal an error to the controller */
      void send_error(std::string);

      /** Send a status report to the controller */
      void send_report(sip::report&);

      /** Add an input configuration that will be send when the controller asks for it */
      inline void add_input_configuration(tool_category, storage_format);

      /** Get the last communicated set of controller capabilities */
      inline const controller_capabilities& get_controller_capabilities() const;

      /** Returns the current status */
      inline tool_communicator::status get_status() const;
  };

  inline tool_communicator::tool_communicator() : current_status(status_initialising), current_layout(0), current_capabilities(0) {
  }

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
}

#endif


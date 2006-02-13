#ifndef SIP_TOOL_H
#define SIP_TOOL_H

#include <string>
#include <deque>

#include <sip/detail/common.h>
#include <sip/detail/report.h>
#include <sip/detail/sip_communicator.h>

/* Interface classes for the tool side of the Squadt Interaction Protocol */
namespace sip {

  class tool_communicator : public communicator::sip_communicator {
    private:
      /** Charactarises the current state */
      typedef enum {
        status_clean,
        status_configured,
        status_started,
        status_reported,
        status_error
      } status;

      /** The current protocol status */
      status                         current_status;

      /** The currently active display layout */
      sip::layout::layout            current_layout;

      /** The last received set of controller capabilities */
      sip::controller_capabilities   current_capabilities;

    public:
      tool_communicator();

      /** Request details about the amount of space that the controller currently has reserved for this tool */
      void request_reserved_space();

      /** Request the list of basic input configurations */
      void reply_input_configurations();

      /** Send a specification of the current configuration */
      void send_accept_configuration();

      /** Send a layout specification for the display space reserved for this tool */
      void send_display_layout(sip::layout::layout);

      /** Send a layout specification for the display space reserved for this tool */
      void send_display_data();

      /** Send a signal that the tool is about to terminate */
      void send_termination();

      /** Signal an error to the controller */
      void send_error(std::string);

      /** Send a status report to the controller */
      void send_report(sip::report&);
  };

  inline tool_communicator::tool_communicator() : current_status(status_clean), current_layout(0), current_capabilities(0) {
  }
}

#endif


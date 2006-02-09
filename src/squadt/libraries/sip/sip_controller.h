#ifndef SIP_CONTROLLER_H
#define SIP_CONTROLLER_H

#include <string>
#include <deque>

#include <detail/sip_communicator.h>
#include <detail/common.h>

/* Interface classes for the tool side of the Squadt Interaction Protocol */
namespace sip {

  namespace layout {
    class layout {
      private:
      public:
        layout();
    };
  }

  class controller_communicator : public sip_communicator {
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
      status                 current_status;

      /** The currently active display layout */
      sip::layout::layout    current_layout;

    public:
      inline controller_communicator();

      /** Send details about the amount of space that the controller currently has reserved for this tool */
      void reply_reserved_space();

      /** Request the list of basic input configurations */
      void request_input_configurations();

      /** Send a specification of the selected configuration */
      void send_selected_input_configuration();

      /** Send a layout specification for the display space reserved for this tool */
      void send_interaction_data();

      /** Request the tool to terminate itself */
      void request_termination();

      /** Wait until the next message arrives */
      void await_message();
  };

  inline controller_communicator::controller_communicator() : current_status(status_clean) {
  }
}

#endif


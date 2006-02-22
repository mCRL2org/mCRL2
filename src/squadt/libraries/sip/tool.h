#ifndef SIP_TOOL_H
#define SIP_TOOL_H

#include <set>

#include <sip/detail/common.h>

/* Interface classes for the tool side of the Squadt Interaction Protocol */
namespace sip {

  class tool_communicator : public sip_messenger {
    private:
      /** Charactarises the current state */
      typedef enum {
         status_initialising /** \brief No connection with controller yet */
        ,status_clean        /** \brief Connection with controller: Phase 0 */
        ,status_configured   /** \brief Tool has accepted a configuration: Phase 1 */
        ,status_started      /** \brief Tool is running: Phase 2 */
        ,status_reported     /** \brief Tool is finished and has send a report: Phase 3 */
        ,status_error        /** \brief An error occurred */
      } status;

      /** \brief The current protocol status */
      status                         current_status;

      /** \brief The last received set of controller capabilities */
      sip::controller_capabilities*  current_capabilities;

      /** \brief A set of available input configurations for this tool */
      std::set < std::pair < tool_category, storage_format > >       current_input_configurations;

      /** \brief Handler for incoming data resulting from user interaction relayed by the controller */
      void accept_interaction_data(sip_messenger::message_ptr&);

    public:

      /** \brief Default constructor */
      tool_communicator();

      /** \brief Constructor that takes controller connection arguments from the command line */
      tool_communicator(int&, char**);

      /** \brief Default destructor */
      ~tool_communicator();

      /** \brief Request details about the amount of space that the controller currently has reserved for this tool */
      void request_controller_capabilities();

      /** \brief Request the list of basic input configurations */
      void reply_tool_capabilities();

      /** \brief Signal that the current configuration is complete enough for the tool to start processing */
      void send_accept_configuration();

      /** \brief Send a layout specification for the display space reserved for this tool */
      void send_display_data();

      /** \brief Send a layout specification for the display space reserved for this tool */
      void send_display_layout(layout::display_layout&);

      /** \brief Send a signal that the tool is about to terminate */
      void send_signal_termination();

      /** \brief Send a status report to the controller */
      void send_report(sip::report&);

      /** \brief Add an input configuration that will be send when the controller asks for it */
      inline void add_input_configuration(tool_category, storage_format);

      /** \brief Get the last communicated set of controller capabilities */
      inline const controller_capabilities& get_controller_capabilities() const;
  };

  /** \pre status is status_initialising */
  inline void tool_communicator::add_input_configuration(tool_category c, storage_format f) {
    assert(current_status == status_initialising);

    std::pair < tool_category, storage_format > p(c,f);

    current_input_configurations.insert(p);
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


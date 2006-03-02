#ifndef SIP_COMMON_H
#define SIP_COMMON_H

#include <list>

#include <sip/detail/report.h>
#include <sip/detail/configuration.h>
#include <sip/detail/controller_capabilities.h>
#include <sip/detail/display_layout.h>
#include <sip/detail/basic_messenger.h>

/* Interface classes for both the tool and the controller side of the Squadt Interaction Protocol */
namespace sip {

  /** Category name that describes a tools function */
  typedef std::string tool_category;

  /** Storage format for tool input/output configuration specification */
  typedef std::string storage_format;

  /** Type for sip protocol message identification */
  enum sip_message_identifier_t {
     unknown                           /// \brief type is was specified or unknown (or should be derived from content)
    ,send_instance_identifier          /// \brief send the unique identifier assigned to a tool instance
    ,request_controller_capabilities   /// \brief request the controller a description of its capabilities
    ,reply_controller_capabilities     /// \brief send a description of controller capabilities to the tool
    ,request_tool_capabilities         /// \brief request a tool a description of its capabilities
    ,reply_tool_capabilities           /// \brief send a description of tool capabilities to the controller
    ,send_configuration                /// \brief send the selected input configuration to a tool
    ,send_accept_configuration         /// \brief sends the current configuration to the controller
    ,send_signal_start                 /// \brief tool can start operation
    ,send_display_layout               /// \brief send the controller a layout description for the display
    ,send_display_data                 /// \brief send the controller a data to be displayed using the current display layout
    ,send_interaction_data             /// \brief send a tool data from user interaction via the display associated with this tool
    ,request_termination               /// \brief send a tool the signal to end processing and terminate as soon as possible
    ,send_signal_termination           /// \brief send the controller the signal that the tool is about to terminate
    ,send_report                       /// \brief send the controller a report of a tools operation
  };

  /** A message type for communication of sip protocol messages */
  typedef messenger::message < sip_message_identifier_t, unknown > sip_message;

  /** A messenger type for communication of sip protocol messages */
  typedef messenger::basic_messenger < sip_message >               sip_messenger;
}

#endif


#ifndef SIP_COMMON_H
#define SIP_COMMON_H

#include <list>

#include <boost/bind.hpp>

#include <sip/detail/basic_messenger.h>

/* Interface classes for both the tool and the controller side of the Squadt Interaction Protocol */
namespace sip {

  /** \brief Category name that describes a tools function */
  typedef std::string tool_category;

  /** \brief Storage format for tool input/output configuration specification */
  typedef std::string storage_format;

  /** \brief Type for sip protocol message identification */
  enum message_identifier_t {
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
    ,request_termination               /// \brief send a tool the signal to end processing and terminate as soon as possible
    ,send_signal_termination           /// \brief send the controller the signal that the tool is about to terminate
    ,send_report                       /// \brief send the controller a report of a tools operation
  };

  /** \brief A message type for communication of sip protocol messages */
  typedef messaging::message < message_identifier_t, unknown > message;

  /** \brief A convenience type to share the boost shared pointer implementation */
  typedef boost::shared_ptr < message >                        message_ptr;

  /** \brief A messenger type for communication of sip protocol messages */
  typedef messaging::basic_messenger < sip::message >          messenger;

  /** \brief Type for protocol version */
  struct version {
    unsigned char major; ///< the major number
    unsigned char minor; ///< the minor number
  };

  /** \brief Protocol version {major,minor} */
  static const version default_protocol_version = {1,0};

  /** \brief Convenience type for connection end points (primarily used by delivery handler functions) */
  typedef const transport::transceiver::basic_transceiver* end_point;
}

#endif


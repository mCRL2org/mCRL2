#ifndef SIP_COMMON_H_
#define SIP_COMMON_H_

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
     message_unknown,                          ///< \brief unspecified or unknown (or should be derived from content)
     message_any,                              ///< \brief matches any of the types below
     message_instance_identifier,              ///< \brief the unique identifier assigned to a tool instance
     message_request_controller_capabilities,  ///< \brief request the controller a description of its capabilities
     message_reply_controller_capabilities,    ///< \brief a description of controller capabilities to the tool
     message_request_tool_capabilities,        ///< \brief request a tool a description of its capabilities
     message_reply_tool_capabilities,          ///< \brief a description of tool capabilities to the controller
     message_offer_configuration,              ///< \brief the selected input configuration to a tool
     message_accept_configuration,             ///< \brief the current configuration to the controller
     message_signal_start,                     ///< \brief tool can start operation
     message_signal_done,                      ///< \brief tool has finished an operation
     message_display_layout,                   ///< \brief the controller a layout description for the display
     message_display_data,                     ///< \brief the controller a data to be displayed using the current display layout
     message_request_termination,              ///< \brief send a tool the signal to end processing and terminate as soon as possible
     message_signal_termination,               ///< \brief send the controller the signal that the tool is about to terminate
     message_report                            ///< \brief send the controller a report of a tools operation
  };

  /** \brief A message type for communication of sip protocol messages */
  typedef messaging::message < message_identifier_t, message_unknown, message_any > message;

  /** \brief A convenience type to share the boost shared pointer implementation */
  typedef boost::shared_ptr < message >                                             message_ptr;

  /** \brief A messenger type for communication of sip protocol messages */
  typedef messaging::basic_messenger < sip::message >                               messenger;

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


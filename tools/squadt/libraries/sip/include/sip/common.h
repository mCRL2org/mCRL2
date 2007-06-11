#ifndef SIP_COMMON_H_
#define SIP_COMMON_H_

#include <list>

#include <boost/function.hpp>
#include <sip/detail/message.h>
#include <sip/detail/basic_messenger.h>

/* Interface classes for both the tool and the controller side */
namespace sip {

  class visitors;
  class report;
  class configuration;
  class store_visitor;
  class store_visitor_impl;
  class restore_visitor;
  class restore_visitor_impl;

  namespace controller {
    class communicator_impl;
  }

  namespace tool {
    class communicator_impl;
  }

  /** Helper function to invoke the as_string method a type T */
  template < typename T > std::string as_string(T const& t) {
    return (as_string(t));
  }

  /** \brief Type for sip protocol message identification */
  enum message_identifier_t {
     message_unknown,                          ///< \brief unspecified or unknown (or should be derived from content)
     message_any,                              ///< \brief matches any of the types below
     message_instance_identification,          ///< \brief the unique identifier assigned to a tool instance
     message_controller_capabilities,          ///< \brief request/response of controller capabilities
     message_tool_capabilities,                ///< \brief request/response of tool capabilities
     message_configuration,                    ///< \brief offer/accept tool configuration
     message_task_start,                       ///< \brief signal to a tool for starting a task
     message_task_done,                        ///< \brief signal to controller that task execution has finished
     message_display_layout,                   ///< \brief the controller a layout description for the display
     message_display_update,                   ///< \brief the controller a data to be displayed using the current display layout
     message_termination,                      ///< \brief request/response of tool termination
     message_report                            ///< \brief send the controller a report of a tools operation
  };

  template <> std::string as_string(message_identifier_t const&);

  /** \brief A message type for communication of sip protocol messages */
  typedef messaging::message < message_identifier_t, message_unknown, message_any > message;

  /** \brief A convenience type to share the boost shared pointer implementation */
  typedef boost::shared_ptr < message >                                             message_ptr;

  /** \brief A messenger type for communication of sip protocol messages */
  typedef messaging::basic_messenger < sip::message >                               messenger;

  /** \brief Convenience type for message delivery handlers */
  typedef boost::function < void (boost::shared_ptr < message > const&) >           message_handler_type;

  /** \brief Category name that describes a tools function */
  typedef std::string tool_category;

  /** \brief Storage format for tool input/output configuration specification */
  typedef std::string storage_format;

  /** \brief Type for protocol version */
  struct version {
    unsigned char major; ///< the major number
    unsigned char minor; ///< the minor number
  };

  /** \brief Protocol version {major,minor} */
  const version default_protocol_version = {1,0};
}

#endif

#ifdef BASIC_MESSENGER_H
#ifndef SIP_COMMON_MESSENGER_H
#define SIP_COMMON_MESSENGER_H

namespace sip {

  /** \brief Convenience type alias for connection end points (primarily used by delivery handler functions) */
  typedef const transport::transceiver::basic_transceiver* end_point;
}

#endif
#endif

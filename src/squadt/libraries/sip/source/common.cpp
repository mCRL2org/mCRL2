#define SIP_IMPORT_STATIC_DEFINITIONS

#include <sip/configuration.h>
#include <sip/exception.h>
#include <sip/tool/category.h>
#include <sip/detail/basic_messenger.h>
#include <sip/detail/common.h>

namespace sip {
  namespace messaging {

    /** \brief The type identifier for messages of which the type is not known */
    template < >
    const sip::message_identifier_t message< sip::message_identifier_t, sip::message_unknown, sip::message_any >::message_unknown = sip::message_unknown;

    /** \brief The type identifier for messages of any type */
    template < >
    const sip::message_identifier_t message< sip::message_identifier_t, sip::message_unknown, sip::message_any >::message_any     = sip::message_any;
  }

  template < >
  std::string as_string(message_identifier_t const& t) {
    static char const* message_identifier_strings[] = {
      "unknown",
      "any",
      "instance_identification",
      "request_controller_capabilities",
      "response_controller_capabilities",
      "request_tool_capabilities",
      "response_tool_capabilities",
      "offer_configuration",
      "accept_configuration",
      "signal_start",
      "signal_done",
      "display_layout",
      "display_update",
      "request_termination",
      "signal_termination",
      "report"
    };

    return (message_identifier_strings[t]);
  }

  /** \brief global variable that indicates whether or not the library was initialised */
  bool initialised = datatype::basic_datatype::initialise();
}


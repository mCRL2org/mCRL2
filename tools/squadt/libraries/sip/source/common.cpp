#define SIP_IMPORT_STATIC_DEFINITIONS

#include <sip/configuration.h>
#include <sip/tool/category.h>
#include <sip/detail/basic_messenger.h>
#include <sip/common.h>

namespace sip {
  namespace messaging {

    /** The type identifier for messages of which the type is not known */
    template < >
    const sip::message_identifier_t message< sip::message_identifier_t, sip::message_unknown, sip::message_any >::message_unknown = sip::message_unknown;

    /** The type identifier for messages of any type */
    template < >
    const sip::message_identifier_t message< sip::message_identifier_t, sip::message_unknown, sip::message_any >::message_any     = sip::message_any;
  }

  /** \brief textual identifiers for each element of message_identifier_t */
  template < >
  std::string as_string(message_identifier_t const& t) {
    static char const* message_identifier_strings[] = {
      "unknown",
      "any",
      "instance_identification",
      "controller_capabilities",
      "tool_capabilities",
      "configuration",
      "task_start",
      "task_done",
      "display_layout",
      "display_update",
      "termination",
      "report"
    };

    return (message_identifier_strings[t]);
  }
}


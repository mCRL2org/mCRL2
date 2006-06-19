#define IMPORT_STATIC_MEMBERS

#include <sip/detail/configuration.h>
#include <sip/detail/common.h>

namespace sip {

  /** \brief The type identifier for messages of which the type is not known */
  const sip::message_identifier_t message::message_unknown = sip::message_unknown;

  /** \brief The type identifier for messages of any type */
  const sip::message_identifier_t message::message_any     = sip::message_any;
}


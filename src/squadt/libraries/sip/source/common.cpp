#define IMPORT_STATIC_MEMBERS

#include <sip/detail/configuration.h>
#include <sip/detail/common.h>

namespace sip {

  /** The optional constraint, option is either not present or only present once */
  const configuration::option_constraint configuration::constrain_optional = {0,1};

  /** The required constraint, option (with possible arguments) must be present */
  const configuration::option_constraint configuration::constrain_required = {1,1};

  /** \brief The type identifier for messages of which the type is not known */
  template <>
  const message::type_identifier_t message::message_unknown = message_unknown;
 
  /** \brief The type identifier for messages of any type */
  template <>
  const message::type_identifier_t message::message_any     = message_any;
}


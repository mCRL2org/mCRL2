#include <sip/detail/exception.h>

namespace sip {

  const char* exception::descriptions[] = {
    "Message contains a forbidden character sequence.",
    "A message of this type cannot follow the previously send message.",
    "Unknown type encountered in tool configuration."
  };
}


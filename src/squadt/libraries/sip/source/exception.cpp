#include <sip/detail/exception.h>

namespace sip {

  const char* exception::descriptions[] = {
    "Expected the start of message sequence.",
    "Expected the end of message sequence.",
    "Message contains a forbidden character sequence.",
    "A message of this type cannot be send in this context.",
    "Unknown type encountered in tool configuration.",
    "Controller capabilities have not yet been communicated."
  };
}


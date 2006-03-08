#include <sip/detail/exception.h>

namespace sip {

  template < >
  const char* const exception::descriptions[] = {
    "Functionality not yet implemented!",
    "Expected the start of message sequence.",
    "Expected the end of message sequence.",
    "Message contains the forbidden character sequence: `%1'.",
    "A message of type `%1' cannot be send in this context.",
    "Missing required attribute `%1%' in context `%2%'.",
    "Unknown type encountered for `%1%' in context `%2%'.",
    "Controller capabilities have not yet been communicated.",
    "Parse error!",
    "Parse error: expected `%1%' before `%2%'."
  };
}


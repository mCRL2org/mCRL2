#include "exception.h"

namespace squadt {

  template < >
  const char* const exception::descriptions[] = {
    "Cannot write to user settings directory ('%s').",
    "Cannot load tool configuration ('%s').",
    "Execution of program `%s' failed miserably."
  };
}

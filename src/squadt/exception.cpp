#include "exception.h"

namespace squadt {

  template < >
  const char* const exception::descriptions[] = {
    "Cannot write to user settings directory ('%s').",
    "Failed to load %s from `%s'.",
    "Values for required attributes are missing in element `%s'.",
    "Requested tool `%s' is not available!"
    "Execution of program `%s' failed miserably."
  };
}

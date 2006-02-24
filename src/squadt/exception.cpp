#include "exception.h"

namespace squadt {

  const char* const exception::descriptions[] = {
    "Cannot write to user settings directory ('%s').",
    "Cannot load tool configuration ('%s')."
  };
}

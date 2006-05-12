#include <xml2pp/detail/exception.h>

namespace xml2pp {
  namespace exception {

    template < >
    const char* const exception::descriptions[] = {
      "Unable to read specified file: `%1%'!",
      "Unable to initialise reader.",
      "Attempted illegal operation after the first read.",
      "Parse error in XML document.",
      "End of stream was reached."
    };
  }
}


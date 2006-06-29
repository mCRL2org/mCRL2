#include <transport/detail/exception.h>

namespace transport {

  namespace exception {
    template < >
    const char* const exception::descriptions[] = {
      "Listener failed connection attempt!",
      "Transceiver failed send or receive!"
    };
  }
}


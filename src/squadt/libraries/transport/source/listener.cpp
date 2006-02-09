#include <detail/listener.h>

namespace transport {
  namespace listener {

    exception::exception(Listener* alistener) : l(alistener) {
    }

    transport::exception::exception_type exception::type() {
      return (listener_failure);
    }

    Listener* exception::listener() {
      return (l);
    }
  }
}


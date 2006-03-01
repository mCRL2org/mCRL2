#include <transport/detail/listener.h>

namespace transport {
  namespace listener {

    exception::exception(basic_listener* alistener) : l(alistener) {
    }

    transport::exception::exception_type exception::type() {
      return (listener_failure);
    }

    basic_listener* exception::originator() {
      return (l);
    }
  }
}


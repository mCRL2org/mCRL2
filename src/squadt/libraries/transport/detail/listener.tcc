#ifndef TRANSPORT_LISTENER_TCC
#define TRANSPORT_LISTENER_TCC

#include <transport/detail/listener.tcc>

namespace transport {
  namespace listener {

    inline void basic_listener::associate(transceiver::basic_transceiver::ptr t) {
      owner.associate(t);
    }

    inline basic_listener::basic_listener(transporter& m) : owner(m) {
    }

    inline basic_listener::~basic_listener() {
    }
  }
}

#endif


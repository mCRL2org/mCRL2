#ifndef TRANSPORT_TRANSCEIVER_TCC
#define TRANSPORT_TRANSCEIVER_TCC

#include <transport/transporter.h>

namespace transport {
  namespace transceiver {

    inline transporter const* basic_transceiver::get_owner() const {
      return (owner);
    }

    inline basic_transceiver::basic_transceiver(transporter* o) : owner(o) {
    }

    inline void basic_transceiver::deliver(std::istream& input) {
      owner->deliver(input, this);
    }

    inline void basic_transceiver::handle_disconnect(basic_transceiver* t) {
      /* Remove instance from the list of connections */
      owner->disassociate(t);
    }

    inline basic_transceiver::~basic_transceiver() {
    }
  }
}

#endif


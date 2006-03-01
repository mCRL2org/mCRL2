#include <iostream>

#include <transport/transporter.h>
#include <transport/detail/transceiver.h>

namespace transport {
  namespace transceiver {

    exception::exception(basic_transceiver* atransceiver) : t(atransceiver) {
    }

    transport::exception::exception_type exception::type() {
      return (transceiver_closed);
    }

    basic_transceiver* exception::originator() {
      return (t);
    }

    void basic_transceiver::handle_disconnect(basic_transceiver* t) {
      /* Remove instance from the list of connections */
      transporter::connection_list::iterator i = owner.connections.begin();
      transporter::connection_list::iterator b = owner.connections.end();

      while (i != b) {
        if ((*i).get() == t) {
          owner.connections.erase(i);

          break;
        }

        ++i;
      }
    }
  }
}


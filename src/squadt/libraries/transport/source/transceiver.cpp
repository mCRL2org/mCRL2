#include <transporter.h>
#include <detail/transceiver.h>
#include <iostream>

namespace transport {
  namespace transceiver {

    exception::exception(Transceiver* atransceiver) : t(atransceiver) {
    }

    transport::exception::exception_type exception::type() {
      return (transceiver_closed);
    }

    Transceiver* exception::transceiver() {
      return (t);
    }

    void Transceiver::handle_disconnect(Transceiver* t) {
      /* Remove instance from the list of connections */
      transporter::ConnectionList::iterator i = owner.connections.begin();
      transporter::ConnectionList::iterator b = owner.connections.end();

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


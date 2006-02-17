#ifndef SOCKET_LISTENER_H
#define SOCKET_LISTENER_H

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <transport/detail/listener.h>

namespace transport {
  namespace transceiver {
    class SocketTransceiver;
  }

  namespace listener {
    using transceiver::SocketTransceiver;

    class SocketListener : public Listener {
      private:
        /** The socket listener */
        asio::socket_acceptor    acceptor;

        /** For mutual exclusive event handling */
        asio::locking_dispatcher dispatcher;

        void handle_accept(const asio::error&, boost::shared_ptr < SocketTransceiver >, transporter::ListenerPtr);

      public:

        /** Constructor */
        SocketListener(transport::transporter& m, const address& address, const long default_port);

        /** Activate the listener */
        void activate(transporter::ListenerPtr);

        /** Schedule shutdown of listener */
        void shutdown();

        /** Destructor */
        virtual inline ~SocketListener();
    };

    inline SocketListener::~SocketListener() {
    }

    inline void SocketListener::shutdown() {
      acceptor.close();
    }
  }
}

#endif

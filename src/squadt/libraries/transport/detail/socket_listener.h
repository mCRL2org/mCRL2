#ifndef SOCKET_LISTENER_H
#define SOCKET_LISTENER_H

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <transport/detail/listener.h>

namespace transport {
  namespace transceiver {
    class socket_transceiver;
  }

  namespace listener {

    class socket_listener : public basic_listener {
      private:
        /** The socket listener */
        asio::socket_acceptor    acceptor;

        /** For mutual exclusive event handling */
        asio::locking_dispatcher dispatcher;

        void handle_accept(const asio::error&, boost::shared_ptr < transceiver::socket_transceiver >, transporter::listener_ptr);

      public:

        /** Constructor */
        socket_listener(transport::transporter& m, const address& address, const long default_port);

        /** Activate the listener */
        void activate(transporter::listener_ptr);

        /** Schedule shutdown of listener */
        void shutdown();

        /** Destructor */
        virtual inline ~socket_listener();
    };

    inline socket_listener::~socket_listener() {
    }

    inline void socket_listener::shutdown() {
      acceptor.close();
    }
  }
}

#endif

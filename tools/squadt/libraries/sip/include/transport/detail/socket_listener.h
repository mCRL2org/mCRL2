#ifndef SOCKET_LISTENER_H
#define SOCKET_LISTENER_H

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <transport/detail/basics.h>
#include <transport/detail/socket_transceiver.h>
#include <transport/detail/listener.tcc>

namespace transport {
  namespace listener {

    class socket_listener : public basic_listener {

      private:

        /** \brief The socket listener */
        boost::asio::ip::tcp::acceptor      acceptor;

        /** \brief For mutual exclusive event handling */
        boost::asio::strand                 dispatcher;

      private:

        /** \brief Handler for incoming socket connections */
        void handle_accept(const boost::system::error_code&, transceiver::socket_transceiver::ptr, basic_listener::ptr);

      public:

        /** \brief Constructor */
        socket_listener(transport::transporter&, ip_address_t const&, port_t const& = 0);

        /** \brief Activate the listener */
        void activate(basic_listener::ptr);

        /** \brief Schedule shutdown of listener */
        void shutdown();

        /** \brief Destructor */
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

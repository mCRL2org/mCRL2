#ifndef SOCKET_LISTENER_H
#define SOCKET_LISTENER_H

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio/ip/address.hpp>

#include <transport/detail/listener.tcc>
#include <transport/detail/socket_transceiver.h>

namespace transport {
  namespace listener {

    class socket_listener : public basic_listener {
      public:

        /** Convenience type */
        typedef boost::asio::ip::address   address;

      private:

        /** \brief The socket listener */
        boost::asio::ip::tcp::acceptor     acceptor;

        /** \brief For mutual exclusive event handling */
        boost::asio::strand                dispatcher;

      private:

        /** \brief Handler for incoming socket connections */
        void handle_accept(const boost::asio::error&, transceiver::socket_transceiver::ptr, basic_listener::ptr);

      public:

        /** \brief Constructor */
        socket_listener(transport::transporter&, address const&, const short = 0);

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

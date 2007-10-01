//  Copyright 2007 Jeroen van der Wulp. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file transport/detail/socket_listener.hpp

#ifndef SOCKET_LISTENER_H
#define SOCKET_LISTENER_H

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "transport/detail/socket_transceiver.hpp"
#include <transport/detail/listener.tcc>

namespace transport {
  /// \cond INTERNAL_DOCS
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
        socket_listener(boost::shared_ptr < transport::transporter_impl > const&, boost::asio::ip::address const&, short int const& = 0);

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
  /// \endcond
}

#endif

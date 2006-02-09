//
// datagram_socket.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2005 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_DATAGRAM_SOCKET_HPP
#define ASIO_DATAGRAM_SOCKET_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/asio/detail/push_options.hpp>

#include <boost/asio/basic_datagram_socket.hpp>
#include <boost/asio/datagram_socket_service.hpp>

namespace asio {

/// Typedef for the typical usage of datagram_socket.
typedef basic_datagram_socket<datagram_socket_service<> > datagram_socket;

} // namespace asio

#include <boost/asio/detail/pop_options.hpp>

#endif // ASIO_DATAGRAM_SOCKET_HPP

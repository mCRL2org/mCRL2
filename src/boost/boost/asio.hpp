//
// asio.hpp
// ~~~~~~~~
//
// Copyright (c) 2003-2005 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_HPP
#define ASIO_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/asio/basic_datagram_socket.hpp>
#include <boost/asio/basic_deadline_timer.hpp>
#include <boost/asio/basic_demuxer.hpp>
#include <boost/asio/basic_locking_dispatcher.hpp>
#include <boost/asio/basic_socket_acceptor.hpp>
#include <boost/asio/basic_stream_socket.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/buffered_read_stream_fwd.hpp>
#include <boost/asio/buffered_read_stream.hpp>
#include <boost/asio/buffered_stream_fwd.hpp>
#include <boost/asio/buffered_stream.hpp>
#include <boost/asio/buffered_write_stream_fwd.hpp>
#include <boost/asio/buffered_write_stream.hpp>
#include <boost/asio/completion_condition.hpp>
#include <boost/asio/datagram_socket.hpp>
#include <boost/asio/datagram_socket_service.hpp>
#include <boost/asio/deadline_timer_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/demuxer.hpp>
#include <boost/asio/demuxer_service.hpp>
#include <boost/asio/error_handler.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/ipv4/address.hpp>
#include <boost/asio/ipv4/basic_host_resolver.hpp>
#include <boost/asio/ipv4/host.hpp>
#include <boost/asio/ipv4/host_resolver.hpp>
#include <boost/asio/ipv4/host_resolver_service.hpp>
#include <boost/asio/ipv4/multicast.hpp>
#include <boost/asio/ipv4/tcp.hpp>
#include <boost/asio/ipv4/udp.hpp>
#include <boost/asio/is_read_buffered.hpp>
#include <boost/asio/is_write_buffered.hpp>
#include <boost/asio/locking_dispatcher.hpp>
#include <boost/asio/locking_dispatcher_service.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/service_factory.hpp>
#include <boost/asio/socket_acceptor.hpp>
#include <boost/asio/socket_acceptor_service.hpp>
#include <boost/asio/socket_base.hpp>
#include <boost/asio/stream_socket.hpp>
#include <boost/asio/stream_socket_service.hpp>
#include <boost/asio/system_exception.hpp>
#include <boost/asio/thread.hpp>
#include <boost/asio/time_traits.hpp>
#include <boost/asio/write.hpp>

#endif // ASIO_HPP

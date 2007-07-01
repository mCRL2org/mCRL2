//  Copyright 2007 Jeroen van der Wulp. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file include/transport/detail/basics.h

#ifndef TRANSPORT_BASICS_H__
#define TRANSPORT_BASICS_H__

#include <boost/asio/ip/address.hpp>

namespace transport {

  /** \brief IP version 4 address verifier (refer to the boost::asio documentation) */
  typedef boost::asio::ip::address ip_address_t;

  /** \brief Used for host name specification */
  typedef std::string              host_name_t;

  /** \brief Used for host name specification */
  typedef short int                port_t;

  /** \brief The loopback address */
  const ip_address_t ip_loopback = boost::asio::ip::address_v4::loopback();

  /** \brief Binds all interfaces */
  const ip_address_t ip_any      = boost::asio::ip::address_v4::any();
}

#endif

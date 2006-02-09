//
// demuxer.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2003-2005 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_DEMUXER_HPP
#define ASIO_DEMUXER_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/asio/detail/push_options.hpp>

#include <boost/asio/basic_demuxer.hpp>
#include <boost/asio/demuxer_service.hpp>

namespace asio {

/// Typedef for typical usage of demuxer.
typedef basic_demuxer<demuxer_service<> > demuxer;

} // namespace asio

#include <boost/asio/detail/pop_options.hpp>

#endif // ASIO_DEMUXER_HPP

//  Copyright 2007 Jeroen van der Wulp. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file include/transport/detail/listener.tcc

#ifndef TRANSPORT_LISTENER_TCC
#define TRANSPORT_LISTENER_TCC

#include <transport/detail/listener.tcc>

namespace transport {
  namespace listener {

    inline void basic_listener::associate(transceiver::basic_transceiver::ptr t) {
      owner.associate(t);
    }

    inline basic_listener::basic_listener(transporter& m) : owner(m) {
    }

    inline basic_listener::~basic_listener() {
    }
  }
}

#endif


// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file transport/detail/listener.ipp

#ifndef TRANSPORT_LISTENER_TCC
#define TRANSPORT_LISTENER_TCC

#include <tipi/detail/transport/detail/listener.ipp>

namespace transport {
  /// \cond INTERNAL_DOCS
  namespace listener {

    inline void basic_listener::associate(transceiver::basic_transceiver::ptr t) {
      boost::shared_ptr < transporter_impl > c(owner.lock());

      if (c.get()) {
        c->associate(c,t);
      }
    }

    inline basic_listener::basic_listener(boost::shared_ptr < transporter_impl > const& m) : owner(m) {
    }

    inline basic_listener::~basic_listener() {
    }
  }
  /// \endcond
}

#endif


//  Copyright 2007 Jeroen van der Wulp. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file include/transport/detail/transceiver.tcc

#ifndef TRANSPORT_TRANSCEIVER_TCC
#define TRANSPORT_TRANSCEIVER_TCC

#include "transport/detail/transporter.tcc"

namespace transport {
  namespace transceiver {

    inline transporter_impl const* basic_transceiver::get_owner() const {
      return (owner.lock().get());
    }

    /**
     * @param o a pointer to the owner
     **/
    inline basic_transceiver::basic_transceiver(boost::shared_ptr < transporter_impl > const& o) : owner(o) {
      assert(o.get() != 0);
    }

    /**
     * @param d an string that represents the data to be delivered
     **/
    inline void basic_transceiver::deliver(const std::string& d) {
      boost::shared_ptr < transporter_impl > o(owner.lock());

      if (o.get()) {
        o->deliver(d, this);
      }
    }

    /**
     * @param d an input stream with the data that is to be delivered
     **/
    inline void basic_transceiver::deliver(std::istream& d) {
      boost::shared_ptr < transporter_impl > o(owner.lock());

      if (o.get()) {
        o->deliver(d, this);
      }
    }

    /**
     * @param t a pointer to the end point of which the connection is termineted
     **/
    inline void basic_transceiver::handle_disconnect(basic_transceiver* t) {
      boost::shared_ptr < transporter_impl > o(owner.lock());

      /* Remove instance from the list of connections */
      if (o.get()) {
        o->disassociate(t);
      }
    }

    inline basic_transceiver::~basic_transceiver() {
    }
  }
}

#endif


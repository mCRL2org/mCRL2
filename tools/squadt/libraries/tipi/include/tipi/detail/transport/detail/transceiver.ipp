// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file transport/detail/transceiver.ipp

#ifndef TRANSPORT_TRANSCEIVER_TCC
#define TRANSPORT_TRANSCEIVER_TCC

#include <tipi/detail/transport/detail/transporter.ipp>

namespace transport {
  namespace transceiver {

    inline transporter_impl const* basic_transceiver::get_owner() const {
      return (owner.lock().get());
    }

    /**
     * @param o a pointer to the owner
     **/
    inline basic_transceiver::basic_transceiver(boost::shared_ptr < transporter_impl > const& o) : owner(o) {
      if(!(o.get() != 0)){
        throw std::runtime_error( "Transporter implementation is NULL" );
      }
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

